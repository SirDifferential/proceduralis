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
    void formTemperature();
    void formBiomes();

    std::vector<std::pair<sf::Vector2i, float>> findLowerNeighbors(sf::Vector2i c, int h, ImagePtr img, float tolerance);
    void expandLake(sf::Vector2i position, ImagePtr img, int** rivermap, int rivercode);
    sf::Vector2i getRandomDirection(sf::Vector2i coords, ImagePtr img);
    bool bordersRiver(sf::Vector2i coord, sf::Vector2i source, ImagePtr img, int** rivermap);
};

#endif

