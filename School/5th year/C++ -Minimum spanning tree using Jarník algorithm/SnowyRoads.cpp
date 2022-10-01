#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include "Jarnik.h"

using namespace std;



std::vector<Town> loadInput(int& T, int& D, int& R, std::vector<int>& plainRoads){

    vector<Town> Towns(T);
    for (int i = 0; i < T; i++)
    {
        if(i<D)
        {
            Towns.at(i).districtID = i+1;
        }
        Towns.at(i).id = i+1;
    }
    //cout << "added towns" << endl;

    int from, to, cost;

    for (size_t i = 0; i < R; i++)
    {
        cin >> from >> to >> cost;
        plainRoads.at(i*3) = from;
        plainRoads.at(i*3+1) = to;
        plainRoads.at(i*3+2) = cost;

        Towns.at(from-1).addRoad(&Towns.at(to-1), cost);
        Towns.at(to-1).addRoad(&Towns.at(from-1), cost);
    }
    //cout << "Added roads to towns" << endl;
    return Towns;
}

void calculateDistricts(vector<Town>& Towns)
{
    for (size_t i = 0; i < Towns.size(); i++)
    {
        Towns.at(Towns.at(i).districtID-1).districtSize++;
    }
}	


int main()
{
    int T, D, R;

    cin >> T >> D >> R;

    //Load and store input

    std::vector<int> plainRoads(3*R);

    auto Towns = loadInput(T, D, R, plainRoads);

    //BFS with all district towns

    bfs(D, Towns);

    calculateDistricts(Towns);

    //Jarník algorithm for each district


    vector<Town> districts(D);

    for (size_t i = 0; i < D; i++)
    {
        districts.at(i).districtID = 1;
        districts.at(i).id = i + 1;
        districts.at(i).districtSize = D;
    }
    

    for (size_t i = 0; i < plainRoads.size(); i+=3)
    {
        if(Towns.at(plainRoads.at(i)-1).districtID != Towns.at(plainRoads.at(i+1)-1).districtID)
        {
            /*
            for (auto r : districts.at(Towns.at(plainRoads.at(i)-1).districtID-1).roads)
            {
                if(r.first->id == districts.at(Towns.at(plainRoads.at(i+1)-1).districtID-1))
            }*/
            //cout << plainRoads.at(i) << " " << plainRoads.at(i+1) << " " << plainRoads.at(i+2) << " " << endl;
            districts.at(Towns.at(plainRoads.at(i)-1).districtID-1).addRoad(&districts.at(Towns.at(plainRoads.at(i+1)-1).districtID-1), plainRoads.at(i+2));
            districts.at(Towns.at(plainRoads.at(i+1)-1).districtID-1).addRoad(&districts.at(Towns.at(plainRoads.at(i)-1).districtID-1), plainRoads.at(i+2));
        }
    }

    int TotalCost = 0;
    std::priority_queue<pair<Town*, int>, vector<pair<Town*, int>>, CustomCompare> pq;
    std::unordered_set<int> visited;

    //for each district
    for (size_t i = 0; i < D; i++)
    {
        jarnik(T, D, Towns, TotalCost, i, pq, visited);
    }
    jarnik(T, D, districts, TotalCost, 0, pq, visited);

    cout << TotalCost << endl;
    
}

