#ifndef __WORLDGENERATOR_HPP_
#define __WORLDGENERATOR_HPP_

#include "precompiled.hpp"

class WorldGenerator
{
private:
    std::chrono::time_point<std::chrono::system_clock> timer1;
    std::chrono::time_point<std::chrono::system_clock> timer2;
public:
    WorldGenerator();
    void init();

    void generate();
    ImagePtr voronoi();

    void formSuperRegions();
    void solveRegions(int** regionmap, sf::Color code, sf::Color tolerance_down, std::shared_ptr<std::map<int,int>> regionsizes, int regionCodeStartRange);
    void formRegions();
    void runRivers();
};

#endif

