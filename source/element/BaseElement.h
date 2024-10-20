#ifndef BASEELEMENT1_H
#define BASEELEMENT1_H

#include <iostream>
#include <vector>
#include <string>
#include "input.h"
#include "mesh.h"
#include "toolbox.h"
class BaseElement
{
    private:

    public:
    int dim;
    int numNodeDof;
    int numNodes;
    std::vector<std::string> dofs;

    public:
    int integration_order = 3;
    int num_intergration_point;
    BaseElement(){};
    virtual ~BaseElement() {}; //定义纯虚析构函数
    void takeDB(Input * pinput, Mesh * pmesh, std::string & name);    
    virtual void SetElement() = 0;

    virtual void ComputeStiffness(double nodes_coordinates[20][3],
                                  std::vector<double> & elementmat) = 0;


    virtual void ComputeInternalForce(){};
    virtual void getShapeFunction(){};
    

    protected:
    void AmnXBpq(const double * A, const int m, const int n,
                 const double * B, const int p, const int q,
                 double * C);
    void AXB3663(const double A[3][6], const double B[6][3], double C[3][3]);
    void AXB3666(const double A[3][6], const double B[6][6], double C[3][6]);
    void AXB3883(const double A[3][8], const double B[8][3], double C[3][3]);
    double invertMatrix(const double input[3][3], double inverse[3][3]);
    void AXB3338(const double A[3][3], const double B[3][8], double C[3][8]);


    public:
    std::vector<int> element_ids;
    std::vector<std::vector<double>> d_GaussPoints;
    
};

#endif