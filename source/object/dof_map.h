#ifndef DOF_MAP1_H
#define DOF_MAP1_H
#include <vector>
#include <string>
#include <unordered_map>
#include "mesh.h"
using namespace std;
class Dof_Map
{

    public:
    const int reserve_size = 1000;
    unordered_map<int, vector<string>> NodesDofs;
    vector<int> NodesIndex;
    vector<int> NodesDofIndex;
    Dof_Map(Mesh * pmesh)
    {
        BuildNodesDofs(pmesh);
    }
    void BuildNodesDofs(Mesh * pmesh);
    void BuildDofMap(Mesh * pmesh);


};

#endif // DOF_MAP1_H