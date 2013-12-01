#include "application.hpp"

Application game;

Application::Application()
{
}

int Application::run()
{
    return 0;
}

RendererPtr Application::getRenderer()
{
    return renderer;
}

