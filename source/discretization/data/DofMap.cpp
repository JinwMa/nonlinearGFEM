#include "DofMap.h"

using namespace std;

void DofMap::initializeDofTag()
{
    d_Tag2Int["ux"] = 0;
    d_Tag2Int["uy"] = 1;
    d_Tag2Int["uz"] = 2;
    d_Tag2Int["rx"] = 3;
    d_Tag2Int["ry"] = 4;
    d_Tag2Int["rz"] = 5;

    for (auto it : d_Tag2Int)
    {
        d_Int2Tag[it.second] = it.first;
    }
}



void DofMap::printNodesDofs()
{
    for (auto it : d_nodes_dofs)
    {
        std::cout << "node id " << d_mesh->getNodeExternalId(it.first) << " :" << std::endl;
        for (auto it2 : it.second)
        {
            std::cout << d_Int2Tag[it2] << " ";
        }
        std::cout << std::endl;
    }
}

void DofMap::addNodeDof(const int node_internal_id, const std::string dof_lab)
{
    //首先判断dof_lab是否存在
    if (d_Tag2Int.find(dof_lab) == d_Tag2Int.end())
    {
        toolbox::error("试图添加一个不存在的自由度标签");
        return;
    }
    d_nodes_dofs[node_internal_id].insert(d_Tag2Int[dof_lab]);
}

void DofMap::removeNodeDof(const int node_internal_id, const std::string dof_lab)
{
    //首先判断dof_lab是否存在
    if (d_Tag2Int.find(dof_lab) == d_Tag2Int.end())
    {
        toolbox::error("试图添加一个不存在的自由度标签");
        return;
    }
    d_nodes_dofs[node_internal_id].erase(d_Tag2Int[dof_lab]);    
}



void DofMap::buildNodeIndex()
{
    // 节点数
    int nodes_num = d_mesh->d_actual_node_count;
    d_nodes_dof_index.resize(nodes_num + 1);
    d_nodes_dof_index[0] = 0;
    for(int i = 0; i < nodes_num; i++)
    {
        int node_dof_size = d_nodes_dofs[i].size();
        if (node_dof_size == 0) 
        {
            std::cout << i << std::endl;
            toolbox::error("存在节点自由度数为0的节点");
        }
        d_nodes_dof_index[i + 1] = d_nodes_dof_index[i] + node_dof_size;
    }
    d_numAllDofs = d_nodes_dof_index[nodes_num];
    std::cout << "number of all dofs is " << d_numAllDofs << std::endl;
}

int DofMap::getDofIndex(const int node_internal_id, const std::string dof_lab)
{
    //首先判断dof_lab是否存在
    if (d_Tag2Int.find(dof_lab) == d_Tag2Int.end())
    {
        toolbox::error("试图查询一个不存在的自由度标签");
        return -1;
    }

    // 再次判断节点是否在查询范围
    if (node_internal_id < 0 || node_internal_id > d_mesh->d_actual_node_count) 
    {
        toolbox::error("node is not in the dof map range");
        return -1;
    }
    int dof_int = d_Tag2Int[dof_lab];
    auto node_dofs = d_nodes_dofs[node_internal_id];    
    auto it = node_dofs.find(dof_int);
    int dof_order = 0;
    if (it != node_dofs.end())
    {
        dof_order = std::distance(node_dofs.begin(), it);
    }
    else
    {
        toolbox::error("试图查询的自由度不在该节点的自由度集合中");
        return -1;
    }

    return d_nodes_dof_index[node_internal_id] + dof_order;
}



std::vector<int> DofMap::transTagsToInts(std::vector<std::string> & Tags)
{
    std::vector<int> Ints;
    for (auto it : Tags)
    {
        if(d_Tag2Int.find(it) == d_Tag2Int.end())
        {
            toolbox::error("failed to trans the tag to int");
        }
        Ints.push_back(d_Tag2Int[it]);        
    }
    return Ints;
}