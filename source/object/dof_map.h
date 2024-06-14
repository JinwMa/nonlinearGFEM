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
    Dof_Map(const std::vector<int>& vec, const int length)
    {
        get_nodes_dofs(vec, length);
    }
    vector<string> getNodeDofs(const int node_id);
public:
    map<int, vector<string>> nodes_dofs; // 每个节点所包含的自由度
    map<int, int> nodes_dof_index;       // 每个节点自由度的起始位置
private:
    void get_nodes_dofs(const std::vector<int>& vec, const int length);
};

#endif // DOF_MAP1_H