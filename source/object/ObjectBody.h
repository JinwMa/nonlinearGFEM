#ifndef OBJECTBODY
#define OBJECTBODY

#include<vector>
#include<set>

struct ObjectBody
{
    std::set<int> Element_ids;
    std::set<std::vector<int>> outer_faces;
    std::set<int> outer_nodes;    
};





#endif