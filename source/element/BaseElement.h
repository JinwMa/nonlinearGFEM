#ifndef BASEELEMENT1_H
#define BASEELEMENT1_H

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "input.h"
#include "mesh.h"
#include "toolbox.h"
#include "materialinterface.h"
#include "ObjectElement.h"
#include "ObjectElementData.h"
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
    int d_num_intergration_point;
    BaseElement(){};
    virtual ~BaseElement() {}; //定义纯虚析构函数
    void takeDB(Input * pinput, Mesh * pmesh, std::string & name);    
    virtual void SetElement(){
        toolbox::error("should not use this function in BaseElement: SetElement");
    };;
    virtual std::string ReturnElementType() = 0;

    virtual void ComputeStiffness(double nodes_coordinates[20][3],
                                  std::vector<double> & displacement,
                                  std::vector<double> & du,
                                  std::vector<double> & ddu,
                                  std::vector<double> & elementmat){
        toolbox::error("should not use this function in BaseElement: Computestiffness");
    };

    virtual void ComputeStiffness(double nodes_coordinates[20][3],
                                  std::vector<double> & displacement,
                                  std::vector<double> & du,
                                  std::vector<double> & ddu,
                                  ObjectElement & elementdata,
                                  std::vector<double> & elementmat){
        toolbox::error("should not use this function in BaseElement: Computestiffness");
    };

    virtual void ComputeStiffness(ObjectElementData & element_data,
                                  std::vector<double> & elementmat){};


    virtual void ComputeInternalForce(){};
    virtual void getShapeFunction(double nodes_coordinate[20][3],
                                  std::vector<std::vector<double>> &,
                                  vector<vector<double>> &ShapeFunction,
                                  vector<vector<vector<double>>> &ShapeFunction_dxy,
                                  vector<double> &value_jkb,
                                  const int num_GP) {
        toolbox::error("should not use this function in BaseElement: getShapeFunction");
    };

    virtual void getShapeFunctionOnIntegrationPoint(std::vector<double> & p_coord,
                                                    std::vector<double> & sf) {
        toolbox::error("should not use this function in BaseElement: getShapeFunctionOnIntegrationPoint");
    };
    

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

    protected:
    std::shared_ptr<BaseMaterial> pmaterial;
    
};

#endif