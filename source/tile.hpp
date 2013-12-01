#ifndef __TILE_HPP_
#define __TILE_HPP_

#include "precompiled.hpp"

class Tile
{
private:
    SpritePtr sprite;
    sf::Vector2f render_coordinates;
public:
    Tile();

    void draw();
};

#endif

