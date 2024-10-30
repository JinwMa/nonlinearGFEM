#include "NodalForce.h"

void NodalForce::takeDB(Input * pinput, Mesh * pmesh, std::string name)
{
    // if (name.empty()) toolbox::error("name of  should not be empty");
    load_vector = pinput->getVectorDouble(name + "_vector");
    load_value = pinput->getDouble(name + "_value");
    if (pinput->ifExist(name + "_node_set_ids"))
    {
        node_set_ids = pinput->getVectorInt(name + "_node_set_ids");
    }
    else if (pinput->ifExist(name + "_node_ids"))
    {
        node_ids = pinput->getVectorInt(name + "_node_ids");
    }
    else
    {
        toolbox::error("at least one type nodes info should given");
    }
    if (node_set_ids.size() != 0)
    {
        for (int i = 0; i < node_set_ids.size(); i++)
        {
            node_ids.insert(node_ids.end(), 
                            pmesh->node_sets[node_set_ids[i]].begin(),
                            pmesh->node_sets[node_set_ids[i]].end());
        }
    }
}

void NodalForce::buildLoadVector(Input * pinput,
                         Mesh * pmesh,
                         Dof_Map * pdofmap,
                         std::vector<int> & dofs_index,
                         std::vector<double> & loadvector)
{
    dofs_index.clear();
    load_vector.clear();
    dofs_index.resize(node_ids.size() * NDIM);
    loadvector.resize(node_ids.size() * NDIM);
    int count = 0;
    for (int i = 0; i < node_ids.size(); i++)
    {
        int node_id = node_ids[i];
        int dof_x = pdofmap->getDofIndex(node_id, "ux");
        int dof_y = pdofmap->getDofIndex(node_id, "uy");
        int dof_z = pdofmap->getDofIndex(node_id, "uz");
        dofs_index[count * 3 + 0] = dof_x;
        dofs_index[count * 3 + 1] = dof_y;
        dofs_index[count * 3 + 2] = dof_z;
        loadvector[count * 3 + 0] = load_value * load_vector[0];
        loadvector[count * 3 + 1] = load_value * load_vector[1];
        loadvector[count * 3 + 2] = load_value * load_vector[2];
        count++;
    }
}