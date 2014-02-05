#ifndef __WORLDGENERATOR_HPP_
#define __WORLDGENERATOR_HPP_

#include "precompiled.hpp"

class WorldGenerator
{
private:
    std::map<int, std::vector<sf::Vector2i>> seas;
    std::vector<sf::Vector2i> sea_pixels;

    int** regionmap;

    int ocean_index_start;
    int mountain_index_start;
    int hill_index_start;
    int flat_index_start;

    int ocean_regions;
    int mountain_regions;
    int hill_regions;
    int flat_regions;
    
    std::map<int, int> ocean_region_sizes;
    std::map<int, int> moutain_region_sizes;
    std::map<int, int> hill_region_sizes;
    std::map<int, int> flat_region_sizes;

public:
    WorldGenerator();
    void init();

    void generate();
    ImagePtr voronoi();

    void formSuperRegions();
    void solveRegions(sf::Color code, sf::Color tolerance, std::map<int,int>& regionsizes, int regionCodeStartRange);
    void formRegions();
};

#endif

