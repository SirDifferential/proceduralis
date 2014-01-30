#ifndef __WORLDGENERATOR_HPP_
#define __WORLDGENERATOR_HPP_

#include "precompiled.hpp"

class WorldGenerator
{
private:
public:
    WorldGenerator();
    void init();

    void generate();
    ImagePtr voronoi();

    void formSuperRegions();
    void runRivers();
};

#endif

