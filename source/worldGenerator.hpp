#ifndef __WORLDGENERATOR_HPP_
#define __WORLDGENERATOR_HPP_

#include "precompiled.hpp"

class WorldGenerator
{
private:
    std::map<int, std::vector<sf::Vector2i>> seas;
    std::vector<sf::Vector2i> sea_pixels;
public:
    WorldGenerator();
    void init();

    void generate();
    ImagePtr voronoi();

    void formSuperRegions();
    void runRivers();
};

#endif

