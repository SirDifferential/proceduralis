#ifndef __WORLD_HPP_
#define __WORLD_HPP_

#include "precompiled.hpp"

class World
{
private:
    TexturePtr heightmap_texture;
    ImagePtr heightmap_image;
public:
    World();
    SpritePtr getWorld();

    void init();
    void draw();
    void work();

    void setWorld(ImagePtr image);
};

#endif

