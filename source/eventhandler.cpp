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
                        app.setProgram(0);
                        break;
                    case sf::Keyboard::Num2:
                        app.setProgram(1);
                        break;
                    case sf::Keyboard::Num3:
                        app.setProgram(2);
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
