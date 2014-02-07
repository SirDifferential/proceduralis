#include "gui.hpp"
#include "application.hpp"
#include "opencl_program.hpp"
#include "textrenderer.hpp"
#include "toolbox.hpp"
#include "mouseHoverer.hpp"

GUI::GUI()
{
}

void GUI::draw()
{
    app.getTextRenderer()->renderText(20, 20, octaveString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
    app.getTextRenderer()->renderText(20, 40, persistenceString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
    app.getTextRenderer()->renderText(20, 60, frequencyString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);

    sf::Vector2i coords = app.getMouseHoverer()->getCoords();
    //app.getTextRenderer()->renderText(20, 20, app.getToolbox()->vec2ToString(coords), FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
}

void GUI::drawLocationText(std::string regionType, int regionSize)
{
    //app.getTextRenderer()->renderText(20, 80, app.getToolbox()->combineStringAndInt(regionType, regionSize), FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
}

