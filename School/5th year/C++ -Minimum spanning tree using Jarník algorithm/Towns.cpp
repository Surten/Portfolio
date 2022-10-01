#include "Towns.h"

Town::Town()
{}

Town::~Town(){}

void Town::addRoad(Town* where, int cost)
{
    std::pair<Town*,int> road;
    road = {where, cost};
    roads.push_back(road);
}

