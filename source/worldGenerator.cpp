#include "worldGenerator.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "datastorage.hpp"
#include "spriteutils.hpp"

WorldGenerator::WorldGenerator()
{
}

void WorldGenerator::init()
{
    SpritePtr voronoi_cells = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("voronoi_cells", voronoi_cells);
    SpritePtr perlinnoise = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("perlinnoise", perlinnoise);
    SpritePtr blurred = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("voronoiblurred", blurred);
    SpritePtr perlinblurred = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("perlinblurred", blurred);

}

void WorldGenerator::generate()
{
}

void WorldGenerator::formSuperRegions()
{
    std::cout << "+WorldGenerator: Combining images" << std::endl;
    auto cells = app.getDataStorage()->getImage("voronoi_cells");
    auto perlinblurred = app.getDataStorage()->getImage("perlinblurred");
    auto perlin = app.getDataStorage()->getImage("perlinnoise");
    auto heightmap = app.getDataStorage()->getImage("heightmap");
    auto voronoiblurred = app.getDataStorage()->getImage("voronoiblurred");
    if (heightmap == nullptr)
    {
        heightmap = ImagePtr(new sf::Image());
        heightmap->create(cells->getSize().x, cells->getSize().y);
        app.getDataStorage()->storeImage("heightmap", heightmap);
    }

    sf::Color perlin_col;
    sf::Color voronoi_col;
    sf::Color blurred_voronoi_col;
    sf::Color blurred_perlin_col;
    
    sf::Color output_col;
    
    for (int i = 0; i < cells->getSize().x; i++)
    {
        for (int j = 0; j < cells->getSize().y; j++)
        {
            perlin_col = perlin->getPixel(i, j);
            blurred_voronoi_col = voronoiblurred->getPixel(i, j);
            blurred_perlin_col = perlinblurred->getPixel(i, j);
            voronoi_col = cells->getPixel(i, j);

            // The blurred large voronoi cells form the basic oceans / continents
            // Possible values are 0.10, 0.20, 0.25, which are blurred smoothly
            float height = blurred_voronoi_col.g / 255.0;
            // Perlin contains values 0.1, 0.5, 1.0, which have been blurred to form smooth areas
            float perlin_multiplier = blurred_perlin_col.r / 255.0;

            // To the height the blurred perlin noise is added to form some fractaline coastline / islands
            height = height * (app.getToolbox()->linearInterpolate(0.8, 2.10, perlin_multiplier));

            // The smaller voronoi cells are in range of 0.1 - 0.5
            // 0.1: lowlands
            // 0.5: flats
            // 0.7: hills
            // 1.0 mountains
            // First interpolate the small region value to range 0.8-1.2)
            // Take the existing height and multiply it by the small cell height
            float region_multiplier_1 = blurred_voronoi_col.r / 255.0;
            float region_multiplier_2 = voronoi_col.r / 255.0;
            height = height * (app.getToolbox()->linearInterpolate(0.5, 1.2, region_multiplier_1));// * (app.getToolbox()->linearInterpolate(0.99, 1.01, region_multiplier_2)) ;

            // Temperature zones are affected by the following: Latitude, height, mountains, oceanic currents
            // Latitude: In DF style, pick one end of the map by random and use that for cold. Use the other as hot
            // Height: Decrease temperature based on height using some kind of logarithmic function
            // Mountains: Split temperature zones near mountains
            // Oceanic currents: Generate some random streams that increase the temperature on some areas of the world

            if (height < 0.17)
            {
                // Ocean
                output_col.r = 0;
                output_col.b = 50;
                output_col.g = 0;
                output_col.a = 255;
                sea_pixels.push_back(sf::Vector2i(i, j));
            }
            else
            {
                // Land

                // Add some more perlin noise to spice things up a tiny bit
                height *= app.getToolbox()->linearInterpolate(0.9, 1.1, (blurred_perlin_col.r / 255.0));
                output_col.r = app.getToolbox()->linearInterpolate(0.05, 1.0, height) * 255;
                output_col.g = app.getToolbox()->linearInterpolate(0.05, 1.0, height) * 255;
                output_col.b = app.getToolbox()->linearInterpolate(0.05, 1.0, height) * 255;
            }

            heightmap->setPixel(i, j, output_col);
        }
    }

    app.getSpriteUtils()->setPixels(app.getDataStorage()->getSprite("heightmap"), "heightmap", heightmap);
}

const int dx[] = {+1, 0, -1, 0};
const int dy[] = {0, +1, 0, -1};

int component = 0;
int recursion_depth = 0;

void dfs(int x, int y, int current_label, int** labels, ImagePtr data)
{
    int row_count = 1024;
    int column_count = 1024;
    recursion_depth++;

    if (x < 0 || x == row_count)
        return;
    if (y < 0 || y == column_count)
        return;

    int a = component;
    int b = labels[x][y];

    // If the label is already set or if this area is land (not ocean)
    if (labels[x][y] != -1 || data->getPixel(x, y).b != 50)
        return;

    if (recursion_depth > 1000)
    {
        //std::cout << "Recursion depth maxed: " << recursion_depth << std::endl;
        labels[x][y] = current_label;
        return;
    }

    labels[x][y] = current_label;

    for (int direction = 0; direction < 4; ++direction)
        dfs(x +dx[direction], y + dy[direction], current_label, labels, data);

}

void WorldGenerator::runRivers()
{
    // Get the finished heightmap
    auto heightmap = app.getDataStorage()->getImage("heightmap");
    
    // Pick a random spot that is higher than the mountain limit height
    sf::Color pixel;

    int sea_size = sea_pixels.size();

    sf::Color sea_color(0, 0, 50, 255);
    int current_ocean = 0;

    int** labels = app.getToolbox()->giveIntArray2D(heightmap->getSize().x, heightmap->getSize().y);
    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
            labels[i][j] = -1;
    }


    /*
    int recursion_capped = 0;

    // Connected components labeling algorithm
    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            // If the label for this pixel is unset
            if (labels[i][j] == -1)
            {
                //std::cout << i << ", " << j << ", " << component << std::endl;
                // If the heightmap at this coordinate is sea
                if (heightmap->getPixel(i, j).b == 50)
                {
                    if (recursion_depth > 1000)
                    {
                        recursion_depth = 0;
                        recursion_capped++;
                        dfs(i, j, component, labels, heightmap);
                    }
                    else
                    {
                        dfs(i, j, ++component, labels, heightmap);
                    }
                }
            }
        }
    }
    */
    //std::cout << "There are " << component << " ocean zones" << std::endl;
    //std::cout << "Recursion capped: " << recursion_capped << std::endl;

    // Mark each sea region with matching color
    /*
    sf::Color c;
    c.r = 0;
    c.b = 0;
    c.a = 255;
    float step_size = 255 / (float)component;

    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            if (labels[i][j] != -1)
            {
                c.g = labels[i][j] * step_size;
                heightmap->setPixel(i, j, c);
            }
        }
    }
    */

    std::stack<sf::Vector2i> stack;
    
    std::vector<sf::Vector2i> regions;
    sf::Color c;

    int** regionmap = app.getToolbox()->giveIntArray2D(heightmap->getSize().x, heightmap->getSize().y);
    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            regionmap[i][j] = -1;
        }
    }

    for (auto p : sea_pixels)
        regionmap[p.x][p.y] = 0;

    int current_region = 0;
    int current_size = 0;
    bool finished = false;

    std::map<int, int> regionsizes;

    int current_x = -1;
    int current_y = 0;

    while (finished == false)
    {
        std::cout << "Region: " << current_region << ", size: " << current_size << std::endl;

        regionsizes[current_region] = current_size;
        current_size = 0;
        current_region++;

        while (current_x < heightmap->getSize().x || current_y < heightmap->getSize().y)
        {
            current_x++;
            if (current_x == heightmap->getSize().x && current_y < heightmap->getSize().y)
            {
                current_x = 0;
                current_y++;
            }

            if (current_x == 0 && current_y == heightmap->getSize().y)
            {
                finished = true;
                while (stack.empty() == false)
                    stack.pop();
                break;
            }

            if (regionmap[current_x][current_y] == 0)
            {
                stack.push(sf::Vector2i(current_x, current_y));
                regionmap[current_x][current_y] = current_region;
                current_size++;
                break;
            }

        }

        while (stack.empty() == false)
        {
            sf::Vector2i coord = stack.top();
            stack.pop();
            c = heightmap->getPixel(coord.x, coord.y);

            if (coord.x > 0)
            {
                auto temp = sf::Vector2i(coord.x-1, coord.y);
                c = heightmap->getPixel(coord.x-1, coord.y);
                if (heightmap->getPixel(coord.x-1, coord.y) == sea_color && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x-1, coord.y);
                    stack.push(p);
                    regions.push_back(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.y > 0)
            {
                auto temp = sf::Vector2i(coord.x, coord.y-1);
                c = heightmap->getPixel(coord.x, coord.y-1);
                if (heightmap->getPixel(coord.x, coord.y-1) == sea_color && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x, coord.y-1);
                    stack.push(p);
                    regions.push_back(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.x < heightmap->getSize().x-1)
            {
                auto temp = sf::Vector2i(coord.x+1, coord.y);
                c = heightmap->getPixel(coord.x+1, coord.y);
                if (heightmap->getPixel(coord.x+1, coord.y) == sea_color && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x+1, coord.y);
                    stack.push(p);
                    regions.push_back(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.y < heightmap->getSize().y-1)
            {
                auto temp = sf::Vector2i(coord.x, coord.y+1);
                c = heightmap->getPixel(coord.x, coord.y+1);
                if (heightmap->getPixel(coord.x, coord.y+1) == sea_color && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x, coord.y+1);
                    stack.push(p);
                    regions.push_back(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
        }
    }

    sf::Color waterreg;
    waterreg.r = 0;
    waterreg.b = 50;
    waterreg.g = 0;
    waterreg.a = 255;
    
    std::cout << "Total regions: " << current_region << std::endl;

    // Color the ocean pixels tagged above
    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            if (regionmap[i][j] != -1)
            {
                if  (regionsizes[regionmap[i][j]] < 10)
                {
                    heightmap->setPixel(i, j, sf::Color::Red);
                }
                else if  (regionsizes[regionmap[i][j]] > 10 && regionsizes[regionmap[i][j]] < 300)
                {
                    heightmap->setPixel(i, j, sf::Color::Yellow);
                }
                else if  (regionsizes[regionmap[i][j]] > 300 && regionsizes[regionmap[i][j]] < 50000)
                {
                    heightmap->setPixel(i, j, sf::Color::Green);
                }
                else
                {
                    waterreg.b = app.getToolbox()->linearInterpolate(0, current_region, regionmap[i][j])*255;
                    heightmap->setPixel(i, j, sf::Color::Blue);
                }
            }
        }
    }

    std::shared_ptr<sf::Texture> heightmap_text = app.getDataStorage()->getTexture("heightmap");

    heightmap_text->update(*heightmap);
    std::shared_ptr<sf::Sprite> heightmap_sprite = app.getDataStorage()->getSprite("heightmap");
    heightmap_sprite->setTexture(*heightmap_text);
}

ImagePtr WorldGenerator::voronoi()
{
    ImagePtr out = ImagePtr(new sf::Image());
    out->create(1024, 1024);
    int number_of_cells = 200;
    std::vector<int> nx;
    std::vector<int> ny;
    std::vector<int> nr;
    std::vector<int> ng;
    std::vector<int> nb;
    
    for (int i = 0; i < number_of_cells; i++)
    {
        nx.push_back(app.getToolbox()->giveRandomInt(0, 1024));
        ny.push_back(app.getToolbox()->giveRandomInt(0, 1024));
        nr.push_back(app.getToolbox()->giveRandomInt(0, 255));
        ng.push_back(app.getToolbox()->giveRandomInt(0, 255));
        nb.push_back(app.getToolbox()->giveRandomInt(0, 255));
    }

    float dmin = 0.0f;
    float d = 0.0f;
    for (int y = 0; y < 1024; y++)
    {
        for (int x = 0; x < 1024; x++)
        {
            dmin = hypot(1023, 1023);
            int j = -1;
            for (int i = 0; i < number_of_cells; i++)
            {
                d = hypot(nx.at(i)-x, ny.at(i)-y);
                if (d < dmin)
                {
                    dmin = d;
                    j = i;
                }
            }
            out->setPixel(x, y, sf::Color(nr.at(j), ng.at(j), nb.at(j), 255));
        }
    }
    return out;
}

