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
    map<int, set<int>> d_nodes_dofs;     // 节点上的自由度
    vector<int> d_nodes_dof_index;       // 每个节点第一个自由度在整体自由度列表中的位置
    map<string, int> d_Tag2Int;          // 自由度标签对应的整型数字
    map<int, string> d_Int2Tag;          // 整型数字对应的自由度标签
    vector<int> d_All2MasterSlave;       // 暂时未用到
    vector<int> d_All2Solver;            // 整体自由度跟求解器中自由度的对应关系
    vector<int> d_Slave2CEId;            // 暂时未用到
    vector<int> d_DofType;               // 0 master -1 slave 

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