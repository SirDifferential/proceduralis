#ifndef __DATASTORAGE_HPP_
#define __DATASTORAGE_HPP_

#include "precompiled.hpp"

class DataStorage
{
private:
    std::map<std::string, SpritePtr> spriteContainer;
    std::map<std::string, TexturePtr> textureContainer;
    std::map<std::string, ImagePtr> imageContainer;
    std::map<std::string, SoundPtr> soundContainer;
    std::map<std::string, SoundBufferPtr> soundBufferContainer;
public:
    DataStorage();

    int storeSprite(std::string name, std::shared_ptr<sf::Sprite> s);
    int storeTexture(std::string name, std::shared_ptr<sf::Texture> t);
    int storeImage(std::string name, std::shared_ptr<sf::Image> i);
    int loadTexture(std::string name, std::string path);
    int loadTextureAndStoreSprite(std::string name, std::string path);
    SpritePtr createAndGiveSprite(std::string name, std::string textureName, int sizeX, int sizeY, int coordX, int coordY);
    SpritePtr loadAndGiveSprite(std::string name, std::string filepath);
    int loadSound(std::string name, std::string path);

    ImagePtr getImage(std::string name);
    SpritePtr getSprite(std::string name);
    TexturePtr getTexture(std::string name);
    SoundPtr getSound(std::string name);

    void deleteImage(std::string name);
    void deleteSprite(std::string name);
    void deleteTexture(std::string name);

    int loadInitialData();
    int loadAllData();
};

#endif

