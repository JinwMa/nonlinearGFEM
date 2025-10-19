#ifndef GFEM_SOLIDHEX8ELEMENT_H
#define GFEM_SOLIDHEX8ELEMENT_H

#include "BaseElement.h"

class SolidHex8 : public BaseElement
{
    public:
    SolidHex8() = default;
    ~SolidHex8() = default;

    void createDofsInElement(int element_id, std::vector<int> nodes, std::vector<Dof> & dofs) override;

    void ComputeStiffness(int elementId,
                          ElementInfoPack * pack,
                          Matrix<double> & K) override;

    void ComputeInternalForce(int elementId,
                              ElementInfoPack * pack,
                              std::vector<double> & fint) override;



    private:
    std::vector<DofTypes::Dof_Tag> d_node_dofTag{DofTypes::Dof_Tag::Ux, DofTypes::Dof_Tag::Uy, DofTypes::Dof_Tag::Uz};
};




#endif