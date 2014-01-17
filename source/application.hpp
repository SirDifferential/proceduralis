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
    GUIPtr gui;
    TextRendererPtr textrenderer;
    WorldGeneratorPtr worldgenerator;
    DataStoragePtr datastorage;
    SpriteUtilsPtr spriteutils;

    CL_ProgramPtr activeCLProgram;
    CL_VoronoiPtr cl_voronoi;
    CL_PerlinPtr cl_perlin;
    CL_BlurPtr cl_blur;

    std::map<std::string, std::shared_ptr<CL_Program>> programs;

	bool windowIsOpen;
	bool showFPS;

public:
    Application();

    int readConfig();

    int run();
    int exit();

	void windowWasClosed();

    void setProgram(std::string name);
    CL_ProgramPtr getProgram(std::string name);

    RendererPtr getRenderer();
    ToolboxPtr getToolbox();
    ApplicationFlagsPtr getApplicationFlags();
    WorldPtr getWorld();
    CL_ProgramPtr getCurrentCLProgram();
    GUIPtr getGUI();
    TextRendererPtr getTextRenderer();
    WorldGeneratorPtr getWorldGenerator();
    DataStoragePtr getDataStorage();
    SpriteUtilsPtr getSpriteUtils();
};

extern Application app;

#endif

