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
        case 1: return "Tundra";
        case 2: return "Taiga";
        case 3: return "Temperate Grassland";
        case 4: return "Subtropical Desert";
        case 5: return "Savanna";
        case 6: return "Temperate Deciduous Forest";
        case 7: return "Tropical Seasonal Forest";
        case 8: return "Temperate Rainforest";
        case 9: return "Tropical rainforest";
        
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
