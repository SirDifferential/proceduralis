#include "world.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "renderer.hpp"

World::World()
{
}

void World::setWorld(float* buffer, unsigned int width, unsigned int height)
{
    heightmap_image = ImagePtr(new sf::Image());
    heightmap_image->create(width, height, sf::Color(0,0,0, 255));

    sf::Color c;

    int x = -1;
    int y = 0;

    // Copy buffer in SFML image
    for (int i = 3; i < width*height*4; i += 4)
    {
        x += 1;
        if (x == width)
        {
            y += 1;
            x = 0;
        }
        c.r = buffer[i-3]*255;
        c.g = buffer[i-2]*255;
        c.b = buffer[i-1]*255;
        c.a = buffer[i]*255;
        heightmap_image->setPixel(x, y, c);
    }

    heightmap_texture = TexturePtr(new sf::Texture());
    heightmap_texture->create(width, height);
    heightmap_texture->update(*heightmap_image);
    
    heightmap = SpritePtr(new sf::Sprite());
    heightmap->setTexture(*heightmap_texture);
    
}

void World::setWorld(float** buffer, unsigned int width, unsigned int height)
{
    heightmap.reset();
    heightmap_texture.reset();
    heightmap_image.reset();

    heightmap_image = ImagePtr(new sf::Image());
    heightmap_image->create(width, height);
    sf::Color c;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            c.r = buffer[x][y] * 255;
            c.g = c.r;
            c.b = c.r;
            c.a = 255;
            heightmap_image->setPixel(x, y, c);
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
