#ifndef __WORLD_HPP_
#define __WORLD_HPP_

#include "precompiled.hpp"

class World
{
private:
    TexturePtr heightmap_texture;
    ImagePtr heightmap_image;
    SpritePtr heightmap;
public:
    World();
    void setWorld(float* buffer, unsigned int width, unsigned int height);
    void setWorld(float** buffer, unsigned int width, unsigned int height);

    void draw();
    void work();
};

#endif

