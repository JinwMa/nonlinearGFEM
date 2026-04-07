#ifndef BASEELEMENT1_H
#define BASEELEMENT1_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <functional>

#include "Matrix.h"
#include "input.h"
#include "MeshDataAgent.h"
#include "toolbox.h"
#include "DofMap.h"
#include "Entity.h"

class BaseMaterial;

using DofTag = DofTypes::Dof_Tag;
using EntityType = Entity::EntityType;

// ====================================================================
// ElementInfoPack - 向后兼容的结构（保持现有代码）
// ====================================================================
struct ElementInfoPack {
    std::array<double, 60> coord;
    std::array<double, 200> disp;
    std::array<double, 200> disp_inc;
    std::array<double, 600> stress_n;
    std::array<double, 100> epeff_n;

     void Clear() {
        // 使用memcpy可能比fill更快，但需要测试
        std::fill(coord.begin(), coord.end(), 0.0);
        std::fill(disp.begin(), disp.end(), 0.0);
        std::fill(disp_inc.begin(), disp_inc.end(), 0.0);
        std::fill(stress_n.begin(), stress_n.end(), 0.0);
        std::fill(epeff_n.begin(), epeff_n.end(), 0.0);
    }

    // 无动态内存分配，栈上分配，性能更好
};

// ====================================================================
// ElementData - 新的通用元素数据结构
// ====================================================================
struct ElementData {
    // 节点坐标 (nNodes × nDim)
    std::vector<double> coordinates;

    // 节点位移 (nNodes × nDOFPerNode)
    std::vector<double> displacements;

    // 节点位移增量 (nNodes × nDOFPerNode)
    std::vector<double> displacementIncrements;

    // 应力历史 (nIntPoints × nStressComponents)
    std::vector<double> stresses;

    // 状态变量 (nIntPoints × nStateVariables)
    std::vector<double> stateVariables;

    // 清除所有数据
    void clear() {
        coordinates.clear();
        displacements.clear();
        displacementIncrements.clear();
        stresses.clear();
        stateVariables.clear();
    }

    // 重置大小为指定值
    void resize(int nNodes, int nDim, int nDOFPerNode, int nIntPoints,
                int nStressComponents, int nStateVars) {
        coordinates.resize(nNodes * nDim);
        displacements.resize(nNodes * nDOFPerNode);
        displacementIncrements.resize(nNodes * nDOFPerNode);
        stresses.resize(nIntPoints * nStressComponents);
        stateVariables.resize(nIntPoints * nStateVars);
    }
};

// ====================================================================
// IntegrationPoint - 积分点数据
// ====================================================================
struct IntegrationPoint {
    std::vector<double> naturalCoords;  // 自然坐标
    double weight;                      // 权重
    double jacobian;                    // Jacobian行列式

    IntegrationPoint(int dim = 3) : naturalCoords(dim, 0.0), weight(0.0), jacobian(0.0) {}
};

// ====================================================================
// ElementDimension - 元素维度信息
// ====================================================================
struct ElementDimension {
    int spatialDimension;      // 空间维度 (2 or 3)
    int numberOfNodes;         // 节点数
    int dofPerNode;           // 每个节点的自由度
    int numberOfIntegrationPoints; // 积分点数
    int stressComponents;      // 应力分量数 (3 for 2D, 6 for 3D)

    ElementDimension(int dim = 3, int nodes = 8, int dofPerNode = 3,
                     int intPoints = 8, int stressComps = 6)
        : spatialDimension(dim), numberOfNodes(nodes), dofPerNode(dofPerNode),
          numberOfIntegrationPoints(intPoints), stressComponents(stressComps) {}

    int totalDOF() const { return numberOfNodes * dofPerNode; }
    int coordinateSize() const { return numberOfNodes * spatialDimension; }
};

// ====================================================================
// BaseElement - 单元基类
// ====================================================================
class BaseElement
{
public:
    // 构造函数和析构函数
    BaseElement();
    virtual ~BaseElement() = default;

    // ====================================================================
    // 必需实现的纯虚函数（现有接口，保持兼容）
    // ====================================================================
    virtual void createDofsInElement(int elementId, std::vector<int> nodes,
                                     std::vector<Dof>& dofs) = 0;

    virtual void ComputeStiffness(int elementId,
                                  ElementInfoPack* pack,
                                  Matrix<double>& K) = 0;

    virtual void ComputeInternalForce(int elementId,
                                      ElementInfoPack* pack,
                                      std::vector<double>& fint) = 0;

    // ====================================================================
    // 新接口（可选实现，提供默认实现）
    // ====================================================================

    // 1. 质量矩阵计算
    virtual void computeMassMatrix(int elementId,
                                   const ElementData& data,
                                   Matrix<double>& massMatrix) {
        // 默认实现：返回零矩阵
        int nDOF = getElementDimension().totalDOF();
        massMatrix = Matrix<double>(nDOF, nDOF);
        for (int i = 0; i < nDOF; i++) {
            for (int j = 0; j < nDOF; j++) {
                massMatrix(i, j) = 0.0;
            }
        }
    }

    // 2. 阻尼矩阵计算
    virtual void computeDampingMatrix(int elementId,
                                      const ElementData& data,
                                      Matrix<double>& dampingMatrix) {
        // 默认实现：返回零矩阵
        int nDOF = getElementDimension().totalDOF();
        dampingMatrix = Matrix<double>(nDOF, nDOF);
        for (int i = 0; i < nDOF; i++) {
            for (int j = 0; j < nDOF; j++) {
                dampingMatrix(i, j) = 0.0;
            }
        }
    }

    // 3. 等效节点荷载（体积力）
    virtual void computeBodyForceVector(int elementId,
                                        const ElementData& data,
                                        const std::vector<double>& bodyForce,
                                        std::vector<double>& forceVector) {
        // 默认实现：返回零向量
        int nDOF = getElementDimension().totalDOF();
        forceVector.resize(nDOF, 0.0);
    }

    // 4. 表面荷载
    virtual void computeSurfaceForceVector(int elementId,
                                           const ElementData& data,
                                           int surfaceId,
                                           const std::vector<double>& surfaceTraction,
                                           std::vector<double>& forceVector) {
        // 默认实现：返回零向量
        int nDOF = getElementDimension().totalDOF();
        forceVector.resize(nDOF, 0.0);
    }

    // 5. 应力恢复
    virtual void recoverStresses(int elementId,
                                 const ElementData& data,
                                 std::vector<double>& stressesAtNodes) {
        // 默认实现：返回零应力
        int nNodes = getElementDimension().numberOfNodes;
        int stressComps = getElementDimension().stressComponents;
        stressesAtNodes.resize(nNodes * stressComps, 0.0);
    }

    // 6. 应变计算
    virtual void computeStrains(int elementId,
                                const ElementData& data,
                                std::vector<double>& strainsAtIntPoints) {
        // 默认实现：返回零应变
        int nIntPoints = getElementDimension().numberOfIntegrationPoints;
        int strainComps = getElementDimension().stressComponents; // 应变分量数与应力相同
        strainsAtIntPoints.resize(nIntPoints * strainComps, 0.0);
    }

    // 7. 单元体积计算
    virtual double computeVolume(int elementId,
                                 const ElementData& data) {
        // 默认实现：返回0.0
        return 0.0;
    }

    // 8. 检查单元质量（Jacobian正定性等）
    virtual bool checkElementQuality(int elementId,
                                     const ElementData& data,
                                     double& qualityMeasure) {
        // 默认实现：假设单元质量良好
        qualityMeasure = 1.0;
        return true;
    }

    // ====================================================================
    // 形状函数相关接口
    // ====================================================================

    // 形状函数值
    virtual void evaluateShapeFunctions(const std::vector<double>& naturalCoords,
                                        std::vector<double>& shapeFunctions) const = 0;

    // 形状函数导数
    virtual void evaluateShapeFunctionDerivatives(const std::vector<double>& naturalCoords,
                                                  std::vector<std::vector<double>>& derivatives) const = 0;

    // 获取积分点
    virtual const std::vector<IntegrationPoint>& getIntegrationPoints() const = 0;

    // ====================================================================
    // 信息查询接口
    // ====================================================================

    // 获取单元维度信息
    virtual ElementDimension getElementDimension() const = 0;

    // 获取单元类型名称
    virtual std::string getElementTypeName() const = 0;

    // 检查单元是否支持特定分析类型
    virtual bool supportsAnalysisType(const std::string& analysisType) const {
        // 默认支持静力分析
        return analysisType == "static";
    }

    // ====================================================================
    // 材料管理
    // ====================================================================

    void setMaterial(std::shared_ptr<BaseMaterial> material) { d_material = material; }
    std::shared_ptr<BaseMaterial> getMaterial() const { return d_material; }
    bool hasMaterial() const { return d_material != nullptr; }

    // ====================================================================
    // 辅助矩阵运算函数（保持现有）
    // ====================================================================
protected:
    void AXB3663(const double A[3][6], const double B[6][3], double C[3][3]);
    void AXB3666(const double A[3][6], const double B[6][6], double C[3][6]);
    void AXB3883(const double A[3][8], const double B[8][3], double C[3][3]);
    void AXB3338(const double A[3][3], const double B[3][8], double C[3][8]);

    // ====================================================================
    // 成员变量
    // ====================================================================
protected:
    std::shared_ptr<BaseMaterial> d_material;

    // 高斯点（由具体单元实现管理）
    std::vector<IntegrationPoint> d_integrationPoints;

    // 节点自由度标签
    std::vector<DofTag> d_nodeDOFTags;
};


#endif
