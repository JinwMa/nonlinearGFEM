#include <iostream>
#include <vector>
#include <string>
#include "dof_map.h"
#include "mesh.h"

using namespace std;

void Dof_Map::BuildNodesDofs(Mesh & mesh)
{
    // 暂时为每一个节点开三个自由度
    vector<string> alldofs = {"ux", "uy", "uz"};
    for (int i = 0; i < mesh.actual_node_count; i++)
    {
        int nodeid = mesh.NodeIdList[i];
        NodesDofs[i] = alldofs;
    }
}

void Dof_Map::BuildDofMap(Mesh & mesh)
{
    // 先为数组开辟空间
    NodesIndex.resize(mesh.actual_node_count);
    int index = 0;
    for (int i = 0; i < mesh.actual_node_count; i++)
    {
        int nodeid = mesh.NodeIdList[i];
        int nodeorder = mesh.NodeOrderInList[nodeid];
        NodesIndex[nodeorder] = index;
        int dofsize = NodesDofs[nodeid].size();
        index += dofsize;
    }
}
