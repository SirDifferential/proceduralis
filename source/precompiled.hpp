/**
* Common includes and typedefs to make compiling faster
*/

#include <memory>
#include <vector>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class World;
class Renderer;

typedef std::shared_ptr<sf::Sprite> SpritePtr;
typedef std::shared_ptr<World> WorldPtr;
typedef std::shared_ptr<sf::RenderWindow> RenderWindowPtr;
typedef std::shared_ptr<Renderer> RendererPtr;
