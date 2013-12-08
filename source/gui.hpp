#ifndef __GUI_HPP_
#define __GUI_HPP_

#include "precompiled.hpp"

class GUI
{
private:
public:
    GUI();

    void draw();

    std::string octaveString;
    std::string persistenceString;
    std::string frequencyString;
    std::string randomString;
};

#endif
