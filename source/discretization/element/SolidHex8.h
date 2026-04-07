#ifndef GFEM_SOLIDHEX8ELEMENT_H
#define GFEM_SOLIDHEX8ELEMENT_H

#include "BaseElement.h"

class SolidHex8 : public BaseElement
{
    public:
    SolidHex8();
    ~SolidHex8() = default;

    void createDofsInElement(int element_id, std::vector<int> nodes, std::vector<Dof> & dofs) override;

    void ComputeStiffness(int elementId,
                          ElementInfoPack * pack,
                          Matrix<double> & K) override;

    void ComputeInternalForce(int elementId,
                              ElementInfoPack * pack,
                              std::vector<double> & fint) override;

    // 形状函数相关接口
    void evaluateShapeFunctions(const std::vector<double>& naturalCoords,
                                std::vector<double>& shapeFunctions) const override;

    void evaluateShapeFunctionDerivatives(const std::vector<double>& naturalCoords,
                                          std::vector<std::vector<double>>& derivatives) const override;

    const std::vector<IntegrationPoint>& getIntegrationPoints() const override;

    // 信息查询接口
    ElementDimension getElementDimension() const override;

    std::string getElementTypeName() const override;



    private:
    std::vector<DofTypes::Dof_Tag> d_node_dofTag{DofTypes::Dof_Tag::Ux, DofTypes::Dof_Tag::Uy, DofTypes::Dof_Tag::Uz};
};




#endif