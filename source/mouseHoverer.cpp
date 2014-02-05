#include "mouseHoverer.hpp"

MouseHoverer::MouseHoverer()
{
    current_coords = sf::Vector2i(10,10);
}

void MouseHoverer::updateCoords(sf::Vector2i coords)
{
    current_coords = coords;
}

sf::Vector2i MouseHoverer::getCoords()
{
    return current_coords;
}
