#ifndef __RENDERER_HPP_
#define __RENDERER_HPP_

#include "precompiled.hpp"

class Renderer
{
private:
    RenderWindowPtr renderWindow;
public:
    Renderer();

    void draw(SpritePtr s);

    RenderWindowPtr getRenderWindow();
};

#endif

