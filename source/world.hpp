#ifndef __WORLD_HPP_
#define __WORLD_HPP_

#include "precompiled.hpp"

class World
{
private:
    TexturePtr heightmap_texture;
    ImagePtr heightmap_image;

    int** regionmap;

    int ocean_index_start;
    int mountain_index_start;
    int hill_index_start;
    int flat_index_start;

    int ocean_regions;
    int mountain_regions;
    int hill_regions;
    int flat_regions;
    
    std::shared_ptr<std::map<int,int>> ocean_region_sizes;
    std::shared_ptr<std::map<int,int>> mountain_region_sizes;
    std::shared_ptr<std::map<int,int>> hill_region_sizes;
    std::shared_ptr<std::map<int,int>> flat_region_sizes;

    sf::Vector2i world_reso;

public:
    World();
    SpritePtr getWorld();

    void init();
    void draw();
    void work();

    //std::pair<std::string, int> getRegionInfo(sf::Vector2i c);

    void setWorld(ImagePtr image);

    void setRegion(int** regions);
    void setOceanRegions(std::shared_ptr<std::map<int,int>> r);
    void setMountainRegions(std::shared_ptr<std::map<int,int>> r);
    void setHillRegions(std::shared_ptr<std::map<int,int>> r);
    void setFlatRegions(std::shared_ptr<std::map<int,int>> r);

    int** getRegionMap();
    std::shared_ptr<std::map<int,int>> getOceanRegions();
    std::shared_ptr<std::map<int,int>> getMountainRegions();
    std::shared_ptr<std::map<int,int>> getHillRegions();
    std::shared_ptr<std::map<int,int>> getFlatRegions();

    int getOceanStartIndex();
    int getMountainStartIndex();
    int getHillStartIndex();
    int getFlatStartIndex();
};

#endif

