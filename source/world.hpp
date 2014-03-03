#ifndef __WORLD_HPP_
#define __WORLD_HPP_

#include "precompiled.hpp"
#include "cl.hpp"

typedef struct
{
    int8_t biome_code;
    int8_t mountain_type;
} BiomeType;

class World
{
private:
    TexturePtr heightmap_texture;
    ImagePtr heightmap_image;
    ImagePtr temperature_image;
    ImagePtr precipitation_image;

    std::string activeSprite;

    int** regionmap;
    int** rivermap;

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

    std::shared_ptr<std::map<long, RegionPtr>> regions;

    std::map<std::pair<int,int>, BiomeType> biomes;

    sf::Vector2i world_reso;

public:
    World();
    SpritePtr getWorld();

    void init();
    void draw();
    void work();

    void setWorld(ImagePtr image);
    void setBiomeCodes(int8_t* data);

    void setActiveSprite(std::string s);

    void setRegion(int** regions);
    void setRiverMap(int** rivers);
    void setOceanRegions(std::shared_ptr<std::map<int,int>> r);
    void setMountainRegions(std::shared_ptr<std::map<int,int>> r);
    void setHillRegions(std::shared_ptr<std::map<int,int>> r);
    void setFlatRegions(std::shared_ptr<std::map<int,int>> r);
    void setTemperatureImage(ImagePtr img);
    void setPrecipitationImage(ImagePtr img);

    int** getRegionMap();
    std::shared_ptr<std::map<long, RegionPtr>> getRegions();
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

