#ifndef __TEXTRENDERER_HPP_
#define __TEXTRENDERER_HPP_

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

enum FONT_SIZE { SMALL_FONT = 1, MEDIUM_FONT = 2, LARGE_FONT = 3 };

class TextRenderer
{
private:
    sf::Font font1;
public:
    TextRenderer();
    ~TextRenderer();

    void renderText(int x, int y, std::string message, FONT_SIZE s);
    void renderText(int x, int y, std::string message, FONT_SIZE s, bool bold);
    void renderText(int x, int y, std::string message, FONT_SIZE s, bool bold, sf::Color c);
};

#endif
