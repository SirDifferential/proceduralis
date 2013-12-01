/**
* Common includes and typedefs to make compiling faster
*/

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

class World;

typedef std::unique_ptr<sf::Sprite> SpritePtr;
typedef std::unique_ptr<World> WorldPtr;
