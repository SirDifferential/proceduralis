#ifndef __APPLICATION_HPP_
#define __APPLICATIPN_HPP_

#include "precompiled.hpp"

class Application
{
private:
    RendererPtr renderer;
    ToolboxPtr toolbox;

	bool windowIsOpen;
public:
    Application();

    int readConfig();

    int run();
    int exit();

	void windowWasClosed();

    RendererPtr getRenderer();
    ToolboxPtr getToolbox();
};

extern Application app;

#endif

