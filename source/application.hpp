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
    MouseHovererPtr mousehoverer;
    BiomeToolsPtr biometools;

    CL_ProgramPtr activeCLProgram;
    CL_VoronoiPtr cl_voronoi;
    CL_PerlinPtr cl_perlin;
    CL_BlurPtr cl_blur;
    CL_WinddirPtr cl_winddir;
    CL_PrecipitationPtr cl_precipitation;
    CL_TemperaturePtr cl_temperature;
    CL_BiomesPtr cl_biomes;
    CL_SatellitePtr cl_satellite;

    std::map<std::string, std::shared_ptr<CL_Program>> programs;

	bool windowIsOpen;
	bool showFPS;

public:
    Application();

    int readConfig();

    int run();
    int exit();

	void windowWasClosed();
    void forceredraw();

    void setProgram(std::string name);
    CL_ProgramPtr getProgram(std::string name);

    void runProgram(std::string name);

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
    MouseHovererPtr getMouseHoverer();
    BiomeToolsPtr getBiomeTools();
};

extern Application app;

#endif

