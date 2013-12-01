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
				sf::Vector2u temp = app.getRenderer()->getRenderWindow()->getSize();
				std::cout << "+EventHandler: Window was resized to ";
				app.getToolbox()->print_vector(temp);
				break;
		}
	}
}
