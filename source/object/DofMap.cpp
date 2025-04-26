#include "DofMap.h"

using namespace std;

void DofMap::initializeDofTag()
{
    d_dof_tag_to_int["ux"] = 0;
    d_dof_tag_to_int["uy"] = 1;
    d_dof_tag_to_int["uz"] = 2;
    d_dof_tag_to_int["rx"] = 3;
    d_dof_tag_to_int["ry"] = 4;
    d_dof_tag_to_int["rz"] = 5;

    for (auto it : d_dof_tag_to_int)
    {
        d_dof_int_to_tag[it.second] = it.first;
    }
}



void DofMap::printNodesDofs()
{
    for (auto it : d_nodes_dofs)
    {
        std::cout << "node id " << d_mesh->getNodeExternalId(it.first) << " :" << std::endl;
        for (auto it2 : it.second)
        {
            std::cout << d_dof_int_to_tag[it2] << " ";
        }
        std::cout << std::endl;
    }
}

void DofMap::addNodeDof(const int node_internal_id, const std::string dof_lab)
{
    //首先判断dof_lab是否存在
    if (d_dof_tag_to_int.find(dof_lab) == d_dof_tag_to_int.end())
    {
        toolbox::error("试图添加一个不存在的自由度标签");
        return;
    }
    d_nodes_dofs[node_internal_id].insert(d_dof_tag_to_int[dof_lab]);
}

void DofMap::removeNodeDof(const int node_internal_id, const std::string dof_lab)
{
    //首先判断dof_lab是否存在
    if (d_dof_tag_to_int.find(dof_lab) == d_dof_tag_to_int.end())
    {
        toolbox::error("试图添加一个不存在的自由度标签");
        return;
    }
    d_nodes_dofs[node_internal_id].erase(d_dof_tag_to_int[dof_lab]);    
}



void DofMap::setNodesDofIndex()
{
    // 节点数
    int nodes_num = d_mesh->d_actual_node_count;
    d_nodes_dof_index.resize(nodes_num + 1);
    d_nodes_dof_index[0] = 0;
    for(int i = 0; i < nodes_num; i++)
    {
        int node_dof_size = d_nodes_dofs[i].size();
        if (node_dof_size == 0) toolbox::error("存在节点自由度数为0的节点");
        d_nodes_dof_index[i + 1] = d_nodes_dof_index[i] + node_dof_size;
    }
}

int DofMap::getDofIndex(const int node_internal_id, const std::string dof_lab)
{
    //首先判断dof_lab是否存在
    if (d_dof_tag_to_int.find(dof_lab) == d_dof_tag_to_int.end())
    {
        toolbox::error("试图查询一个不存在的自由度标签");
        return -1;
    }
    int dof_int = d_dof_tag_to_int[dof_lab];
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



std::vector<int> DofMap::transTagsToInt(std::vector<std::string> & Tags)
{
    std::vector<int> Ints;
    for (auto it : Tags)
    {
        if(d_dof_tag_to_int.find(it) == d_dof_tag_to_int.end())
        {
            toolbox::error("failed to trans the tag to int");
        }
        Ints.push_back(d_dof_tag_to_int[it]);        
    }
    return Ints;
}