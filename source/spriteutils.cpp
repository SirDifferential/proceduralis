#include "spriteutils.hpp"
#include "application.hpp"
#include "datastorage.hpp"

SpriteUtils::SpriteUtils()
{
}

void SpriteUtils::setPixels(SpritePtr target, std::string name, float* buffer, unsigned int width, unsigned int height)
{
    target.reset();
    app.getDataStorage()->deleteSprite(name);
    app.getDataStorage()->deleteTexture(name);
    app.getDataStorage()->deleteImage(name);

    ImagePtr sprite_image = ImagePtr(new sf::Image());
    sprite_image->create(width, height, sf::Color(0,0,0, 255));

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
        c.a = 255 + buffer[i]*255;
        sprite_image->setPixel(x, y, c);
    }

    TexturePtr sprite_texture = TexturePtr(new sf::Texture());
    sprite_texture->create(width, height);
    sprite_texture->update(*sprite_image);

    target = SpritePtr(new sf::Sprite());
    target->setTexture(*sprite_texture);

    app.getDataStorage()->storeImage(name, sprite_image);
    app.getDataStorage()->storeTexture(name, sprite_texture);
    app.getDataStorage()->storeSprite(name, target);

    auto temp = app.getDataStorage()->getSprite(name);

}

void SpriteUtils::setPixelsNorerange(SpritePtr target, std::string name, float* buffer, unsigned int width, unsigned int height)
{
    target.reset();
    app.getDataStorage()->deleteSprite(name);
    app.getDataStorage()->deleteTexture(name);
    app.getDataStorage()->deleteImage(name);

    ImagePtr sprite_image = ImagePtr(new sf::Image());
    sprite_image->create(width, height, sf::Color(0,0,0, 255));

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
        c.r = buffer[i-3];
        c.g = buffer[i-2];
        c.b = buffer[i-1];
        c.a = buffer[i];
        sprite_image->setPixel(x, y, c);
    }

    TexturePtr sprite_texture = TexturePtr(new sf::Texture());
    sprite_texture->create(width, height);
    sprite_texture->update(*sprite_image);

    target = SpritePtr(new sf::Sprite());
    target->setTexture(*sprite_texture);

    app.getDataStorage()->storeImage(name, sprite_image);
    app.getDataStorage()->storeTexture(name, sprite_texture);
    app.getDataStorage()->storeSprite(name, target);

    auto temp = app.getDataStorage()->getSprite(name);
}

void SpriteUtils::setPixels(SpritePtr target, std::string name, float** buffer, unsigned int width, unsigned int height)
{
    target.reset();

    app.getDataStorage()->deleteImage(name);
    app.getDataStorage()->deleteTexture(name);

    ImagePtr sprite_image = ImagePtr(new sf::Image());
    app.getDataStorage()->storeImage(name, sprite_image);
    sprite_image->create(width, height);
    sf::Color c;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            c.r = buffer[x][y] * 255;
            c.g = c.r;
            c.b = c.r;
            c.a = 255;
            sprite_image->setPixel(x, y, c);
        }
    }

    TexturePtr sprite_texture = TexturePtr(new sf::Texture());
    sprite_texture->create(width, height);
    sprite_texture->update(*sprite_image);

    target = SpritePtr(new sf::Sprite());
    target->setTexture(*sprite_texture);

    app.getDataStorage()->storeImage(name, sprite_image);
    app.getDataStorage()->storeTexture(name, sprite_texture);
    app.getDataStorage()->storeSprite(name, target);
}

/**
* Updates a Sprite
* Assumes that sprite image, texture and the sprite itself already exist
* Params:
* target: the Sprite to be updated
* name: The name by which the sprite is stored in the database
* buffer: ImagePtr containing the new pixel data
*/
void SpriteUtils::setPixels(SpritePtr target, std::string name, ImagePtr buffer)
{
    target.reset();
    app.getDataStorage()->deleteSprite(name);
    app.getDataStorage()->deleteTexture(name);
    auto sprite_texture = TexturePtr(new sf::Texture());
    sprite_texture->create(buffer->getSize().x, buffer->getSize().y);
    sprite_texture->update(*buffer);
    target = SpritePtr(new sf::Sprite());
    target->setTexture(*sprite_texture);
    app.getDataStorage()->storeSprite(name, target);
    app.getDataStorage()->storeTexture(name, sprite_texture);
}

