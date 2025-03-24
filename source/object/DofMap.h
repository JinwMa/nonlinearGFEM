#ifndef DOF_MAP1_H
#define DOF_MAP1_H
#include <vector>
#include <string>
#include <unordered_map>
#include "mesh.h"
#include "input.h"
#include "toolbox.h"
using namespace std;
class DofMap
{
    public:
    DofMap();
    ~DofMap(){};
    public:
    vector<int>d_dof_map;
    unordered_map<int, vector<string>> d_nodes_dofs;


    private:
    int d_max_size_dof = 6;


    public:
    void buildNodeDofs(shared_ptr<Mesh> mesh);
    void takeDB(shared_ptr<DataBase> db);
    int getDofIndex(const int node_local_id, const string dof_lab);
    int getDofIndex(const int node_local_id, const int dof_order);


};

#endif // DOF_MAP1_H