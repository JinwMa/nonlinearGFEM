#ifndef DOF_MAP1_H
#define DOF_MAP1_H
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include "mesh.h"
#include "input.h"
#include "toolbox.h"
using namespace std;
class DofMap
{
public:
    DofMap(shared_ptr<Mesh> mesh)
    {
        d_mesh = mesh;
        initializeDofTag();
    };
    ~DofMap() {};

public:
    vector<int> d_dof_map;

    int d_numAllDofs = 0;
    int d_numMasterDofs = 0;
    int d_numSlaveDofs = 0;
    map<int, set<int>> d_nodes_dofs;
    vector<int> d_nodes_dof_index;
    map<string, int> d_Tag2Int;
    map<int, string> d_Int2Tag;
    vector<int> d_All2MasterSlave;       // 
    vector<int> d_All2Solver;            // 
    vector<int> d_Slave2CEId;            // 
    vector<int> d_DofType;               // 

public:
    // 根据单元特征开放节点上的自由度
    void buildNodeIndex();
    void buildDofMap(){};
    int getDofIndex(const int node_internal_id, const string dof_lab);
    // int getDofIndex(const int node_internal_id, const int dof_order);

    void addNodeDof(const int node_internal_id, const string dof_lab);
    void removeNodeDof(const int node_internal_id, const string dof_lab);
    void printNodesDofs();


     vector<int> transTagsToInts(vector<string> & Tags);

private:
    void initializeDofTag();
    shared_ptr<Mesh> d_mesh;
};

#endif // DOF_MAP1_H