#include "ModifiedBFS.h"

void bfs(int &D, std::vector<Town>& Towns)
{
    std::queue<Town> q;
    for (size_t i = 0; i < D; i++)
    {
        q.push(Towns.at(i));
    }
    
    while(!q.empty())
    {
        Town node = q.front();
        q.pop();
        
        for (auto t : node.roads)
        {
            if(t.first->districtID == 0){
                t.first->districtID = node.districtID;
                q.push(*t.first);
            }
        }
    }
}