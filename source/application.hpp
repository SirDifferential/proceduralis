#ifndef __APPLICATION_HPP_
#define __APPLICATIPN_HPP_

#include "precompiled.hpp"

class Application
{
private:
    RendererPtr renderer;
public:
    Application();

    int readConfig();

    int run();
    RendererPtr getRenderer();
};

extern Application game;

#endif

