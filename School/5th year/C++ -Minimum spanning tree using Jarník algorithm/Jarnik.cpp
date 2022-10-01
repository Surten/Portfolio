#include "Jarnik.h"

using namespace std;


void jarnik(int T, int D, vector<Town>& Towns, int& TotalCost, int i,
std::priority_queue<pair<Town*, int>, vector<pair<Town*, int>>, CustomCompare>& pq,
std::unordered_set<int>& visited){

    while(!pq.empty()) pq.pop();
    visited.clear();
    visited.insert(i+1);
    
    //add all roads leading from the District main town

    for(auto r : Towns.at(i).roads)
    {
        if(r.first->districtID == i+1)
        {
            pq.push(r);
        }
    }
        //priority queue search
    while(visited.size() != Towns.at(i).districtSize)
    {
        auto node = pq.top();
        pq.pop();
        if(visited.find(node.first->id) != visited.end()) continue;
        TotalCost += node.second;
        visited.insert(node.first->id);
        //add roads that are 
        for(auto n : node.first->roads)
        {
            if(n.first->districtID == i+1 && visited.find(n.first->id) == visited.end())
            {
                pq.push(n);
            }
        }
    }
}