#ifndef __BIOMETOOLS_HPP_
#define __BIOMETOOLS_HPP_

#include "precompiled.hpp"
#include "cl.hpp"

class BiomeTools
{
private:
public:
    BiomeTools();

    std::string biomeCodeToString(int8_t code);
    std::string mountainCodeToString(int8_t code);
};

#endif
