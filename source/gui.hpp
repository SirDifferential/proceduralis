#ifndef __GUI_HPP_
#define __GUI_HPP_

#include "precompiled.hpp"

class GUI
{
private:
public:
    GUI();

    void drawLoading();
    void drawNormal();

    void drawLocationText(std::string, int regionsize);
    void drawBiomeText(std::string biomeName, std::string mountainName);
    void drawTemperature(int temp);
    void drawPrecipitationText(int preci);
    void drawRiverText(int river);
    void drawLoadingText(std::string t);
    void drawHeight(int h);

    std::string octaveString;
    std::string persistenceString;
    std::string frequencyString;
    std::string randomString;
};

#endif
