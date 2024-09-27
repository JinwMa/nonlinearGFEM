#include"SPC.h"

void SPC::takeDB(Input & input, Mesh & mesh)
{
    auto it = input.db.find(name+"nodes_type");
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
        }
    }
}