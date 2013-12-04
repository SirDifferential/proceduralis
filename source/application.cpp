#include "application.hpp"
#include "renderer.hpp"
#include "json/json.h"
#include "toolbox.hpp"
#include "eventhandler.hpp"
#include "applicationFlags.hpp"
#include "world.hpp"
#include <fstream>
#include <chrono>
#include <iostream>

Application app;

Application::Application()
{
    renderer = RendererPtr(new Renderer());
    toolbox = ToolboxPtr(new Toolbox());
	eventhandler = EventHandlerPtr(new EventHandler());
    applicationFlags = ApplicationFlagsPtr(new ApplicationFlags());
    world = WorldPtr(new World());
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
