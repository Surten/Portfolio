#include <iostream>
#include <vector>

class Town
{
    public:
    std::vector<std::pair<Town*,int>> roads;
    int id = 0;
    int districtID = 0;
    int districtSize = 0;

    Town();
    ~Town();

    void addRoad(Town* where, int cost);

};


