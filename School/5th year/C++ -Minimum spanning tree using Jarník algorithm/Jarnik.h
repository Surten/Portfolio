#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include "ModifiedBFS.h"
#include <unordered_set>

using namespace std;
struct CustomCompare
{
    bool operator()(std::pair<Town*, int> l, std::pair<Town*, int> r)
    {
        return l.second > r.second;
    }
};

void jarnik(int T, int D, vector<Town>& Towns, int& TotalCost, int i, std::priority_queue<pair<Town*, int>, vector<pair<Town*, int>>, CustomCompare>& pq,
std::unordered_set<int>& visited);