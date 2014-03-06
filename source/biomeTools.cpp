#include "biomeTools.hpp"
#include "toolbox.hpp"
#include "application.hpp"

BiomeTools::BiomeTools()
{
}

std::string BiomeTools::biomeCodeToString(int8_t c)
{
    switch (c)
    {
        case 0: return "Ocean";
        case 1: return "Arctic";
        case 2: return "Tundra";
        case 3: return "Temperate Grassland";
        case 4: return "Temperate Desert";
        case 5: return "Savanna";
        case 6: return "Taiga";
        case 7: return "Tropical Decidious Forest";
        case 8: return "Tropical Seasonal Forest";
        case 9: return "Temperate Rain Forest";
        case 10: return "Tropical Rain Forest";
    }

    return app.getToolbox()->combineStringAndInt("Unknown biome code: ", c);
}

std::string BiomeTools::mountainCodeToString(int8_t c)
{
    switch (c)
    {
        case 0: return "Ocean bottom";
        case 1: return "Flats";
        case 2: return "Hills";
        case 3: return "Mountains";
    }
    return app.getToolbox()->combineStringAndInt("Unknown mountain code: ", c);

}
