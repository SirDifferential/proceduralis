#include "gui.hpp"
#include "application.hpp"
#include "opencl_program.hpp"
#include "textrenderer.hpp"
#include "toolbox.hpp"
#include "mouseHoverer.hpp"
#include "renderer.hpp"
#include "applicationFlags.hpp"

GUI::GUI()
{
}

void GUI::drawLoading()
{
    if (app.getApplicationFlags()->use_GPU == false)
    {
        app.getTextRenderer()->renderText(20, 20, "I could not use your graphics card as OpenCL device. Please upgrade your GPU drivers.", FONT_SIZE::SMALL_FONT, false, sf::Color::Red);
    }

}

void GUI::drawNormal()
{
    //app.getTextRenderer()->renderText(20, 20, octaveString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
    //app.getTextRenderer()->renderText(20, 40, persistenceString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
    //app.getTextRenderer()->renderText(20, 60, frequencyString, FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);

    sf::Vector2i coords = app.getMouseHoverer()->getCoords();
    //app.getTextRenderer()->renderText(20, 20, app.getToolbox()->vec2ToString(coords), FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);

    app.getTextRenderer()->renderText(20, 60, "1, 2, 3 and 4 to change map mode", FONT_SIZE::SMALL_FONT, false, sf::Color::Red);

}

void GUI::drawLocationText(std::string regionType, int regionSize)
{
    app.getTextRenderer()->renderText(20, 80, app.getToolbox()->combineStringAndInt(regionType, regionSize), FONT_SIZE::SMALL_FONT, false, sf::Color::Red);
}

void GUI::drawTemperature(int t)
{
    app.getTextRenderer()->renderText(20, 100, app.getToolbox()->combineStringAndInt("Temperature: ", t), FONT_SIZE::SMALL_FONT, false, sf::Color::Red);
}

void GUI::drawBiomeText(std::string biomeText, std::string mountainText)
{
    app.getTextRenderer()->renderText(20, 120, biomeText, FONT_SIZE::SMALL_FONT, false, sf::Color::Red);
    app.getTextRenderer()->renderText(20, 140, mountainText, FONT_SIZE::SMALL_FONT, false, sf::Color::Red);
}

void GUI::drawPrecipitationText(int preci)
{
    app.getTextRenderer()->renderText(20, 160, app.getToolbox()->combineStringAndInt("Precipitation: ", preci), FONT_SIZE::SMALL_FONT, false, sf::Color::Red);
}

void GUI::drawRiverText(int river)
{
    if (river != -1)
        app.getTextRenderer()->renderText(20, 180, app.getToolbox()->combineStringAndInt("River number: ", river), FONT_SIZE::SMALL_FONT, false, sf::Color::Red);
}

void GUI::drawLoadingText(std::string t)
{
    app.getRenderer()->getRenderWindow()->clear();
    app.getTextRenderer()->renderText(20, 40, app.getToolbox()->combineStringAndString("Loading: ", t), FONT_SIZE::MEDIUM_FONT, false, sf::Color::Red);
    app.forceredraw();
}
