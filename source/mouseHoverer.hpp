#ifndef __MOUSEHOVERER_HPP_
#define __MOUSEHOVERER_HPP_

#include "precompiled.hpp"

class MouseHoverer
{
private:
    sf::Vector2i current_coords;
public:
    MouseHoverer();

    void updateCoords(sf::Vector2i coords);
    sf::Vector2i getCoords();
};

#endif
