#include "eventhandler.hpp"
#include "application.hpp"
#include "renderer.hpp"
#include "toolbox.hpp"

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
				break;
		}
	}

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        // Do something with left
    }
}
