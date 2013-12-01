#ifndef __APPLICATION_HPP_
#define __APPLICATIPN_HPP_

#include "precompiled.hpp"

class Application
{
private:
    RendererPtr renderer;
    ToolboxPtr toolbox;
public:
    Application();

    int readConfig();

    int run();
    int exit();

    RendererPtr getRenderer();
    ToolboxPtr getToolbox();
};

extern Application game;

#endif

