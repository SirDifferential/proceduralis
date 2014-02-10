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
    SpritePtr regionmap = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("regionmap", regionmap);
    ImagePtr regionmap_image = ImagePtr(new sf::Image());
    regionmap_image->create(1024, 1024);
    app.getDataStorage()->storeImage("regionmap", regionmap_image);
    TexturePtr regionmap_text = TexturePtr(new sf::Texture());
    regionmap_text->create(1024, 1024);
    app.getDataStorage()->storeTexture("regionmap", regionmap_text);
    SpritePtr winddir = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("winddirections", winddir);
    TexturePtr winddir_text = TexturePtr(new sf::Texture());
    winddir_text->create(1024, 1024);
    app.getDataStorage()->storeTexture("winddirections", winddir_text);
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
* There are two tolerances: down and up. The down tolerance means the allowed differences down from the
* code color, while up refers tolerance of colors that are higher than the code color
*/
void WorldGenerator::solveRegions(int** regionmap, sf::Color code, sf::Color tolerance, std::shared_ptr<std::map<int, int>> rsizes, int regionCodeStartRange)
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
            // Only check pixels that aren't already checked
            if (regionmap[i][j] == -1 && app.getToolbox()->colorValidRange(code, heightmap->getPixel(i, j), tolerance) == true)
                regionmap[i][j] = regionCodeStartRange;
        }
    }

    int current_region = regionCodeStartRange;
    int current_size = 0;
    bool finished = false;

    int current_x = -1;
    int current_y = 0;

    sf::Color c;
    sf::Vector2i p;
    sf::Vector2i temp;

    while (finished == false)
    {
        //std::cout << "Region: " << current_region << ", size: " << current_size << std::endl;

        (*rsizes)[current_region] = current_size;
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
                temp = sf::Vector2i(coord.x-1, coord.y);
                c = heightmap->getPixel(coord.x-1, coord.y);
                if (regionmap[temp.x][temp.y] == regionCodeStartRange && app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x-1, coord.y), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    p = sf::Vector2i(coord.x-1, coord.y);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.y > 0)
            {
                temp = sf::Vector2i(coord.x, coord.y-1);
                c = heightmap->getPixel(coord.x, coord.y-1);
                if (regionmap[temp.x][temp.y] == regionCodeStartRange && app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x, coord.y-1), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    p = sf::Vector2i(coord.x, coord.y-1);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.x < heightmap->getSize().x-1)
            {
                temp = sf::Vector2i(coord.x+1, coord.y);
                c = heightmap->getPixel(coord.x+1, coord.y);
                if (regionmap[temp.x][temp.y] == regionCodeStartRange && app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x+1, coord.y), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    p = sf::Vector2i(coord.x+1, coord.y);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
            if (coord.y < heightmap->getSize().y-1)
            {
                temp = sf::Vector2i(coord.x, coord.y+1);
                c = heightmap->getPixel(coord.x, coord.y+1);
                if (regionmap[temp.x][temp.y] == regionCodeStartRange && app.getToolbox()->colorValidRange(code, heightmap->getPixel(coord.x, coord.y+1), tolerance) == true && regionmap[temp.x][temp.y] != current_region)
                {
                    p = sf::Vector2i(coord.x, coord.y+1);
                    stack.push(p);
                    regionmap[p.x][p.y] = current_region;
                    current_size++;
                }
            }
        }
    }

    if (code == sf::Color(0,0,50,255))
        timer1 = std::chrono::system_clock::now();
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
    auto regionmap_image = app.getDataStorage()->getImage("regionmap");

    int ocean_index_start = app.getWorld()->getOceanStartIndex();
    int mountain_index_start = app.getWorld()->getMountainStartIndex();
    int hill_index_start = app.getWorld()->getHillStartIndex();
    int flat_index_start = app.getWorld()->getFlatStartIndex();

    int ocean_regions;
    int mountain_regions;
    int hill_regions;
    int flat_regions;
    
    std::shared_ptr<std::map<int,int>> ocean_region_sizes = app.getWorld()->getOceanRegions();
    std::shared_ptr<std::map<int,int>> mountain_region_sizes = app.getWorld()->getMountainRegions();
    std::shared_ptr<std::map<int,int>> hill_region_sizes = app.getWorld()->getHillRegions();
    std::shared_ptr<std::map<int,int>> flat_region_sizes = app.getWorld()->getFlatRegions();

    int** regionmap = app.getWorld()->getRegionMap();
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
    sf::Color tolerance(0,0,0,0);
    
    std::chrono::time_point<std::chrono::system_clock> start, end;
    
    start = std::chrono::system_clock::now();

    std::cout << "Solving oceans..." << std::endl;

    solveRegions(regionmap, sea_color, tolerance, ocean_region_sizes, ocean_index_start);
    timer2 = std::chrono::system_clock::now();
    ocean_regions = ocean_region_sizes->size();

    end = std::chrono::system_clock::now();
    int elapsed_seconds_1 = std::chrono::duration_cast<std::chrono::seconds> (end-start).count();
    
    std::cout << "Solving mountains..." << std::endl;

    sf::Color mountain_color(255, 255, 255, 255);
    tolerance = sf::Color(152, 152, 152, 0);
    solveRegions(regionmap, mountain_color, tolerance, mountain_region_sizes, mountain_index_start);
    timer2 = std::chrono::system_clock::now();
    mountain_regions = mountain_region_sizes->size();

    end = std::chrono::system_clock::now();
    int elapsed_seconds_2 = std::chrono::duration_cast<std::chrono::seconds> (end-start).count();

    std::cout << "Solving hills..." << std::endl;

    sf::Color hill_color(102, 102, 102, 255);
    tolerance = sf::Color(15, 15, 15, 0);
    solveRegions(regionmap, hill_color, tolerance, hill_region_sizes, hill_index_start);
    hill_regions = hill_region_sizes->size();

    end = std::chrono::system_clock::now();
    int elapsed_seconds_3 = std::chrono::duration_cast<std::chrono::seconds> (end-start).count();

    std::cout << "Solving flats..." << std::endl;
    sf::Color flat_color(86, 86, 86, 255);
    tolerance = sf::Color(86, 86, 86, 0);
    solveRegions(regionmap, flat_color, tolerance, flat_region_sizes, flat_index_start);
    flat_regions = flat_region_sizes->size();

    end = std::chrono::system_clock::now();
    int elapsed_seconds_4 = std::chrono::duration_cast<std::chrono::seconds> (end-start).count();

    std::cout << "Colorizing..." << std::endl;
    // Color the ocean pixels tagged above
    
    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            if (regionmap[i][j] > ocean_index_start && regionmap[i][j] <= ocean_index_start + ocean_regions)
            {
                if  ((*ocean_region_sizes)[regionmap[i][j]] < 10)
                {
                    regionmap_image->setPixel(i, j, sf::Color(135, 135, 245, 255));
                }
                else if ((*ocean_region_sizes)[regionmap[i][j]] > 10 && (*ocean_region_sizes)[regionmap[i][j]] < 300)
                {
                    regionmap_image->setPixel(i, j, sf::Color(12, 12, 162, 255));
                }
                else if  ((*ocean_region_sizes)[regionmap[i][j]] > 300 && (*ocean_region_sizes)[regionmap[i][j]] < 50000)
                {
                    regionmap_image->setPixel(i, j, sf::Color(8, 8, 92, 255));
                }
                else
                {
                    regionmap_image->setPixel(i, j, sf::Color(0,0,30,255));
                }
            }
            else if (regionmap[i][j] > mountain_index_start && regionmap[i][j] <= mountain_index_start + mountain_regions)
            {
                regionmap_image->setPixel(i, j, sf::Color(233, 222, 197, 255));
            }
            else if (regionmap[i][j] > hill_index_start && regionmap[i][j] <= hill_index_start + hill_regions)
            {
                regionmap_image->setPixel(i, j, sf::Color(220, 169, 60, 255));
            }
            else if (regionmap[i][j] > flat_index_start && regionmap[i][j] <= flat_index_start + flat_regions)
            {
                regionmap_image->setPixel(i, j, sf::Color(14, 36, 20, 255));
            }
        }
    }
    

    end = std::chrono::system_clock::now();
    int elapsed_seconds_5 = std::chrono::duration_cast<std::chrono::seconds> (end-start).count();
    int waited = std::chrono::duration_cast<std::chrono::seconds> (timer2-timer1).count();

    std::shared_ptr<sf::Texture> regionmap_text = app.getDataStorage()->getTexture("regionmap");

    regionmap_text->update(*regionmap_image);
    std::shared_ptr<sf::Sprite>  regionmap_sprite = app.getDataStorage()->getSprite("regionmap");
    regionmap_sprite->setTexture(*regionmap_text);

    end = std::chrono::system_clock::now();
    int sprites_updated = std::chrono::duration_cast<std::chrono::seconds> (end-start).count();

    std::cout << "Operation\tTime" << std::endl;
    std::cout << "oceans   \t" << elapsed_seconds_1 << std::endl;
    std::cout << "mountains\t" << elapsed_seconds_2 << std::endl;
    std::cout << "hills    \t" << elapsed_seconds_3 << std::endl;
    std::cout << "flats    \t" << elapsed_seconds_4 << std::endl;
    std::cout << "coloring \t" << elapsed_seconds_5 << std::endl;
    std::cout << "total    \t" << elapsed_seconds_1 + elapsed_seconds_2 + elapsed_seconds_3 + elapsed_seconds_4 + elapsed_seconds_5 << std::endl;
    std::cout << "waiting  \t" << waited << std::endl;
    std::cout << "sprites  \t" << sprites_updated << std::endl;

    app.getWorld()->setRegion(regionmap);
    app.getWorld()->setOceanRegions(ocean_region_sizes);
    app.getWorld()->setMountainRegions(mountain_region_sizes);
    app.getWorld()->setHillRegions(hill_region_sizes);
    app.getWorld()->setFlatRegions(flat_region_sizes);
}

void WorldGenerator::rainclouds()
{
    auto w = app.getDataStorage()->getImage("winddirections");

    sf::Vector2i origin(512, 512);
    sf::Vector2i current = origin;
    sf::Vector2i old = current;
    sf::Color clouds(255, 255, 0, 255);

    int stepsTaken = 0;
    int stepLimit = 50000;

    // Angles are in radians multiplied by 100
    // The 100 is there so that integers can represent small angles

    float sectorrange = 3.14159 / 8.0;

    std::stack<sf::Vector2i> windpoints;
    
    
    for (int i = 0; i < 100; i++)
    {
        windpoints.push(sf::Vector2i(app.getToolbox()->giveRandomInt(0, 1023), app.getToolbox()->giveRandomInt(0, 1023)));
    }

    /*
    windpoints.push(sf::Vector2i(512, 1023));
    windpoints.push(sf::Vector2i(10, 1023));
    windpoints.push(sf::Vector2i(1000, 1023));
    windpoints.push(sf::Vector2i(10, 0));
    windpoints.push(sf::Vector2i(512, 0));
    
    windpoints.push(sf::Vector2i(1023, 0));
    
    windpoints.push(sf::Vector2i(512, 100));
    
    windpoints.push(sf::Vector2i(15, 1000));
    
    
    windpoints.push(sf::Vector2i(128, 20));
    
    windpoints.push(sf::Vector2i(768, 128));
    
    windpoints.push(sf::Vector2i(768, 768));
    
    windpoints.push(sf::Vector2i(512, 402));
    */
    while (windpoints.empty() == false)
    {
        origin = windpoints.top();
        windpoints.pop();
        current = origin;
        old = origin;
        stepsTaken = 0;

        while (stepsTaken < stepLimit)
        {
            stepsTaken++;

            clouds.b = w->getPixel(old.x, old.y).b;

            w->setPixel(old.x, old.y, clouds);
            old = current;

            // Get the angle at this coord
            float rads = w->getPixel(current.x, current.y).b / 100.0;
            if (rads < sectorrange)
            {
                // Go up
                current.y++;
            }
            else if (rads >= sectorrange && rads < sectorrange * 3)
            {
                // Go up right
                current.x++;
                current.y++;
            }
            else if (rads >= sectorrange * 3 && rads < 5 * sectorrange)
            {
                // Go right
                current.x++;
            }
            else if (rads >= 5 * sectorrange && rads < 7 * sectorrange)
            {
                // Go down right
                current.x++;
                current.y--;
            }
            else if (rads >= 7 * sectorrange && rads < 9 * sectorrange)
            {
                // Go down
                current.y++;
            }
            else if (rads >= 9 * sectorrange && rads < 11 * sectorrange)
            {
                // go down left
                current.x--;
                current.y--;
            }
            else if (rads >= 11 * sectorrange && rads < 13 * sectorrange)
            {
                // go left
                current.x--;
            }
            else if (rads >= 13 * sectorrange && rads < 15 * sectorrange)
            {
                // go up left
                current.x--;
                current.y++;
            }
            else
            {
                std::cout << "Weird angle " << rads << std::endl;
                current.x++;
            }
        
            if (current.y < 0)
            {
                current.y = 2;
                current.x++;
            }
            else if (current.y > w->getSize().y -1)
            {
                current.y = w->getSize().y -2;
                current.x++;
            }

            if (current.x < 0)
                current.x = w->getSize().x -1;
            else if (current.x > w->getSize().x-1)
                current.x = 0;
        }
    }

    std::shared_ptr<sf::Texture> windmap_text = app.getDataStorage()->getTexture("winddirections");

    windmap_text->update(*w);
    std::shared_ptr<sf::Sprite>  windmap_sprite = app.getDataStorage()->getSprite("winddirections");
    windmap_sprite->setTexture(*windmap_text);

}

void WorldGenerator::runRivers()
{
    auto heightmap = app.getDataStorage()->getImage("heightmap");
    auto regionmap_image = app.getDataStorage()->getImage("regionmap");

    auto regionmap = app.getWorld()->getRegionMap();
    //auto regioninfo = app.getWorld()->getRegionInfo();
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

