#ifndef __RENDERER_HPP_
#define __RENDERER_HPP_

#include "precompiled.hpp"

class Renderer
{
private:
    RenderWindowPtr renderWindow;
    sf::Vector2f resolution;
    std::string title;
    int framerate;

	ViewPtr main_view;
    float display_area_x;
    float display_area_y;
    float display_ratio_x;
    float display_ratio_y;
    float display_offset_x;
    float display_offset_y;
    float aspect_ratio;

    SpritePtr splashSprite;
public:
    Renderer();

    void draw(SpritePtr s);
	void work();
    void workLoading();

    RenderWindowPtr getRenderWindow();

    void openWindow();
    void closeWindow();

    void showSplash();

    void updateTitleFPS(int fps);
	void OnWindowResize();
    void setResolution(sf::Vector2f res);
    void setTitle(std::string t);
    void setFramerate(int r);
};

#endif

