#include "worldGenerator.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "datastorage.hpp"
#include "spriteutils.hpp"

WorldGenerator::WorldGenerator()
{
    // codes used for indexing the world's regionmap in specific types of areas
    ocean_index_start = 50;
    mountain_index_start = 5000;
    hill_index_start = 10000;
    flat_index_start = 20000;
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

/**
* Finds areas of the world heightmap that match a specific height
* and performs a neighbor search to form continous regions with the matching code
* The data is saved in the worldgenerator's variable int** regionmap, where
* each region will be a number at least regionCodeStartRange+1
* The rsizes is a reference to one of the region_size maps store in WorldGenerator, and
* is used for getting an idea of how large each region of specific type is.
* For example, the ocean region number 55 might be 10 pixels in size,
* while ocean region number 56 might be 196919 pixels in size
* regionCodeStartRange is used in regionMap for splitting specific types of regions
* Oceans may be, for example, indexed from 50 to up, and mountains from 5000 upwards
* The code is the search term which the solver will attempt to match, and tolerance describes how much
* the search will allow there to be a difference to the desired code
* Using tolerance of sf::Color(0,0,0,0) will mean only absolute matches will be considered
*/
void WorldGenerator::solveRegions(sf::Color code, sf::Color tolerance, std::map<int, int>& rsizes, int regionCodeStartRange)
{
    // Get the finished heightmap
    auto heightmap = app.getDataStorage()->getImage("heightmap");

    std::stack<sf::Vector2i> stack;

    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            // If this current pixel is within the specified range to trigger this region code
            // If yes, use start range code to mark this area as a not-yet-computed important pixel
            if (app.getToolbox()->colorValidRange(code, heightmap->getPixel(i, j), tolerance) == true)
                regionmap[i][j] = regionCodeStartRange;
        }
    }

    int current_region = regionCodeStartRange;
    int current_size = 0;
    bool finished = false;

    int current_x = -1;
    int current_y = 0;

    std::cout << "Solving region with code " << code.r << ", " << code.g << ", " << code.b << ", " << code.a << " and tolerance " << tolerance.r << ", " << tolerance.g << ", " << tolerance.b << ", " << tolerance.a << std::endl; 

    sf::Color c;

    while (finished == false)
    {
        std::cout << "Region: " << current_region << ", size: " << current_size << std::endl;

        rsizes[current_region] = current_size;
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

            // If the current pixel has been marked for computation, but is not yet solved to be a member of some region
            if (regionmap[current_x][current_y] == regionCodeStartRange)
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
                if (app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x-1, coord.y), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x-1, coord.y);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.y > 0)
            {
                auto temp = sf::Vector2i(coord.x, coord.y-1);
                c = heightmap->getPixel(coord.x, coord.y-1);
                if (app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x, coord.y-1), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x, coord.y-1);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.x < heightmap->getSize().x-1)
            {
                auto temp = sf::Vector2i(coord.x+1, coord.y);
                c = heightmap->getPixel(coord.x+1, coord.y);
                if (app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x+1, coord.y), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x+1, coord.y);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.y < heightmap->getSize().y-1)
            {
                auto temp = sf::Vector2i(coord.x, coord.y+1);
                c = heightmap->getPixel(coord.x, coord.y+1);
                if (app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x, coord.y+1), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    auto p = sf::Vector2i(coord.x, coord.y+1);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
        }
    }

    std::cout << "Total regions: " << current_region-regionCodeStartRange << ": [" << regionCodeStartRange << ", " << current_region << "]" << std::endl;
}

/**
* This function assumes that a heightmap has been generated
* The function will perform various region searches attempting to form continuous regions that share some property
* This search is usually called Connected Component Searching or something along those lines
* After this function, the int regionmap** will be populated with region codes
* In addition, the various std::map<int,int> will describe how large each region is
*/
void WorldGenerator::formRegions()
{
    // Get the finished heightmap
    auto heightmap = app.getDataStorage()->getImage("heightmap");

    if (regionmap != NULL)
        app.getToolbox()->deleteIntArray2D(regionmap, heightmap->getSize().x);
    regionmap = app.getToolbox()->giveIntArray2D(heightmap->getSize().x, heightmap->getSize().y);
    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            regionmap[i][j] = -1;
        }
    }

    sf::Color sea_color(0, 0, 50, 255);
    sf::Color tolerance(0, 0, 0, 0);
    int ocean_tag_start = 50;
    int land_tag_start = 500;
    solveRegions(sea_color, tolerance, ocean_region_sizes, ocean_tag_start);
    ocean_regions = ocean_region_sizes.size();

    // Color the ocean pixels tagged above
    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            if (regionmap[i][j] > ocean_tag_start && regionmap[i][j] <= ocean_tag_start + ocean_regions)
            {
                if  (ocean_region_sizes[regionmap[i][j]] < 10)
                {
                    heightmap->setPixel(i, j, sf::Color::Red);
                }
                else if (ocean_region_sizes[regionmap[i][j]] > 10 && ocean_region_sizes[regionmap[i][j]] < 300)
                {
                    heightmap->setPixel(i, j, sf::Color::Yellow);
                }
                else if  (ocean_region_sizes[regionmap[i][j]] > 300 && ocean_region_sizes[regionmap[i][j]] < 50000)
                {
                    heightmap->setPixel(i, j, sf::Color::Green);
                }
                else
                {
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

