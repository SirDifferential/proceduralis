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
    // Temp code: Render all sprites next to each other
    auto s1 = app.getDataStorage()->getSprite("voronoi_cells");
    auto s2 = app.getDataStorage()->getSprite("perlinnoise");
    auto s3 = app.getDataStorage()->getSprite("heightmap");

    s1->setScale(0.2, 0.2);
    s2->setScale(0.2, 0.2);
    s3->setScale(0.2, 0.2);

    s1->setPosition(0, 0);
    s2->setPosition(250, 0);
    s3->setPosition(500, 0);

    //app.getRenderer()->getRenderWindow()->draw(*app.getDataStorage()->getSprite("heightmap"));
    app.getRenderer()->getRenderWindow()->draw(*s1);
    app.getRenderer()->getRenderWindow()->draw(*s2);
    app.getRenderer()->getRenderWindow()->draw(*s3);
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

