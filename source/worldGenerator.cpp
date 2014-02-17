#include "worldGenerator.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "datastorage.hpp"
#include "spriteutils.hpp"
#include "region.hpp"

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
    SpritePtr winddir_blur = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("windblurred", winddir_blur);
    TexturePtr winddir_text_blur = TexturePtr(new sf::Texture());
    winddir_text->create(1024, 1024);
    app.getDataStorage()->storeTexture("windblurred", winddir_text_blur);
    SpritePtr preci = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("precipitation", preci);
    TexturePtr preci_text = TexturePtr(new sf::Texture());
    preci_text->create(1024, 1024);
    app.getDataStorage()->storeTexture("precipitation", preci_text);
    ImagePtr preci_img = ImagePtr(new sf::Image());
    preci_img->create(1024, 1024);
    app.getDataStorage()->storeImage("precipitation", preci_img);
    SpritePtr preci_blur = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("precipitation_blurred", preci_blur);
    TexturePtr preci_text_blur = TexturePtr(new sf::Texture());
    preci_text->create(1024, 1024);
    app.getDataStorage()->storeTexture("precipitation_blurred", preci_text_blur);
    ImagePtr preci_img_blur = ImagePtr(new sf::Image());
    preci_img->create(1024, 1024);
    app.getDataStorage()->storeImage("precipitation_blurred", preci_img_blur);
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

/**
* Returns a vector of neighboring pixels that are lower in height
* Only matches that are exactly the local minimum height are returned
* For example, if height is 5, and neighbors involve 2, 2 and 3, then 3 won't be in the vector
*/
std::vector<std::pair<sf::Vector2i, float>> WorldGenerator::findLowerNeighbors(sf::Vector2i coords, int h, ImagePtr img)
{
    typedef std::pair<sf::Vector2i, float> heightdiff;

    std::vector<std::pair<sf::Vector2i, float>> out;

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
    out.erase(std::remove_if(out.begin(), out.end(), [current_min](heightdiff p)
        {
            return p.second > current_min;
        }),
        out.end());

    return out;
}

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
    int max_steps = 5000;
    int height = 0;
    float preci = 0;
    int mountain_start = 103;
    bool drainageFound = false;
    int river_limit = 200;
    int water_supply = 0;
    std::vector<sf::Vector2i> visited_points;
    sf::Vector2i flow_direction;
    int random_choice = 0;
    sf::Vector2i current_position;
    int** rivermap = app.getToolbox()->giveIntArray2D(heightmap->getSize().x, heightmap->getSize().y);

    int oceanEndIndex = app.getWorld()->getOceanStartIndex() + app.getWorld()->getOceanRegions()->size();

    for (int i = 0; i < heightmap->getSize().x; i++)
    {
        for (int j = 0; j < heightmap->getSize().y; j++)
        {
            rivermap[i][j] = -1;
        }
    }

    // Iterate the region map, looking for areas to form rivers to
    for (int i = 0; i < regionmap_image->getSize().x; i++)
    {
        for (int j = 0; j < regionmap_image->getSize().y; j++)
        {
            height = heightmap->getPixel(i, j).r;
            current_position = sf::Vector2i(i, j);
            // If this pixel is great enough in height for rivers to form
            if (height > mountain_start)
            {
                preci = precipitation->getPixel(i, j).r;
                
                // If this area has high enough rainfall to produce rivers
                if (preci >= river_limit)
                {
                    visited_points.push_back(sf::Vector2i(i,j));
                    water_supply = 100;

                    steps_taken = 0;

                    // Begin stepping the river down towards drainage
                    while (steps_taken < max_steps && drainageFound == false)
                    {
                        if (regionmap[current_position.x][current_position.y] <= oceanEndIndex)
                            drainageFound = true;

                        // Form the river by decreasing height one unit
                        height = heightmap->getPixel(current_position.x, current_position.y).r;
                        heightmap->setPixel(current_position.x, current_position.y, sf::Color(height-1, 255, height-1, 255));

                        // Check neighbors that allow flowing down
                        auto matches = findLowerNeighbors(sf::Vector2i(i, j), height, heightmap);

                        // remove neighbors that are already in the rivermap
                        matches.erase(std::remove_if(matches.begin(), matches.end(),
                            [rivermap](const std::pair<sf::Vector2i, float> e)
                                { return rivermap[e.first.x][e.first.y] != -1; }),
                            matches.end());

                        // No neighbors are lower, form lake
                        if (matches.size() <= 0)
                        {
                            // Temp code, just return
                            drainageFound = true;
                        }
                        else
                        {
                            // Choose random direction
                            if (matches.size() > 1)
                                random_choice = app.getToolbox()->giveRandomInt(0, matches.size()-1);
                            else
                                random_choice = 0;

                            auto flow_direction = matches.at(random_choice);

                            current_position = flow_direction.first;
                        }
                    }
                }
            }
        }
    }
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

