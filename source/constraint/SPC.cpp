#include "SPC.h"



void SPC::takeDB()
{
    // get nodes
    double eps = 1.E-8;
    auto nodeDB = d_db->getDataBase("nodes");
    string type = nodeDB->getString("type");
    if (type == "loc")
    {
        double cof[10] = {0.0};
        if (nodeDB->ifExist("C0")) cof[0] = nodeDB->getDouble("C0");
        if (nodeDB->ifExist("x")) cof[1] = nodeDB->getDouble("x");
        if (nodeDB->ifExist("y")) cof[2] = nodeDB->getDouble("y");
        if (nodeDB->ifExist("z")) cof[3] = nodeDB->getDouble("z");
        if (nodeDB->ifExist("xy")) cof[4] = nodeDB->getDouble("xy");
        if (nodeDB->ifExist("yz")) cof[5] = nodeDB->getDouble("yz");
        if (nodeDB->ifExist("xz")) cof[6] = nodeDB->getDouble("xz");
        if (nodeDB->ifExist("x2")) cof[7] = nodeDB->getDouble("x2");
        if (nodeDB->ifExist("y2")) cof[8] = nodeDB->getDouble("y2");
        if (nodeDB->ifExist("z2")) cof[9] = nodeDB->getDouble("z2");      
        int numNode = d_mesh->d_actual_node_count;
        for (int i = 0; i < numNode; i++)
        {
            //内部节点编号
            int nodeId = i;
            double x = d_mesh->d_nodes_coordinate[i][0];
            double y = d_mesh->d_nodes_coordinate[i][1];
            double z = d_mesh->d_nodes_coordinate[i][2];
            if (std::abs(x * cof[1] +
                         y * cof[2] +
                         z * cof[3] +
                         x * y * cof[4] +
                         y * z * cof[5] +
                         x * z * cof[6] +
                         x * x * cof[7] +
                         y * y * cof[8] +
                         z * z * cof[9] -
                         cof[0]) < eps)
                d_nodes.push_back(nodeId);
        }  
    }
    else
    {
        toolbox::error("not support this type of nodes input");
    }

    // get dofs 
    d_dofs = d_db->getVectorString("dofs");
    // get values
    int numDofs = d_dofs.size();
    int numNodes = d_nodes.size();
    d_values.resize(numDofs);
    for (int i = 0; i < numDofs; i++)
    {
        auto dofDB = d_db->getDataBase(d_dofs[i]);
        d_values[i].resize(numNodes);
        for (int j = 0; j < numNodes; j++)
        {
            int nodeId = d_nodes[j];
            double cof[10] = {0.0};
            if (dofDB->ifExist("C0")) cof[0] = dofDB->getDouble("C0");
            if (dofDB->ifExist("x")) cof[1] = dofDB->getDouble("x");
            if (dofDB->ifExist("y")) cof[2] = dofDB->getDouble("y");
            if (dofDB->ifExist("z")) cof[3] = dofDB->getDouble("z");
            if (dofDB->ifExist("xy")) cof[4] = dofDB->getDouble("xy");
            if (dofDB->ifExist("yz")) cof[5] = dofDB->getDouble("yz");
            if (dofDB->ifExist("xz")) cof[6] = dofDB->getDouble("xz");
            if (dofDB->ifExist("x2")) cof[7] = dofDB->getDouble("x2");
            if (dofDB->ifExist("y2")) cof[8] = dofDB->getDouble("y2");
            if (dofDB->ifExist("z2")) cof[9] = dofDB->getDouble("z2"); 
            double x = d_mesh->d_nodes_coordinate[nodeId][0];
            double y = d_mesh->d_nodes_coordinate[nodeId][1];
            double z = d_mesh->d_nodes_coordinate[nodeId][2];
            d_values[i][j] = cof[0] +
                             x * cof[1] +
                             y * cof[2] +
                             z * cof[3] +
                             x * y * cof[4] +
                             y * z * cof[5] +
                             x * z * cof[6] +
                             x * x * cof[7] +
                             y * y * cof[8] +
                             z * z * cof[9];
        }
    }
}
void SPC::buildNodeDofs()
{
    std::cout << "build dofs for " << d_db->d_name << std::endl;
    return;
}

void SPC::buildConstraintEquations(vector<ConstraintEquation> &CEs,
                                   shared_ptr<set<int>> SlaveSet,
                                   shared_ptr<set<int>> MasterSet)
{

}