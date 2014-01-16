#include "gui.hpp"
#include "application.hpp"
#include "opencl_program.hpp"
#include "textrenderer.hpp"

GUI::GUI()
{
}

void GUI::draw()
{
    app.getTextRenderer()->renderText(20, 20, octaveString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
    app.getTextRenderer()->renderText(20, 40, persistenceString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
    app.getTextRenderer()->renderText(20, 60, frequencyString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
}
