#ifndef DOF_MAP1_H
#define DOF_MAP1_H
#include <vector>
#include <string>
#include <map>
#include "mesh.h"
using namespace std;
class Dof_Map
{
    public:
    map<int, vector<string>> NodesDofs;
    vector<int> NodesIndex;
    Dof_Map(Mesh * pmesh)
    {
        BuildNodesDofs(pmesh);
    }
    void BuildNodesDofs(Mesh * pmesh);
    void BuildDofMap(Mesh * pmesh);

};

#endif // DOF_MAP1_H