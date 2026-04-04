#ifndef BASEELEMENT1_H
#define BASEELEMENT1_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "Matrix.h"
#include "input.h"
#include "MeshDataAgent.h"
#include "toolbox.h"
#include "DofMap.h"
#include "Entity.h"


using DofTag = DofTypes::Dof_Tag;
using EntityType = Entity::EntityType;

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

class BaseElement
{
    private:
    public:
    BaseElement(){};
    virtual ~BaseElement() {}; 

    virtual void createDofsInElement(int element_id, std::vector<int> nodes, std::vector<Dof> & dofs) = 0;

    virtual void ComputeStiffness(int elementId,
                                  ElementInfoPack * pack,
                                  Matrix<double> & K) = 0;

    virtual void ComputeInternalForce(int elementId,
                                      ElementInfoPack * pack,
                                      std::vector<double> & fint) = 0;



    protected:

        void AXB3663(const double A[3][6], const double B[6][3], double C[3][3]);
        void AXB3666(const double A[3][6], const double B[6][6], double C[3][6]);
        void AXB3883(const double A[3][8], const double B[8][3], double C[3][3]);
        void AXB3338(const double A[3][3], const double B[3][8], double C[3][8]);

    public:
        // 高斯点的参数坐标和权重，每个单元都一样，因此可用openmp并行
        std::vector<std::vector<double>> d_GaussPoints;

    protected:
    
};


#endif
