#ifndef __REGION_HPP_
#define __REGION_HPP_

#include "precompiled.hpp"

class Region
{
private:
    std::vector<sf::Vector2i> pixels;
    int size;
    sf::Vector2i weighted_center;
public:
    Region();
};

#endif
