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
    int node_num = mesh.actual_node_count;
    for (int i = 0; i < node_num; i++)
    {
        int node_id = mesh.NodeIdList[i];
        int node_order = mesh.NodeOrderInList[node_id];
        double x = mesh.NodesCoordinate[node_order][0]; 
        double y = mesh.NodesCoordinate[node_order][1]; 
        double z = mesh.NodesCoordinate[node_order][2];  
        std::cout << x << " "<< y << " "<< z << " " <<std::endl;
        std::cout << "aaaaaaa" << std::endl;
    }
    std::cout << "sssssss" << std::endl;
}