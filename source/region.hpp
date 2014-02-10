#ifndef __REGION_HPP_
#define __REGION_HPP_

#include "precompiled.hpp"

typedef enum
{
    Region_Ocean = 1,
    Region_Flat = 2,
    Region_Hills = 3,
    Region_Mountains = 4
} RegionType;

class Region
{
private:
    long id;
    int number_of_sites;
    sf::Vector2i weighted_center;
    RegionType regiontype;
    std::shared_ptr<std::vector<sf::Vector2i>> sites;
public:
    Region(RegionType t, long i);
    void addSite(sf::Vector2i p);
    RegionType getRegionType();
    std::shared_ptr<std::vector<sf::Vector2i>> getSites();
};

#endif
