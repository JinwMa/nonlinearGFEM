#include "DofMap.h"

using namespace std;
void DofMap::takeDB(shared_ptr<DataBase> db)
{
    if (db->d_name != "global_control") toolbox::error("the db is wrong in DofMap");
    if (db->ifExist("max_size_dof")) d_max_size_dof = db->getInt("max_size_dof");
}


void DofMap::buildNodeDofs(shared_ptr<Mesh> mesh)
{
    int num_nodes = mesh->d_actual_node_count;
    int dof_size = num_nodes * d_max_size_dof;
    int num_element = mesh->d_actual_element_count;
    d_nodes_dofs.resize(dof_size);
    for (int i = 0; i < dof_size; i++)
    {
        d_nodes_dofs[i] = -1;
    }
    if (d_max_size_dof < 3)
    {
        toolbox::error("not support 2D cases now");
    }
    else if (d_max_size_dof == 3)
    {
        for (int i = 0; i < dof_size; i++)
        {
            d_nodes_dofs[i] = 1;
        }
    }
    else if (d_max_size_dof == 6)
    {
        for (int ielement = 0; ielement < num_element; ielement++)
        {
            int element_type = mesh->d_element_type[ielement];
            if (element_type == 1 || element_type == 2)
            {
                for (auto node_global_id : mesh->d_element_connectivity[ielement])
                {
                    int node_local_id = mesh->getNodeLocalId(node_global_id);
                    int dof_start = node_local_id * d_max_size_dof;
                    for (int i = 0; i < 3; i++)
                    {
                        d_nodes_dofs[dof_start + i] = 1;
                    }
                }
            }
            else
            {
                toolbox::error("not support this type of element now");
            }
        }
    }
    else
    {
        toolbox::error("only support structual analysis now");
    }

}


void DofMap::buildDofMap()
{
    int dof_size = d_nodes_dofs.size();
    d_dof_map.resize(dof_size);
    int dof_index = 0;
    for (int i = 0; i < dof_size; i++)
    {
        if (d_nodes_dofs[i] == 1)
        {
            dof_index++;
            d_dof_map[i] = dof_index - 1;
        }
    }
    d_opened_dof_size = dof_index;
}


int DofMap::getDofIndex(const int node_local_id, const string dof)
{
    int dof_order = 0;
    if (dof == "ux") dof_order = 0;
    else if (dof == "uy") dof_order = 1;
    else if (dof == "uz") dof_order = 2;
    else if (dof == "rx") dof_order = 3;
    else if (dof == "ry") dof_order = 4;
    else if (dof == "rz") dof_order = 5;
    else
    {
        toolbox::error("not support this labal of dof");
    }

    return d_dof_map[node_local_id * d_max_size_dof + dof_order];
}

int DofMap::getDofIndex(const int node_local_id, const int dof_order)
{
    if (dof_order >= d_max_size_dof) toolbox::error("dof size is too large to support now");
    return d_dof_map[node_local_id * d_max_size_dof + dof_order];
}


void DofMap::addNodeDof(const int node_local_id, const string dof)
{
    int dof_order = 0;
    if (dof == "ux") dof_order = 0;
    else if (dof == "uy") dof_order = 1;
    else if (dof == "uz") dof_order = 2;
    else if (dof == "rx") dof_order = 3;
    else if (dof == "ry") dof_order = 4;
    else if (dof == "rz") dof_order = 5;
    else
    {
        toolbox::error("not support this labal of dof");
    }
    d_nodes_dofs[node_local_id * d_max_size_dof + dof_order] = 1;
}