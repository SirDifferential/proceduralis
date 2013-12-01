#include "renderer.hpp"

Renderer::Renderer()
{
}

void Renderer::draw(SpritePtr s)
{
    renderWindow->draw(*s);
}

RenderWindowPtr Renderer::getRenderWindow()
{
    return renderWindow;
}

void Renderer::setResolution(sf::Vector2f res)
{
    resolution = res;
}

void Renderer::setTitle(std::string t)
{
    title = t;
}

void Renderer::setFramerate(int r)
{
    framerate = r;
}

