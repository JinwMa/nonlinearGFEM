#include <iostream>
#include <vector>
#include <string>
#include "dof_map.h"
#include "mesh.h"

using namespace std;
void Dof_Map::get_nodes_dofs(const std::vector<int>& vec, const int length)
{
    //默认实体
    vector<string> alldofs = {"ux", "uy", "uz"};
    // 默认给每个节点开三个自由度
    for (int i = 0; i < length; i++)
    {
        nodes_dofs[vec[i]] = alldofs;
    }
}

vector<string> Dof_Map::getNodeDofs(const int node_id)
{
    if(nodes_dofs.find(node_id) == nodes_dofs.end())
    {
        std::cout << "该节点不在映射表中" << std::endl;
        exit(0);
    }
    return nodes_dofs[node_id];
}