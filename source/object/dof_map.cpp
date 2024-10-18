#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "dof_map.h"
#include "mesh.h"
#include "toolbox.h"

using namespace std;

void Dof_Map::BuildNodesDofs(Mesh * pmesh)
{
    // 暂时为每一个节点开三个自由度
    vector<string> alldofs = {"ux", "uy", "uz"};
    for (int i = 0; i < pmesh->actual_node_count; i++)
    {
        int nodeid = pmesh->NodeIdList[i];
        NodesDofs[nodeid] = alldofs;
    }
}

void Dof_Map::BuildDofMap(Mesh * pmesh)
{
    // 先为数组开辟空间
    // NodesIndex.resize(pmesh->actual_node_count);
    // int index = 0;
    // for (int i = 0; i < pmesh->actual_node_count; i++)
    // {
    //     int nodeid = pmesh->NodeIdList[i];
    //     int nodeorder = pmesh->NodeOrderInList[nodeid];
    //     NodesIndex[nodeorder - 1] = index;
    //     int dofsize = NodesDofs[nodeid].size();
    //     index += dofsize;
    // }

    NodesDofIndex.resize(pmesh->actual_node_count * 6 + reserve_size);
    std::fill(NodesDofIndex.begin(), NodesDofIndex.end(), -1);
    for (int i = 0; i < pmesh->actual_node_count; i++)
    {
        int node_id = pmesh->NodeIdList[i];
        int node_order = pmesh->NodeOrderInList[node_id];
        auto dofs = NodesDofs[node_id];
        for (auto dof : dofs)
        {
            int index = 0;
            if (dof == "ux") index = (node_order - 1) * 6;
            else if (dof == "uy") index = (node_order - 1) * 6 + 1;
            else if (dof == "uz") index = (node_order - 1) * 6 + 2;
            else if (dof == "rx") index = (node_order - 1) * 6 + 3;
            else if (dof == "ry") index = (node_order - 1) * 6 + 4;
            else if (dof == "rz") index = (node_order - 1) * 6 + 5; 
            else
               toolbox::error("error in buildDoFMap: an error input of dof");           
            NodesDofIndex[index] = 1;
            // std::cout << node_id << " " << node_order << " " << index << std::endl;
        }
    }

    for (int i = 0; i < NodesDofIndex.size(); i++)
    {
        int index = NodesDofIndex[i];
        if (index == 1) 
        {
            max_dof_index++;
            NodesDofIndex[i] = max_dof_index - 1;
        }
    }
    for (int i = 0; i < pmesh->NodeIdList.size(); i++)
    {
        int node_id = pmesh->NodeIdList[i];
        int node_order = pmesh->NodeOrderInList[node_id];
        for (int j = 0; j < 6; j++)
        {
            dofmap[(node_id - 1) * 6 + j] = NodesDofIndex[(node_order - 1) * 6 + j];
        }        
    }

}
