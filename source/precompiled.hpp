/**
* Common includes and typedefs to make compiling faster
*/

#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class World;
class Renderer;
class Toolbox;
class EventHandler;

typedef std::shared_ptr<sf::Sprite> SpritePtr;
typedef std::shared_ptr<sf::RenderWindow> RenderWindowPtr;
typedef std::shared_ptr<sf::View> ViewPtr;
typedef std::shared_ptr<Renderer> RendererPtr;
typedef std::shared_ptr<Toolbox> ToolboxPtr;
typedef std::shared_ptr<World> WorldPtr;
typedef std::shared_ptr<EventHandler> EventHandlerPtr;