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

SpritePtr World::getWorld()
{
    return app.getDataStorage()->getSprite("heightmap");
}

