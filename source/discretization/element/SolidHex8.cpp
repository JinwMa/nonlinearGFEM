#include "SolidHex8.h"

void SolidHex8::createDofsInElement(int element_id, std::vector<int> nodes, std::vector<Dof> & dofs)
{
    // std::cout << element_id << std::endl;
    dofs.clear();
    int nodeSize = nodes.size();
    int dofSize = d_node_dofTag.size();
    dofs.resize(nodeSize * dofSize);
    for (int i = 0; i < nodeSize; i++)
    {
        int node_id = nodes[i];
        for (int j = 0; j < dofSize; j++)
        {
            auto dofTag = d_node_dofTag[j];
            dofs[i * dofSize + j].dofTag = dofTag;
            dofs[i * dofSize + j].entity_type = Entity::EntityType::NODE;
            dofs[i * dofSize + j].entity_id = node_id;
        }
    } 
}



void SolidHex8::ComputeStiffness(int elementId,
                                 ElementInfoPack * pack,
                                 Matrix<double> & K) 
{

}


 void SolidHex8::ComputeInternalForce(int elementId,
                                      ElementInfoPack * pack,
                                      std::vector<double> & fint) 
{

}