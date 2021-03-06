#include "worldGenerator.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "datastorage.hpp"
#include "spriteutils.hpp"
#include "region.hpp"
#include "cl_precipitation.hpp"
#include "gui.hpp"

WorldGenerator::WorldGenerator()
{
}

void WorldGenerator::init()
{
    sf::Vector2i standard_size(1024,1024);
    app.getDataStorage()->generateSpriteTriplet("voronoi_cells", standard_size);
    app.getDataStorage()->generateSpriteTriplet("perlinnoise", standard_size);
    app.getDataStorage()->generateSpriteTriplet("voronoiblurred", standard_size);
    app.getDataStorage()->generateSpriteTriplet("perlinblurred", standard_size);
    app.getDataStorage()->generateSpriteTriplet("regionmap", standard_size);
    app.getDataStorage()->generateSpriteTriplet("winddirections", standard_size);
    app.getDataStorage()->generateSpriteTriplet("windblurred", standard_size);
    app.getDataStorage()->generateSpriteTriplet("precipitation", standard_size);
    app.getDataStorage()->generateSpriteTriplet("precipitation_blurred", standard_size);
    app.getDataStorage()->generateSpriteTriplet("heightmap", standard_size);
    app.getDataStorage()->generateSpriteTriplet("temperature", standard_size);
    app.getDataStorage()->generateSpriteTriplet("temperature_blurred", standard_size);
    app.getDataStorage()->generateSpriteTriplet("biomes", standard_size);
    app.getDataStorage()->generateSpriteTriplet("satellite", standard_size);
}

/**
* The big momma.
* Generates the entire world
*/
void WorldGenerator::generate()
{
    app.getGUI()->drawLoadingText("Initting OpenCL programs...");
    app.getProgram("voronoi")->init();
    app.getProgram("perlin")->init();
    app.getProgram("winddir")->init();
    app.getProgram("blur")->init();

    app.getGUI()->drawLoadingText("Forming tectonic plates...");
    app.runProgram("voronoi");
    app.getGUI()->drawLoadingText("Forming details...");
    app.runProgram("perlin");
    app.getGUI()->drawLoadingText("Forming global wind circulation...");
    app.runProgram("winddir");
    app.getGUI()->drawLoadingText("Forming continents...");
    formSuperRegions();

    app.getGUI()->drawLoadingText("Calculating temperature...");
    app.getProgram("temperature")->init();
    app.runProgram("temperature");
    app.getWorld()->setTemperatureImage(app.getDataStorage()->getImage("temperature"));
    app.getGUI()->drawLoadingText("Forming regions...");
    formRegions();

    app.getGUI()->drawLoadingText("Solving rainfall...");
    app.getProgram("precipitation")->init();
    app.runProgram("precipitation");
    app.getWorld()->setPrecipitationImage(app.getDataStorage()->getImage("precipitation_blurred"));
    app.getGUI()->drawLoadingText("Forming rivers and erosion...");
    runRivers();

    app.getGUI()->drawLoadingText("Forming biomes...");
    app.getProgram("biomes")->init();
    app.runProgram("biomes");

    app.getGUI()->drawLoadingText("Satellite photographing planet...");
    app.getProgram("satellite")->init();
    app.runProgram("satellite");


    app.getDataStorage()->writeImageToDisk("heightmap");
    app.getDataStorage()->writeImageToDisk("biomes");
    app.getDataStorage()->writeImageToDisk("precipitation_blurred");
    app.getDataStorage()->writeImageToDisk("windblurred");
    app.getDataStorage()->writeImageToDisk("regionmap");
    app.getDataStorage()->writeImageToDisk("temperature");
    app.getDataStorage()->writeImageToDisk("perlinblurred");
    app.getDataStorage()->writeImageToDisk("voronoi_cells");
}

void WorldGenerator::formSuperRegions()
{
    std::cout << "+WorldGenerator: Combining images" << std::endl;
    auto cells = app.getDataStorage()->getImage("voronoi_cells");
    auto perlinblurred = app.getDataStorage()->getImage("perlinblurred");
    auto perlin = app.getDataStorage()->getImage("perlinnoise");
    auto heightmap = app.getDataStorage()->getImage("heightmap");
    auto voronoiblurred = app.getDataStorage()->getImage("voronoiblurred");

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

    auto regions = app.getWorld()->getRegions();
    int s = regions->size();
    
    
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

                if (regions->find(regionmap[i][j]) == regions->end())
                    (*regions)[regionmap[i][j]] = RegionPtr(new Region(RegionType::Region_Ocean, regionmap[i][j]));
                (*regions)[regionmap[i][j]]->addSite(sf::Vector2i(i, j));
            }
            else if (regionmap[i][j] > mountain_index_start && regionmap[i][j] <= mountain_index_start + mountain_regions)
            {
                regionmap_image->setPixel(i, j, sf::Color(233, 222, 197, 255));
                if (regions->find(regionmap[i][j]) == regions->end())
                    (*regions)[regionmap[i][j]] = RegionPtr(new Region(RegionType::Region_Mountains, regionmap[i][j]));
                (*regions)[regionmap[i][j]]->addSite(sf::Vector2i(i, j));
            }
            else if (regionmap[i][j] > hill_index_start && regionmap[i][j] <= hill_index_start + hill_regions)
            {
                regionmap_image->setPixel(i, j, sf::Color(220, 169, 60, 255));
                if (regions->find(regionmap[i][j]) == regions->end())
                    (*regions)[regionmap[i][j]] = RegionPtr(new Region(RegionType::Region_Hills, regionmap[i][j]));
                (*regions)[regionmap[i][j]]->addSite(sf::Vector2i(i, j));
            }
            else if (regionmap[i][j] > flat_index_start && regionmap[i][j] <= flat_index_start + flat_regions)
            {
                regionmap_image->setPixel(i, j, sf::Color(14, 36, 20, 255));
                if (regions->find(regionmap[i][j]) == regions->end())
                    (*regions)[regionmap[i][j]] = RegionPtr(new Region(RegionType::Region_Flat, regionmap[i][j]));
                (*regions)[regionmap[i][j]]->addSite(sf::Vector2i(i, j));
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

void WorldGenerator::expandLake(sf::Vector2i position, ImagePtr img, int** rivermap, int rivercode)
{
    float height = img->getPixel(position.x, position.y).r;

    sf::Color elevated(height+1, 200, height+1, 255);

    if (position.x > 0)
    {
        img->setPixel(position.x-1, position.y, elevated);
        rivermap[position.x-1][position.y] = rivercode;
    }
    if (position.y > 0)
    {
        img->setPixel(position.x, position.y-1, elevated);
        rivermap[position.x][position.y-1] = rivercode;
    }
    if (position.x < img->getSize().x -1)
    {
        img->setPixel(position.x+1, position.y, elevated);
        rivermap[position.x+1][position.y] = rivercode;
    }
    if (position.y < img->getSize().y -1)
    {
        img->setPixel(position.x, position.y+1, elevated);
        rivermap[position.x][position.y+1] = rivercode;
    }
    if (position.x > 0 && position.y > 0)
    {
        img->setPixel(position.x-1, position.y-1, elevated);
        rivermap[position.x-1][position.y-1] = rivercode;
    }
    if (position.x < img->getSize().x -1 && position.y > 0)
    {
        img->setPixel(position.x+1, position.y-1, elevated);
        rivermap[position.x+1][position.y-1] = rivercode;
    }
    if (position.x < img->getSize().x -1 && position.y < img->getSize().y-1)
    {
        img->setPixel(position.x+1, position.y+1, elevated);
        rivermap[position.x+1][position.y+1] = rivercode;
    }
    if (position.x > 0 && position.y < img->getSize().y-1)
    {
        img->setPixel(position.x-1, position.y+1, elevated);
        rivermap[position.x-1][position.y+1] = rivercode;
    }
}

/**
* Returns a vector of neighboring pixels that are lower in height
* Only matches that are exactly the local minimum height are returned
* For example, if height is 5, and neighbors involve 2, 2 and 3, then 3 won't be in the vector
*/
std::vector<std::pair<sf::Vector2i, float>> WorldGenerator::findLowerNeighbors(sf::Vector2i coords, int current_height, ImagePtr img, float tolerance)
{
    typedef std::pair<sf::Vector2i, float> heightdiff;

    std::vector<std::pair<sf::Vector2i, float>> out;

    int h = current_height + tolerance;

    float height = 0;
    float current_min = 2000;

    if (coords.x > 0)
    {
        height = img->getPixel(coords.x-1, coords.y).r;
        if (height < h)
        {
            out.push_back(heightdiff(sf::Vector2i(coords.x-1, coords.y), height));
            current_min = h;
        }
    }

    
    if (coords.y > 0)
    {
        height = img->getPixel(coords.x, coords.y-1).r;
        if (height < h)
            out.push_back(heightdiff(sf::Vector2i(coords.x, coords.y-1), height));
        if (height < current_min)
            current_min = height;
    }

    if (coords.x < img->getSize().x-1)
    {
        height = img->getPixel(coords.x+1, coords.y).r;
        if (height < h)
            out.push_back(heightdiff(sf::Vector2i(coords.x+1, coords.y), height));
        if (height < current_min)
            current_min = height;
    }

    if (coords.y < img->getSize().y-1)
    {
        height = img->getPixel(coords.x, coords.y+1).r;
        if (height < h)
            out.push_back(heightdiff(sf::Vector2i(coords.x, coords.y+1), height));
        if (height < current_min)
            current_min = height;
    }

    if (coords.x > 0 && coords.y > 0)
    {
        height = img->getPixel(coords.x-1, coords.y-1).r;
        if (height < h)
            out.push_back(heightdiff(sf::Vector2i(coords.x-1, coords.y-1), height));
        if (height < current_min)
            current_min = height;
    }

    if (coords.x < img->getSize().x-1 && coords.y > 0)
    {
        height = img->getPixel(coords.x+1, coords.y-1).r;
        if (height < h)
            out.push_back(heightdiff(sf::Vector2i(coords.x+1, coords.y-1), height));
        if (height < current_min)
            current_min = height;
    }

    if (coords.x < img->getSize().x-1 && coords.y < img->getSize().y-1)
    {
        height = img->getPixel(coords.x+1, coords.y+1).r;
        if (height < h)
            out.push_back(heightdiff(sf::Vector2i(coords.x+1, coords.y+1), height));
        if (height < current_min)
            current_min = height;
    }

    if (coords.x > 0 && coords.y < img->getSize().y-1)
    {
        height = img->getPixel(coords.x-1, coords.y+1).r;
        if (height < h)
            out.push_back(heightdiff(sf::Vector2i(coords.x-1, coords.y+1), height));
        if (height < current_min)
            current_min = height;
    }

    // Remove those matches which are greater than the current minimum
    /*
    out.erase(std::remove_if(out.begin(), out.end(), [current_min](heightdiff p)
        {
            return p.second > current_min;
        }),
        out.end());
    */

    return out;
}

sf::Vector2i WorldGenerator::getRandomDirection(sf::Vector2i coords, ImagePtr img)
{
    sf::Vector2i out = coords;

    if (app.getToolbox()->giveRandomFloat() > 0.5)
        out.x--;
    else
        out.x++;
    if (app.getToolbox()->giveRandomFloat() > 0.5)
        out.y++;
    else
        out.y--;

    if (out.x <= 0)
        out.x = 2;
    else if (out.x > img->getSize().x-1)
        out.x = img->getSize().x-3;
    if (out.y <= 0)
        out.y = 2;
    else if (out.y > img->getSize().y-1)
        out.y = img->getSize().y-3;

    return out;
}

/**
* Returns true if the specified coordinate is bordered by a river
* Returns false if not
* parameters:
* coord: The coordinate that is checked
* source: the coordinate from where this search is coming from. Ignored in the test.
*/
bool WorldGenerator::bordersRiver(sf::Vector2i coord, sf::Vector2i source, ImagePtr img, int** rivermap)
{
    if (coord.x < 0 || coord.y < 0 || coord.x > img->getSize().x-1 || coord.y > img->getSize().y-1)
        return false;

    sf::Vector2i temp;

    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            temp.x = coord.x + i;
            temp.y = coord.y + j;

            if (temp.x >= 0 && temp.x < img->getSize().x -1)
            {
                if (temp.y >= 0 && temp.y < img->getSize().y -1)
                {
                    if (temp.x != source.x || temp.y != source.y)
                    {
                        if (rivermap[temp.x][temp.y] != -1 && rivermap[temp.x][temp.y] != rivermap[source.x][source.y])
                            return true;
                    }
                } 
            }
        }
    }

    // No bordering rivers except for source
    return false;
}

/**
* After height, wind and precipitation have been calculated,
* this function forms a network of rivers
*/
void WorldGenerator::runRivers()
{
    auto heightmap = app.getDataStorage()->getImage("heightmap");
    auto regionmap_image = app.getDataStorage()->getImage("regionmap");
    auto precipitation = app.getDataStorage()->getImage("precipitation_blurred");
    auto regionmap = app.getWorld()->getRegionMap();

    // Select all high rainfall points on mountain tiles
    // For each point, begin producing water input
    // Check all neighbors and find the one that is lowest and not yet visited
    // Move all water from this tile to that tile
    // Remove random amount of height from this tile

    int steps_taken = 0;
    int max_steps = 300;
    int height = 0;
    float preci = 0;
    int mountain_start = 103;
    bool drainageFound = false;
    float river_preci_limit = 200;
    int water_supply = 0;
    sf::Vector2i flow_direction;
    int random_choice = 0;
    int currentriver = 0;
    int** rivermap = app.getToolbox()->giveIntArray2D(heightmap->getSize().x, heightmap->getSize().y);
    int ocean_end_height = 10;
    float river_probability = 0.05f;
    float tolerance = 17;
    int rainfall_supply = 100;
    sf::Vector2i dir;
    sf::Vector2i current_coords;
    sf::Color rivercolor(150, 150, 255, 255);
    int river_start_height = 50;
    sf::Vector2i previous(-1, -1);
    float height_probability = 1.0;

    int oceanEndIndex = app.getWorld()->getOceanStartIndex() + app.getWorld()->getOceanRegions()->size();

    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            rivermap[i][j] = -1;
        }
    }

    float pixel_h = 0;

    // Iterate the region map, looking for areas to form rivers to
    for (int i = 0; i < regionmap_image->getSize().x; i++)
    {
        for (int j = 0; j < regionmap_image->getSize().y; j++)
        {
            current_coords = sf::Vector2i(i, j);

            preci = precipitation->getPixel(i, j).r;
            pixel_h = heightmap->getPixel(i, j).r;
            float r = app.getToolbox()->giveRandomFloat();
            float r2 = app.getToolbox()->giveRandomFloat();
            float r3 = app.getToolbox()->giveRandomFloat();
            float current_height = 0;
            steps_taken = 0;
            drainageFound = false;
            bool done = false;

            if (steps_taken > max_steps)
                std::cout << "maxed out" << std::endl;

            if (r3 > app.getToolbox()->linearInterpolate(0.0, 1.0, preci / river_preci_limit) && pixel_h > river_start_height && r < river_probability && r2 > app.getToolbox()->linearInterpolate(0.85, height_probability, pixel_h / 125.0))
            {
                currentriver++;
                while (drainageFound == false && steps_taken < max_steps)
                {
                    steps_taken++;

                    rivermap[current_coords.x][current_coords.y] = currentriver;
                    current_height = heightmap->getPixel(current_coords.x, current_coords.y).r;
                    rivercolor = heightmap->getPixel(current_coords.x, current_coords.y);
                    // TODO: temp code, change this to erosion
                    rivercolor.r = current_height-3;
                    rivercolor.g = current_height-3;
                    rivercolor.b = current_height-3;
                    heightmap->setPixel(current_coords.x, current_coords.y, rivercolor);
                    
                    auto lower = findLowerNeighbors(current_coords, current_height, heightmap, tolerance);
                    lower.erase(std::remove_if(lower.begin(), lower.end(),
                        [rivermap](const std::pair<sf::Vector2i, float> e)
                            { return rivermap[e.first.x][e.first.y] != -1; }),
                        lower.end());

                    std::pair<sf::Vector2i, float> choice;
                    if (lower.size() > 1)
                        choice = lower.at(app.getToolbox()->giveRandomInt(0, lower.size()-1));
                    else if (lower.size() == 0)
                    {
                        auto temp = getRandomDirection(current_coords, heightmap);
                        choice.first = temp;
                        choice.second = heightmap->getPixel(temp.x, temp.y).r;
                    }
                    else
                        choice = lower.at(0);

                    if (rivermap[choice.first.x][choice.first.y] != -1 && rivermap[choice.first.x][choice.first.y] != currentriver)
                        drainageFound = true;

                    if (bordersRiver(choice.first, current_coords, heightmap, rivermap))
                    {
                        drainageFound = true;
                        heightmap->setPixel(choice.first.x, choice.first.y, rivercolor);
                        rivermap[choice.first.x][choice.first.y] = currentriver;
                    }

                    current_coords = choice.first;
                    if (choice.second < ocean_end_height)
                        drainageFound = true;
                }
            }
        }
    }

    app.getWorld()->setRiverMap(rivermap);
    app.getWorld()->setHeightmapImage(heightmap);
    
    std::shared_ptr<sf::Texture> heightmap_text = app.getDataStorage()->getTexture("heightmap");

    heightmap_text->update(*heightmap);
    std::shared_ptr<sf::Sprite> heightmap_sprite = app.getDataStorage()->getSprite("heightmap");
    heightmap_sprite->setTexture(*heightmap_text);
}

/**
* After height is generated, this function forms temperature zones
*/
void WorldGenerator::formTemperature()
{
    
}

/**
* After height, wind and temperature is formed,
* this function forms biomes
*/
void WorldGenerator::formBiomes()
{

}


/**
* Simple brute force Voronoi diagram generator
* Slow and not practical on the CPU
*/
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

