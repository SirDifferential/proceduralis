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

            if (height < 0.19)
            {
                // Ocean
                output_col.r = 0;
                output_col.b = 50;
                output_col.g = 0;
            }
            else
            {
                // Land

                output_col.r = app.getToolbox()->linearInterpolate(0.19, 1.0, height) * 255;
                output_col.g = app.getToolbox()->linearInterpolate(0.19, 1.0, height) * 255;
                output_col.b = app.getToolbox()->linearInterpolate(0.19, 1.0, height) * 255;
            }

            heightmap->setPixel(i, j, output_col);
        }
    }

    app.getSpriteUtils()->setPixels(app.getDataStorage()->getSprite("heightmap"), "heightmap", heightmap);
}

void WorldGenerator::runRivers()
{
    // Get the finished heightmap
    auto heightmap = app.getDataStorage()->getImage("heightmap");
    
    // Pick a random spot that is higher than the mountain limit height
    sf::Color pixel;

    sf::Vector2i riverOrigin(-1, -1);
    unsigned int max_river_steps = 10000;

    float mountain_min_height = 0.50f * 255;

    float height_topleft = 0;
    float height_top = 0;
    float height_topright = 0;
    float height_left = 0;
    float height_right = 0;
    float height_bottomleft = 0;
    float height_bottom = 0;
    float height_bottomright = 0;

    bool checkleft = false;
    bool checkright = false;
    bool checktop = false;
    bool checkbottom = false;
    bool riverEnded = false;

    sf::Vector2i current_river_coords;
    sf::Vector2i next_river_coords;
    unsigned int stepsTaken = 0;

    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            pixel = heightmap->getPixel(i,j);
            if (pixel.r >= mountain_min_height)
            {
                riverOrigin.x = i;
                riverOrigin.y = j;
                heightmap->setPixel(i, j, sf::Color::Red);
                
                // From this location, start going downhill
                //bool riverEnded = false;

                current_river_coords = riverOrigin;
                float current_min = 0;
                riverEnded = false;

                while (!riverEnded || stepsTaken < max_river_steps)
                {
                    stepsTaken++;
                    current_min = heightmap->getPixel(current_river_coords.x, current_river_coords.y).r;
                    heightmap->setPixel(current_river_coords.x, current_river_coords.y, sf::Color::Green);

                    checkleft = true;
                    checkright = true;
                    checktop = true;
                    checkbottom = true;

                    height_topleft = 1000;
                    height_top = 1000;
                    height_topright = 1000;
                    height_left = 1000;
                    height_right = 1000;
                    height_bottomleft = 1000;
                    height_bottom = 1000;
                    height_bottomright = 1000;

                    // See which direction has sharpest declining height
                    if (current_river_coords.x <= 0)
                    {
                        checkleft = false;
                    }
                    else if (current_river_coords.x >= heightmap->getSize().x -1)
                    {
                        checkright = false;
                    }
                    if (current_river_coords.y <= 0)
                    {
                        checktop = false;
                    }
                    else if (current_river_coords.y >= heightmap->getSize().y -1)
                    {
                        checkbottom = false;
                    }

                    if (checkleft && checktop)
                    {
                        height_topleft = heightmap->getPixel(current_river_coords.x-1, current_river_coords.y-1).r;
                        if (height_topleft < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x-1, current_river_coords.y-1);
                            current_min = height_topleft;
                        }
                    }
                    if (checktop)
                    {
                        height_top = heightmap->getPixel(current_river_coords.x, current_river_coords.y-1).r;
                        if (height_top < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x, current_river_coords.y-1);
                            current_min = height_top;
                        }
                    }
                    if (checktop && checkright)
                    {
                        height_topright = heightmap->getPixel(current_river_coords.x+1, current_river_coords.y-1).r;
                        if (height_topright < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x+1, current_river_coords.y-1);
                            current_min = height_topright;
                        }
                    }
                    if (checkleft)
                    {
                        height_left = heightmap->getPixel(current_river_coords.x-1, current_river_coords.y).r;
                        if (height_left < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x-1, current_river_coords.y);
                            current_min = height_left;
                        }
                    }
                    if (checkright)
                    {
                        height_right = heightmap->getPixel(current_river_coords.x+1, current_river_coords.y).r;
                        if (height_right < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x+1, current_river_coords.y);
                            current_min = height_right;
                        }
                    }
                    if (checkleft && checkbottom)
                    {
                        height_bottomleft = heightmap->getPixel(current_river_coords.x-1, current_river_coords.y+1).r;
                        if (height_bottomleft < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x-1, current_river_coords.y+1);
                            current_min = height_bottomleft;
                        }
                    }
                    if (checkbottom)
                    {
                        height_bottom = heightmap->getPixel(current_river_coords.x, current_river_coords.y+1).r;
                        if (height_bottom < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x, current_river_coords.y+1);
                            current_min = height_bottom;
                        }
                    }
                    if (checkright && checkbottom)
                    {
                        height_bottomright = heightmap->getPixel(current_river_coords.x+1, current_river_coords.y+1).r;
                        if (height_bottomright < current_min)
                        {
                            next_river_coords = sf::Vector2i(current_river_coords.x+1, current_river_coords.y+1);
                            current_min = height_bottomright;
                        }
                    }

                    if (current_min == heightmap->getPixel(current_river_coords.x, current_river_coords.y).r)
                    if (checkleft)
                        next_river_coords = sf::Vector2i(current_river_coords.x-1, current_river_coords.y);
                    else if (checkbottom && checkright)
                        next_river_coords = sf::Vector2i(current_river_coords.x+1, current_river_coords.y+1);
                    else if (checktop && checkright)
                        next_river_coords = sf::Vector2i(current_river_coords.x+1, current_river_coords.y-1);
                    
                    current_river_coords = next_river_coords;

                    

                    if (current_min < 0.20*255)
                        riverEnded = true;

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

