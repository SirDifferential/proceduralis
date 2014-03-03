#include "application.hpp"
#include "eventhandler.hpp"
#include "renderer.hpp"
#include "toolbox.hpp"
#include "datastorage.hpp"
#include "mouseHoverer.hpp"
#include "world.hpp"
#include "opencl_program.hpp"

EventHandler::EventHandler()
{
}

void EventHandler::checkEvents()
{
    sf::Event e;
    while (app.getRenderer()->getRenderWindow()->pollEvent(e))
    {
        switch (e.type)
        {
            case sf::Event::Closed:
                app.windowWasClosed();
                break;
            case sf::Event::Resized:
                app.getRenderer()->OnWindowResize();
                break;
            case sf::Event::KeyReleased:
                break;
            case sf::Event::KeyPressed:
                switch (e.key.code)
                {
                    case sf::Keyboard::A:
                        app.getCurrentCLProgram()->event1();
                        break;
                    case sf::Keyboard::S:
                        app.getCurrentCLProgram()->event2();
                        break;
                    case sf::Keyboard::Q:
                        app.getCurrentCLProgram()->event3();
                        break;
                    case sf::Keyboard::W:
                        app.getCurrentCLProgram()->event4();
                        break;
                    case sf::Keyboard::Z:
                        app.getCurrentCLProgram()->event5();
                        break;
                    case sf::Keyboard::X:
                        app.getCurrentCLProgram()->event6();
                        break;
                    case sf::Keyboard::Num1:
                        app.getWorld()->setActiveSprite("heightmap");
                        break;
                    case sf::Keyboard::Num2:
                        app.getWorld()->setActiveSprite("biomes");
                        break;
                    case sf::Keyboard::Num3:
                        app.getWorld()->setActiveSprite("temperature");
                        break;
                    case sf::Keyboard::Num4:
                        app.getWorld()->setActiveSprite("precipitation_blurred");
                        break;
                    case sf::Keyboard::Escape:
                        app.exit();
                        break;
                }
                break;
            case sf::Event::MouseMoved:
                int x = e.mouseMove.x;
                int y = e.mouseMove.y;
                app.getMouseHoverer()->updateCoords(sf::Vector2i(x, y));
                break;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        auto heightmap = app.getDataStorage()->getSprite("heightmap");
        auto pos = heightmap->getPosition();
        pos.x += 5;
        heightmap->setPosition(pos);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        auto heightmap = app.getDataStorage()->getSprite("heightmap");
        auto pos = heightmap->getPosition();
        pos.x -= 5;
        heightmap->setPosition(pos);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        auto heightmap = app.getDataStorage()->getSprite("heightmap");
        auto pos = heightmap->getPosition();
        pos.y += 5;
        heightmap->setPosition(pos);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        auto heightmap = app.getDataStorage()->getSprite("heightmap");
        auto pos = heightmap->getPosition();
        pos.y -= 5;
        heightmap->setPosition(pos);
    }
}
