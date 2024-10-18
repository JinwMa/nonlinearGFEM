#ifndef LINEARHEX81_H
#define LINEARHEX81_H

#include "BaseElement.h"


using namespace std;

class LinearHex8 : public BaseElement
{
    static const int num_edofs = 24;
    static const int num_nodes = 8;
    static const int num_node_dof = 3;
    static const int integration_order = 3;
    int num_intergration_point = integration_order * integration_order * integration_order;
    public:
    void ComputeStiffness(double nodes_coordinates[8][3],
                          std::vector<std::vector<double>> &GaussPoints,
                          double elementmat[num_edofs][num_edofs]);
    void ComputeInternalForce(){};
    void SetGaussIntegration(std::vector<std::vector<double>>&);
    void getShapeFunction(double nodes_coordinate[8][3],
                          std::vector<std::vector<double>>&,
                          vector<vector<double>>& ShapeFunction,
                          vector<vector<vector<double>>>& ShapeFunction_dxy,
                          vector<double>& value_jkb,
                          const int num_GP);
    ~LinearHex8(){};
    
    
};

#endif