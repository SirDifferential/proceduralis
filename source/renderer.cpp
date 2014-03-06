#include "renderer.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "gui.hpp"
#include "datastorage.hpp"

Renderer::Renderer()
{
}

void Renderer::openWindow()
{
    renderWindow = RenderWindowPtr(new sf::RenderWindow());
    renderWindow->create(sf::VideoMode(resolution.x, resolution.y), title);
    renderWindow->setFramerateLimit(framerate);
	main_view = ViewPtr(new sf::View(sf::FloatRect(0, 0, renderWindow->getSize().x/2,renderWindow->getSize().y/2)));
	OnWindowResize();

    splashSprite = app.getDataStorage()->loadAndGiveSprite("splashImage", "data/2D/splash.png");
}

void Renderer::closeWindow()
{
    renderWindow->close();
}

void Renderer::showSplash()
{
    renderWindow->draw(*splashSprite);
    renderWindow->display();
}

void Renderer::workLoading()
{
	renderWindow->setView(*main_view);
    app.getGUI()->drawLoading();
	renderWindow->display();
}

void Renderer::work()
{
	renderWindow->setView(*main_view);
    app.getGUI()->drawNormal();
	renderWindow->display();
}

void Renderer::draw(SpritePtr s)
{
    renderWindow->draw(*s);
}

void Renderer::updateTitleFPS(int fps)
{
    renderWindow->setTitle(app.getToolbox()->combineStringAndInt(app.getToolbox()->combineStringAndString(title, ", FPS: "), fps));
}

void Renderer::OnWindowResize()
{
	sf::Vector2u temp = renderWindow->getSize();
	std::cout << "+EventHandler: Window was resized to ";
	app.getToolbox()->print_vector(temp);

    display_area_y = renderWindow->getSize().y;
    display_area_x = display_area_y*aspect_ratio;

    display_ratio_x = display_area_x/renderWindow->getSize().x;
    display_ratio_y = display_area_y/renderWindow->getSize().y;
    display_offset_x = -1 * display_ratio_x + 1;
    display_offset_y = -1 * display_ratio_y + 1;   

    main_view->setViewport(sf::FloatRect(display_offset_x/2.0, display_offset_y/2.0, 1.0 - display_offset_x, 1.0 - display_offset_y));
}

RenderWindowPtr Renderer::getRenderWindow()
{
    return renderWindow;
}

void Renderer::setResolution(sf::Vector2f res)
{
    resolution = res;
	display_area_x = resolution.x;
	display_area_y = resolution.y;
	aspect_ratio = display_area_x / display_area_y;
}

void Renderer::setTitle(std::string t)
{
    title = t;
}

void Renderer::setFramerate(int r)
{
    framerate = r;
}

