#include "world.hpp"
#include "application.hpp"
#include "toolbox.hpp"
#include "renderer.hpp"
#include "datastorage.hpp"
World::World()
{
}

void World::init()
{
    SpritePtr heightmap = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("heightmap", heightmap);
}

void World::draw()
{
    app.getRenderer()->getRenderWindow()->draw(*app.getDataStorage()->getSprite("heightmap"));
}

void World::work()
{
    draw();
}

void World::setWorld(ImagePtr image)
{
    app.getDataStorage()->deleteImage("heightmap");
    app.getDataStorage()->deleteTexture("heightmap");
    app.getDataStorage()->deleteSprite("heightmap");

    sf::Color c;
    c = image->getPixel(10, 10);

    TexturePtr spriteTexture = TexturePtr(new sf::Texture());
    spriteTexture->create(1024, 1024);
    spriteTexture->update(*image);
    SpritePtr heightmap = SpritePtr(new sf::Sprite());
    heightmap->setTexture(*spriteTexture);

    app.getDataStorage()->storeImage("heightmap", image);
    app.getDataStorage()->storeTexture("heightmap", spriteTexture);
    app.getDataStorage()->storeSprite("heightmap", heightmap);
}

SpritePtr World::getWorld()
{
    return app.getDataStorage()->getSprite("heightmap");
}

