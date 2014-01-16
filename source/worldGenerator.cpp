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
}

void WorldGenerator::generate()
{
}

/**
* Assumes that a map of voronoi cells has been generated
* Takes this map and produces sets of pixels based on cell color
* Each voronoi cell is nicely categorized afterwards
*/
void WorldGenerator::formSuperRegions()
{
    auto cells = app.getDataStorage()->getImage("voronoi_cells");

    std::map<float, std::vector<sf::Vector2i>> regions;
    std::vector<float> superregions;
    sf::Color cell_color;
    float region_code = 0.0f;

    int superregion_cores = 0;
    int superregion_size_x = 6;
    int start_super = 3;
    int region_counter = 0;

    // Iterate through our generated voronoi noise
    // Add each pixel to the proper set based on the generated voronoi cell color
    // This is slow and stupid, but so is yours truly
    for (int i = 0; i < cells->getSize().x; i++)
    {
        for (int j = 0; j < cells->getSize().y; j++)
        {
            cell_color = cells->getPixel(i, j);
            region_code = cell_color.r;
            if (regions.count(region_code) > 0)
            {
                // Region is not added yet
                region_counter++;
                if (start_super >= region_counter && region_counter < superregion_size_x)
                {
                    superregions.push_back(region_code);
                }
            }
            regions[region_code].push_back(sf::Vector2i(i, j));
        }
    }

    std::cout << "+WorldGenerator: Regions found: " << regions.size() << std::endl;
    std::cout << "+WorldGenerator: Superregions created: " << superregions.size() << std::endl;

    // Add color to superregions

    sf::Color temp2;
    for (float r : superregions)
    {
        for (auto coord : regions[r])
        {
            temp2 = cells->getPixel(coord.x, coord.y);
            temp2.r = 250;
            temp2.g = 250;
            cells->setPixel(coord.x, coord.y, temp2);
        }
    }

    app.getSpriteUtils()->setPixels(app.getDataStorage()->getSprite("heightmap"), "heightmap", cells);

}

float** WorldGenerator::diamondSquare()
{
    return nullptr;
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

