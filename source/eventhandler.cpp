#include "application.hpp"
#include "eventhandler.hpp"
#include "renderer.hpp"
#include "toolbox.hpp"

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
                        app.getOpenCL()->event1();
                        break;
                    case sf::Keyboard::S:
                        app.getOpenCL()->event2();
                        break;
                    case sf::Keyboard::Q:
                        app.getOpenCL()->event3();
                        break;
                    case sf::Keyboard::W:
                        app.getOpenCL()->event4();
                        break;
                    case sf::Keyboard::Z:
                        app.getOpenCL()->event5();
                        break;
                    case sf::Keyboard::X:
                        app.getOpenCL()->event6();
                        break;
                    case sf::Keyboard::Escape:
                        app.exit();
                        break;
                }
				break;
		}
	}

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        // Do something with left
    }
}
