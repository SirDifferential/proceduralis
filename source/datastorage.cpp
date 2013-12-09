#include "datastorage.hpp"

DataStorage::DataStorage()
{
}

/**
* Loads the data that is required by the game for
* displaying the game before it officially starts
* This includes loading screens, loading music
* and similar
* Returns 0 if successful
*/
int DataStorage::loadInitialData()
{
    loadTextureAndStoreSprite("logo", "data/2D/engine_logo.png");
    loadSound("biisi", "data/audio/biisi.ogg");
    
    return 0;
}

/**
* Load all the game's content
* Returns 0 if successful
*/
int DataStorage::loadAllData()
{
    return 0;
}

/**
* Loads a new texture from the given path and stores the produced Texture in the textureContainer as a shared_ptr
* The texture is stored based on the given name
* Returns -1 if error, 0 if success
*/
int DataStorage::loadTexture(std::string name, std::string path)
{
    std::shared_ptr<sf::Texture> texture = std::shared_ptr<sf::Texture>(new sf::Texture());
    if (!texture->loadFromFile(path))
    {
        std::cout << "-DataStorage: Error loading file " << path << std::endl;
        return -1;
    }
    textureContainer[name] = texture;
    std::cout << "+DataStorage: Successfully loaded " << path << " as " << name << std::endl;
    return 0;
}

/**
* Loads a new texture from the given path and stores the produced Texture in the textureContainer as a shared_ptr
* Also creates a new sprite with the original dimensions from this texture
* Returns -1 if error, 0 if success
*/
int DataStorage::loadTextureAndStoreSprite(std::string name, std::string path)
{
    TexturePtr texture(new sf::Texture());
    if (!texture->loadFromFile(path))
    {
        std::cout << "-DataStorage: Error loading file: " << path << std::endl;
        return -1;
    }
    textureContainer[name] = texture;
    SpritePtr sp(new sf::Sprite(*texture));
    storeSprite(name, sp);

    std::cout << "+DataStorage: Successfully loaded " << path << " as " << name << std::endl;
    return 0;
}

/**
* Stores an existing sprite into the spriteContainer
*/
int DataStorage::storeSprite(std::string name, std::shared_ptr<sf::Sprite> s)
{
    spriteContainer[name] = s;
    return 0;
}

/**
* Loads a new texture from the filepath, and stores it in texture map by name
* Returns a SpritePtr containing the entire texture
*/
SpritePtr DataStorage::loadAndGiveSprite(std::string name, std::string filepath)
{
    loadTextureAndStoreSprite(name, filepath);
    return getSprite(name);
}

/**
* Creates a new texture and then returns a shared_ptr to it
* params:
* name: The name that will be given to the created sprite. Also stored in spriteContainer by this name
* textureName: The texture that will be used for this, searched from textureContainer
* sizeX, sizeY: The area of the rectangle that will be used out of the texture. You might want to use 100x100 of a 256x256 texture
* coordX, coordY: The offset in the texture. You may want to take a 100x100 region out of 256x256 texture, but not from top left corner
* Returns a pointer to the newly created texture, or nullptr if failed
*/
SpritePtr DataStorage::createAndGiveSprite(std::string name, std::string textureName, int sizeX, int sizeY, int coordX, int coordY)
{
    sf::IntRect subRect;
    subRect.left = coordX;
    subRect.top = coordY;
    subRect.width = sizeX;
    subRect.height = sizeY;
    TexturePtr texturePointer = getTexture(textureName);
    if (texturePointer == nullptr)
    {
        std::cout << "-DataStorage: Cannot create texture. Desired texture not found in memory: " << textureName << std::endl;
        return nullptr;
    }
    SpritePtr sprite(new sf::Sprite((*texturePointer), subRect));
    storeSprite(name, sprite);
    return sprite;
}

/**
* Loads a new sound by filename. Also creates a soundbuffer object and stores it
* Returns -1 if failure
*
* Does not support mp3. ogg is preferred over other formats
*/
int DataStorage::loadSound(std::string name, std::string path)
{
     SoundBufferPtr buffer(new sf::SoundBuffer());
    if (!(*buffer).loadFromFile(path))
    {
        std::cout << "-DataStorage: Error loading audio file " << path << std::endl;
        return -1;
    }
    soundBufferContainer[name] = buffer;
    std::shared_ptr<sf::Sound> sound = std::shared_ptr<sf::Sound>(new sf::Sound());
    sound->setBuffer((*buffer));
    soundContainer[name] = sound;
    std::cout << "+DataStorage: Successfully loaded " << path << "as " << name << std::endl;
    return 0;
}


/**
* Returns a shared_ptr<sf::Sprite> to a loaded sprite
* or nullptr if not found
*/
SpritePtr DataStorage::getSprite(std::string name)
{
    std::map<std::string, SpritePtr>::iterator iter;
    iter = spriteContainer.find(name);
    if (iter == spriteContainer.end())
    {
        std::cout << "-DataStorage: Can't find requested sprite in map: " << name << std::endl;
        return nullptr;
    }
    return iter->second;
}

/**
* Returns a shared_ptr<sf::Texture> to a loaded texture
* or nullptr if not found
*/
TexturePtr DataStorage::getTexture(std::string name)
{
    std::map<std::string, TexturePtr>::iterator iter;
    iter = textureContainer.find(name);
    if (iter == textureContainer.end())
    {
        std::cout << "-DataStorage: Can't find requested texture in map: " << name << std::endl;
        return nullptr;
    }
    return iter->second;
}

/**
* Returns a shared_ptr<sf::Sound> to a loaded sound
* or nullptr if not found
*/
SoundPtr DataStorage::getSound(std::string name)
{
    std::map<std::string, SoundPtr>::iterator iter;
    iter = soundContainer.find(name);
    if (iter == soundContainer.end())
    {
        std::cout << "-DataStorage: Can't find requested soundfile in map: " << name << std::endl;
        return nullptr;
    }
    return iter->second;
}

