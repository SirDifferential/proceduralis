#ifndef __RENDERER_HPP_
#define __RENDERER_HPP_

#include "precompiled.hpp"

class Renderer
{
private:
    RenderWindowPtr renderWindow;
    sf::Vector2f resolution;
    std::string title;
public:
    Renderer();

    void draw(SpritePtr s);

    RenderWindowPtr getRenderWindow();

    void setResolution(sf::Vector2f res);
    void setTitle(std::string t);
};

#endif

