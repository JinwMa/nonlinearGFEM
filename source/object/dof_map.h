#ifndef DOF_MAP1_H
#define DOF_MAP1_H
#include <vector>
#include <string>
#include <unordered_map>
#include "mesh.h"
#include "toolbox.h"
using namespace std;
class Dof_Map
{

    public:
    const int reserve_size = 1000;
    unordered_map<int, vector<string>> NodesDofs;
    vector<int> NodesIndex;
    vector<int> NodesDofIndex;  // get dof_index by node order order: (node_order - 1) * 6 + dof_index
    unordered_map<int, int> dofmap; // get dof_index by node id
    int max_dof_index = 0;
    Dof_Map(Mesh * pmesh)
    {
        BuildNodesDofs(pmesh);
    }
    void BuildNodesDofs(Mesh * pmesh);
    void BuildDofMap(Mesh * pmesh);

    int getDofIndex(const int & node_id, const string & dof)
    {
        int dof_index = 0;
        if (dof == "ux") dof_index = 0;
        else if (dof == "uy") dof_index = 1;
        else if (dof == "uz") dof_index = 2;
        else if (dof == "rx") dof_index = 3;
        else if (dof == "ry") dof_index = 4;
        else if (dof == "rz") dof_index = 5;
        else toolbox::error("wrong input of dof in getDofIndex");
        return dofmap[(node_id - 1) * 6 + dof_index];
    };

    void addNodeDof(const int node_id, const string dof){
        std::cout << "add some code here" << std::endl;
    };
    void removeNodeDof(const int node_id, const string dof){
        std::cout << "add some code here" << std::endl;
    };


};

#endif // DOF_MAP1_H