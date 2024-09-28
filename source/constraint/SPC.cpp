#include"SPC.h"
#include"toolbox.h"

void SPC::takeDB(Input & input, Mesh & mesh)
{
    auto it = input.db.find(name+"_nodes_type");
    if (it != input.db.end())
    {
        if (it->second.size() != 1) 
        {
            std::cout << "the input of " << it->first << " is wrong" << std::endl;
            exit(1);
        }
        if (it->second[0] == "geometry")
        {
            // d_node_ids = this->getNodesFromGeometry();
            d_geometry_expression = input.getVectorDouble(name + "_geometry_expression");
            getNodesFromGeometry(mesh);
        }
    }
    else
    {
        toolbox::error("not found " + name + "nodes_type");
    }
}

void SPC::getNodesFromGeometry(Mesh & mesh)
{
    // d_node_ids
    const double eps = 1.E-10;
    int count = d_geometry_expression.size();
    if (count == 0) return;
    if (count > 10) toolbox::error("the size of d_geometry_expression is wrong");
    double expression[10] = {0.0};
    for (int i = 0; i < count; i++)expression[i] = d_geometry_expression[i];

    int node_num = mesh.actual_node_count;
    for (int i = 0; i < node_num; i++)
    {
        int node_id = mesh.NodeIdList[i];
        int node_order = mesh.NodeOrderInList[node_id] - 1;        
        double x = mesh.NodesCoordinate[node_order][0]; 
        double y = mesh.NodesCoordinate[node_order][1]; 
        double z = 0.0;
        if (NDIM == 3) z = mesh.NodesCoordinate[node_order][2];
        double value = 1E10;
        if (NDIM != 3) toolbox::error("2D or 1D cases are not supported");
        else
        {
            value = expression[0] + expression[1] * x + expression[2] * y + expression[3] * z +
                    expression[4] * x * x + expression[5] * y * y + expression[6] * z * z + 
                    expression[7] * x * y + expression[8] * y * z + expression[9] * x * z;
        }
        if (std::fabs(value) < eps) d_node_ids.push_back(node_id);
    }
    std::cout << d_node_ids.size() << std::endl;
    for (size_t i = 0; i < d_node_ids.size(); i++){}
        // std::cout << d_node_ids[i] << std::endl;
}