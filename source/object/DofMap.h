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
    DofMap(){};
    ~DofMap(){};
    public:
    vector<int>d_dof_map;
    vector<int>d_nodes_dofs;

    int d_opened_dof_size = 0;


    private:
    int d_max_size_dof = 6;


    public:
    void buildNodeDofs(shared_ptr<Mesh> mesh);
    void buildDofMap();
    void takeDB(shared_ptr<DataBase> db);
    int getDofIndex(const int node_local_id, const string dof_lab);
    int getDofIndex(const int node_local_id, const int dof_order);

    void addNodeDof(const int node_local_id, const string dof);


};

#endif // DOF_MAP1_H