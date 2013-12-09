#ifndef __APPLICATION_HPP_
#define __APPLICATIPN_HPP_

#include "precompiled.hpp"

class Application
{
private:
    RendererPtr renderer;
    ToolboxPtr toolbox;
	EventHandlerPtr eventhandler;
    ApplicationFlagsPtr applicationFlags;
    WorldPtr world;
    CL_ProgramPtr opencl;
    GUIPtr gui;
    TextRendererPtr textrenderer;
    DataStoragePtr datastorage;

	bool windowIsOpen;
	bool showFPS;

public:
    Application();

    int readConfig();

    int run();
    int exit();

	void windowWasClosed();

    RendererPtr getRenderer();
    ToolboxPtr getToolbox();
    ApplicationFlagsPtr getApplicationFlags();
    WorldPtr getWorld();
    CL_ProgramPtr getOpenCL();
    GUIPtr getGUI();
    TextRendererPtr getTextRenderer();
    DataStoragePtr getDataStorage();
};

extern Application app;

#endif

