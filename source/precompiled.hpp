/**
* Common includes and typedefs to make compiling faster
*/

#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <stack>
#include <functional>
#include <chrono>
#include <ctime>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class World;
class Renderer;
class Toolbox;
class EventHandler;
class ApplicationFlags;
class CL_Program;
class GUI;
class TextRenderer;
class WorldGenerator;
class DataStorage;
class SpriteUtils;
class MouseHoverer;
class CL_Voronoi;
class CL_Perlin;
class CL_Blur;

typedef std::shared_ptr<sf::Sprite> SpritePtr;
typedef std::shared_ptr<sf::Image> ImagePtr;
typedef std::shared_ptr<sf::Texture> TexturePtr;
typedef std::shared_ptr<sf::Sound> SoundPtr;
typedef std::shared_ptr<sf::SoundBuffer> SoundBufferPtr;
typedef std::shared_ptr<sf::RenderWindow> RenderWindowPtr;
typedef std::shared_ptr<sf::View> ViewPtr;
typedef std::shared_ptr<Renderer> RendererPtr;
typedef std::shared_ptr<Toolbox> ToolboxPtr;
typedef std::shared_ptr<World> WorldPtr;
typedef std::shared_ptr<EventHandler> EventHandlerPtr;
typedef std::shared_ptr<ApplicationFlags> ApplicationFlagsPtr;
typedef std::shared_ptr<CL_Program> CL_ProgramPtr;
typedef std::shared_ptr<GUI> GUIPtr;
typedef std::shared_ptr<TextRenderer> TextRendererPtr;
typedef std::shared_ptr<WorldGenerator> WorldGeneratorPtr;
typedef std::shared_ptr<DataStorage> DataStoragePtr;
typedef std::shared_ptr<SpriteUtils> SpriteUtilsPtr;
typedef std::shared_ptr<MouseHoverer> MouseHovererPtr;
typedef std::shared_ptr<CL_Voronoi> CL_VoronoiPtr;
typedef std::shared_ptr<CL_Perlin> CL_PerlinPtr;
typedef std::shared_ptr<CL_Blur> CL_BlurPtr;
