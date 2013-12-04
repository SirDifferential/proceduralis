#include "world.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "renderer.hpp"

World::World()
{
}

void World::setWorld(std::shared_ptr<std::vector<float>> in, unsigned int width, unsigned int height)
{
    heightmap_raw = in;

    heightmap_image = ImagePtr(new sf::Image());
    heightmap_image->create(width, height, sf::Color(0,0,0, 255));
    
    

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            heightmap_image->setPixel(x, y, app.getToolbox()->int_to_sfmlcolor((int)in->at(x*y)));
        }
    }

    heightmap_texture = TexturePtr(new sf::Texture());
    heightmap_texture->create(width, height);
    heightmap_texture->update(*heightmap_image);
    
    heightmap = SpritePtr(new sf::Sprite());
    heightmap->setTexture(*heightmap_texture);
    
}

void World::draw()
{
    app.getRenderer()->getRenderWindow()->draw(*heightmap);
}

void World::work()
{
    draw();
}
