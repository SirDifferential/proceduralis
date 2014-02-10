#include "region.hpp"

Region::Region(RegionType t, long i)
{
    sites = std::shared_ptr<std::vector<sf::Vector2i>>(new std::vector<sf::Vector2i>());
    regiontype = t;
    id = i;
}

void Region::addSite(sf::Vector2i p)
{
    sites->push_back(p);
}

RegionType Region::getRegionType()
{
    return regiontype;
}

std::shared_ptr<std::vector<sf::Vector2i>> Region::getSites()
{
    return sites;
}
