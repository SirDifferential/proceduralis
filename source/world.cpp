#include "world.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "renderer.hpp"
#include "datastorage.hpp"
#include "mouseHoverer.hpp"
#include "gui.hpp"
#include "region.hpp"

World::World()
{
    ocean_index_start = 50;
    mountain_index_start = 5000;
    hill_index_start = 10000;
    flat_index_start = 20000;

    ocean_region_sizes = std::shared_ptr<std::map<int,int>>(new std::map<int,int>());
    mountain_region_sizes = std::shared_ptr<std::map<int,int>>(new std::map<int,int>());
    hill_region_sizes = std::shared_ptr<std::map<int,int>>(new std::map<int,int>());
    flat_region_sizes = std::shared_ptr<std::map<int,int>>(new std::map<int,int>());

    world_reso = sf::Vector2i(1024, 1024);

    regions = std::shared_ptr<std::map<long, RegionPtr>>(new std::map<long, RegionPtr>());
}

void World::init()
{
    TexturePtr heightmap_tex = TexturePtr(new sf::Texture());
    SpritePtr heightmap = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("heightmap", heightmap);
    app.getDataStorage()->storeTexture("heightmap", heightmap_tex);
}

void World::draw()
{
    // Temp code: Render all sprites next to each other
    auto s1 = app.getDataStorage()->getSprite("voronoi_cells");
    auto s2 = app.getDataStorage()->getSprite("perlinnoise");
    auto s3 = app.getDataStorage()->getSprite("heightmap");
    auto s4 = app.getDataStorage()->getSprite("voronoiblurred");
    auto s5 = app.getDataStorage()->getSprite("perlinblurred");
    auto s6 = app.getDataStorage()->getSprite("regionmap");
    auto s7 = app.getDataStorage()->getSprite("winddirections");
    auto s8 = app.getDataStorage()->getSprite("precipitation");
    auto s9 = app.getDataStorage()->getSprite("windblurred");
    auto s10 = app.getDataStorage()->getSprite("precipitation_blurred");
    auto s11 = app.getDataStorage()->getSprite("temperature");

    //s1->setScale(0.2, 0.2);
    //s2->setScale(0.2, 0.2);
    s3->setScale(0.2, 0.2);
    //s4->setScale(0.2, 0.2);
    //s5->setScale(0.2, 0.2);
    s6->setScale(0.2, 0.2);
    //s7->setScale(0.2, 0.2);
    s8->setScale(0.2, 0.2);
    s9->setScale(0.2, 0.2);
    s10->setScale(0.2, 0.2);
    s11->setScale(0.2, 0.2);


    //s1->setPosition(10, 10);
    //s2->setPosition(220, 10);
    //s3->setPosition(440, 10);
    //s4->setPosition(10, 220);
    //s5->setPosition(440, 220);
    s6->setPosition(440, 220);
    //s7->setPosition(10, 10);
    s8->setPosition(220, 10);
    s9->setPosition(10, 220);
    s10->setPosition(10, 10);
    s11->setPosition(220, 10);

    //app.getRenderer()->getRenderWindow()->draw(*s1);
    //app.getRenderer()->getRenderWindow()->draw(*s2);
    app.getRenderer()->getRenderWindow()->draw(*s3);
    //app.getRenderer()->getRenderWindow()->draw(*s4);
    //app.getRenderer()->getRenderWindow()->draw(*s5);
    //app.getRenderer()->getRenderWindow()->draw(*s6);
    //app.getRenderer()->getRenderWindow()->draw(*s7);
    //app.getRenderer()->getRenderWindow()->draw(*s8);
    //app.getRenderer()->getRenderWindow()->draw(*s9);
    //app.getRenderer()->getRenderWindow()->draw(*s10);
    app.getRenderer()->getRenderWindow()->draw(*s11);
}

void World::work()
{
    // Check where the mouse is hovering
    sf::Vector2i coords = app.getMouseHoverer()->getCoords();

    draw();

    /*
    // Get the region type and name for those coords
    if (coords.x > 0 && coords.y > 0 && coords.x < world_reso.x && coords.y < world_reso.y)
    {
        int code = regionmap[coords.x][coords.y];
        if (code > ocean_index_start && code < ocean_index_start + ocean_regions)
        {
            app.getGUI()->drawLocationText("Ocean, sized ", (*ocean_region_sizes)[code]);
        }
        else if (code > mountain_index_start && code < mountain_index_start + mountain_regions)
        {
            app.getGUI()->drawLocationText("Mountains, sized ", (*mountain_region_sizes)[code]);
        }
        else if (code > hill_index_start && code < hill_index_start + hill_regions)
        {
            app.getGUI()->drawLocationText("Hills, sized ", (*hill_region_sizes)[code]);
        }
        else if (code > flat_index_start && code < flat_index_start + flat_regions)
        {
            app.getGUI()->drawLocationText("Flats, sized ", (*flat_region_sizes)[code]);
        }
    }
    */

}

void World::setWorld(ImagePtr image)
{
    app.getDataStorage()->deleteImage("heightmap");
    app.getDataStorage()->deleteTexture("heightmap");
    app.getDataStorage()->deleteSprite("heightmap");

    sf::Color c;
    c = image->getPixel(10, 10);

    TexturePtr spriteTexture = TexturePtr(new sf::Texture());
    spriteTexture->create(1024, 1024);
    spriteTexture->update(*image);
    SpritePtr heightmap = SpritePtr(new sf::Sprite());
    heightmap->setTexture(*spriteTexture);

    app.getDataStorage()->storeImage("heightmap", image);
    app.getDataStorage()->storeTexture("heightmap", spriteTexture);
    app.getDataStorage()->storeSprite("heightmap", heightmap);
}

SpritePtr World::getWorld()
{
    return app.getDataStorage()->getSprite("heightmap");
}

void World::setRegion(int** regions)
{
    //if (regionmap != regions)
    //    app.getToolbox()->deleteIntArray2D(regionmap, app.getDataStorage()->getImage("heightmap")->getSize().x);
    regionmap = regions;
}

void World::setOceanRegions(std::shared_ptr<std::map<int,int>> r)
{
    ocean_region_sizes = r;
    ocean_regions = r->size();
}

void World::setMountainRegions(std::shared_ptr<std::map<int,int>> r)
{
    mountain_region_sizes = r;
    mountain_regions = r->size();
}

void World::setHillRegions(std::shared_ptr<std::map<int,int>> r)
{
    hill_region_sizes = r;
    hill_regions = r->size();
}

void World::setFlatRegions(std::shared_ptr<std::map<int,int>> r)
{
    flat_region_sizes = r;
    flat_regions = r->size();
}

int** World::getRegionMap()
{
    return regionmap;
}

std::shared_ptr<std::map<int,int>> World::getOceanRegions()
{
    return ocean_region_sizes;
}

std::shared_ptr<std::map<int,int>> World::getMountainRegions()
{
    return mountain_region_sizes;
}

std::shared_ptr<std::map<int,int>> World::getHillRegions()
{
    return hill_region_sizes;
}

std::shared_ptr<std::map<int,int>> World::getFlatRegions()
{
    return flat_region_sizes;
}

int World::getOceanStartIndex() { return ocean_index_start; }
int World::getMountainStartIndex() { return mountain_index_start; }
int World::getHillStartIndex() { return hill_index_start; }
int World::getFlatStartIndex() { return flat_index_start; }
std::shared_ptr<std::map<long, RegionPtr>> World::getRegions() { return regions; }
