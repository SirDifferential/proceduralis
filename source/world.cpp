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
    TexturePtr heightmap_tex = TexturePtr(new sf::Texture());
    SpritePtr heightmap = SpritePtr(new sf::Sprite());
    app.getDataStorage()->storeSprite("heightmap", heightmap);
    app.getDataStorage()->storeTexture("heightmap", heightmap_tex);
}

void World::draw()
{
    // Temp code: Render all sprites next to each other
    auto s1 = app.getDataStorage()->getSprite("voronoi_cells");
    auto s2 = app.getDataStorage()->getSprite("perlinnoise");
    auto s3 = app.getDataStorage()->getSprite("heightmap");
    auto s4 = app.getDataStorage()->getSprite("voronoiblurred");
    auto s5 = app.getDataStorage()->getSprite("perlinblurred");

    s1->setScale(0.2, 0.2);
    s2->setScale(0.2, 0.2);
    s3->setScale(0.2, 0.2);
    s4->setScale(0.2, 0.2);
    s5->setScale(0.2, 0.2);

    s1->setPosition(10, 10);
    s2->setPosition(220, 10);
    s3->setPosition(220, 220);
    s4->setPosition(10, 220);
    s5->setPosition(440, 220);
    

    app.getRenderer()->getRenderWindow()->draw(*s1);
    app.getRenderer()->getRenderWindow()->draw(*s2);
    app.getRenderer()->getRenderWindow()->draw(*s3);
    app.getRenderer()->getRenderWindow()->draw(*s4);
    app.getRenderer()->getRenderWindow()->draw(*s5);
    
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

