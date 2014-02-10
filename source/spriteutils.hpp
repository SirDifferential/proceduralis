#ifndef __SPRITEUTILS_HPP_
#define __SPRITEUTILS_HPP_

#include "precompiled.hpp"

class SpriteUtils
{
private:
public:
    SpriteUtils();

    void setPixels(SpritePtr target, std::string name, float* buffer, unsigned int width, unsigned int height);
    void setPixelsNorerange(SpritePtr target, std::string name, float* buffer, unsigned int width, unsigned int height);
    void setPixels(SpritePtr target, std::string name, float** buffer, unsigned int width, unsigned int height);
    void setPixels(SpritePtr target, std::string name, ImagePtr buffer);
};

#endif

