#include "tile.hpp"
#include "application.hpp"
#include "renderer.hpp"

Tile::Tile()
{
}

void Tile::draw()
{
    sprite->setPosition(render_coordinates);
    game.getRenderer()->draw(sprite);
    
}

