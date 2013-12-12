#ifndef __WORLDGENERATOR_HPP_
#define __WORLDGENERATOR_HPP_

#include "precompiled.hpp"

class WorldGenerator
{
private:
public:
    WorldGenerator();

    void generate();
    float** diamondSquare();
    ImagePtr voronoi();
};

#endif

