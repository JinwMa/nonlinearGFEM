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
        buildNodeDofs();
    };
    ~DofMap(){};
    public:
    vector<int>d_dof_map;

    int d_opened_dof_size = 0;

    map<string, int> d_dof_tag_to_int;
    map<int, string> d_dof_int_to_tag;
    map<int, set<int>> d_nodes_dofs;

    vector<int> d_nodes_dof_index;

    public:
    // 根据单元特征开放节点上的自由度
    void buildNodeDofs();
    void buildDofMap();
    int getDofIndex(const int node_internal_id, const string dof_lab);
    // int getDofIndex(const int node_internal_id, const int dof_order);

    void addNodeDof(const int node_internal_id, const string dof_lab);
    void removeNodeDof(const int node_internal_id, const string dof_lab);

    void printNodesDofs();

    // 建立或更新节点自由度的起始值
    void setNodesDofIndex();

    private:
    void initializeDofTag();
    shared_ptr<Mesh> d_mesh;


};

#endif // DOF_MAP1_H