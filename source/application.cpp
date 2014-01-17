#include "application.hpp"
#include "opencl_program.hpp"
#include "renderer.hpp"
#include "json/json.h"
#include "toolbox.hpp"
#include "eventhandler.hpp"
#include "applicationFlags.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "textrenderer.hpp"
#include "worldGenerator.hpp"
#include "datastorage.hpp"
#include "spriteutils.hpp"
#include "cl_voronoi.hpp"
#include "cl_perlin.hpp"
#include "cl_blur.hpp"
#include <fstream>
#include <chrono>
#include <iostream>
#include <ctime>

Application app;

Application::Application()
{
    renderer = RendererPtr(new Renderer());
    toolbox = ToolboxPtr(new Toolbox());
	eventhandler = EventHandlerPtr(new EventHandler());
    applicationFlags = ApplicationFlagsPtr(new ApplicationFlags());
    world = WorldPtr(new World());
    cl_voronoi = CL_VoronoiPtr(new CL_Voronoi("voronoi.cl"));
    cl_perlin = CL_PerlinPtr(new CL_Perlin("perlin.cl"));
    cl_blur = CL_BlurPtr(new CL_Blur("blur.cl"));
    gui = GUIPtr(new GUI());
    textrenderer = TextRendererPtr(new TextRenderer());
    worldgenerator = WorldGeneratorPtr(new WorldGenerator());
    datastorage = DataStoragePtr(new DataStorage());
    spriteutils = SpriteUtilsPtr(new SpriteUtils());
}

int Application::readConfig()
{
    std::string config_path = "config.json";
    Json::Value root;
    Json::Reader reader;

    std::ifstream configdata(config_path);
    bool parsingSuccessful = reader.parse(configdata, root);
    if (!parsingSuccessful)
    {
        std::cout << "-Application: Error loading config data " << config_path << ": " << reader.getFormattedErrorMessages() << std::endl;
        configdata.close();
        return -1;
    }

    std::cout << "+Application: Loaded config data successfully" << std::endl;

    std::cout << "+Application: Set title to " << root["title"].asString() << std::endl;
    renderer->setTitle(root["title"].asString());
    sf::Vector2f temp;
    temp.x = root["resolution"].get("x", 800).asInt();
    temp.y = root["resolution"].get("y", 600).asInt();
    std::cout << "+Application: Set resolution to ";
    toolbox->print_vector(temp);
    renderer->setResolution(temp);

    std::cout << "+Application: Set framerate to " << root["framerate"].asInt() << std::endl;
    renderer->setFramerate(root["framerate"].asInt());

	if (root["show_fps"].asString() == "yes")
		showFPS = true;
	else
		showFPS = false;

    if (root["gpu_device_debug"].asString() == "yes")
        applicationFlags->opencl_devices_debug = true;
    else
        applicationFlags->opencl_devices_debug = false;

    if (root["use_GPU"].asString() == "yes")
        applicationFlags->use_GPU = true;
    else
        applicationFlags->use_GPU = false;

    if (root["print_cl_programs"].asString() == "yes")
        applicationFlags->print_cl_programs = true;
    else
        applicationFlags->print_cl_programs = false;

    configdata.close();

    std::cout << "+Application: Finished reading config" << std::endl;

    return 0;
}

int Application::run()
{
    int fps = 0;
    int nextFPS = 3;
    sf::Clock fps_clock;
    sf::Clock main_clock;

    renderer->openWindow();
    windowIsOpen = true;
    renderer->showSplash();

    world->init();
    worldgenerator->init();

    int start_s = clock();

    cl_voronoi->loadProgram();
    cl_voronoi->setOutputTarget(datastorage->getSprite("voronoi_cells"));

    cl_perlin->loadProgram();
    cl_perlin->setOutputTarget(datastorage->getSprite("perlinnoise"));

    cl_blur->loadProgram();
    cl_blur->setOutputTarget(datastorage->getSprite("blurred"));

    programs["voronoi"] = cl_voronoi;
    programs["perlin"] = cl_perlin;
    programs["blur"] = cl_blur;

    activeCLProgram = programs["voronoi"];
    activeCLProgram->runKernel();

    cl_perlin->runKernel();

    worldgenerator->formSuperRegions();

    int stop_s = clock();
    std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;

    while (renderer->getRenderWindow()->isOpen() && windowIsOpen)
    {
        fps = 1 / fps_clock.getElapsedTime().asSeconds();
		fps_clock.restart();
        if (main_clock.getElapsedTime().asSeconds() > nextFPS)
        {
            nextFPS += 1;
            renderer->updateTitleFPS(fps);
            //std::cout << fps << std::endl);
        }

        renderer->getRenderWindow()->clear();
        eventhandler->checkEvents();
        world->work();
        renderer->work();
    }

    return exit();
}

int Application::exit()
{
    if (windowIsOpen)
        renderer->closeWindow();
    std::cout << "+Application exiting normally" << std::endl;
    return 0;
}

void Application::windowWasClosed()
{
    windowIsOpen = false;
}

/**
* Quick ugly hack to see output faster
* TODO: delete this later
*/
void Application::forceredraw()
{
    world->work();
    renderer->work();
}

void Application::setProgram(std::string name)
{
    if (programs.find(name) == programs.end())
    {
        std::cout << "-Application: No program " << name << " found. Loaded programs: " << programs.size() << std::endl;
        return;
    }

    activeCLProgram = programs[name];
    std::cout << "+Application: Active program set to: " << activeCLProgram->getSourceName() << std::endl;
    activeCLProgram->runKernel();
}

CL_ProgramPtr Application::getProgram(std::string name)
{
    auto p = programs.find(name);
    if (p == programs.end())
    {
        std::cout << "-Application: No program " << name << " loaded" << std::endl;
        return nullptr;
    }
    return p->second;
}

RendererPtr Application::getRenderer()
{
    return renderer;
}

ToolboxPtr Application::getToolbox()
{
    return toolbox;
}

ApplicationFlagsPtr Application::getApplicationFlags()
{
    return applicationFlags;
}

WorldPtr Application::getWorld()
{
    return world;
}

CL_ProgramPtr Application::getCurrentCLProgram()
{
    return activeCLProgram;
}

GUIPtr Application::getGUI()
{
    return gui;
}

TextRendererPtr Application::getTextRenderer()
{
    return textrenderer;
}

WorldGeneratorPtr Application::getWorldGenerator()
{
    return worldgenerator;
}

DataStoragePtr Application::getDataStorage()
{
    return datastorage;
}

SpriteUtilsPtr Application::getSpriteUtils()
{
    return spriteutils;
}

