#ifndef __WORLD_HPP_
#define __WORLD_HPP_

#include "precompiled.hpp"

class World
{
private:
    std::shared_ptr<std::vector<float>> heightmap_raw;
    TexturePtr heightmap_texture;
    ImagePtr heightmap_image;
    SpritePtr heightmap;
public:
    World();
    void setWorld(std::shared_ptr<std::vector<float>> in, unsigned int width, unsigned int height);

    void draw();
    void work();
};

#endif

