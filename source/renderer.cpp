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

