#ifndef LINEARHEX81_H
#define LINEARHEX81_H

#include "BaseElement.h"


using namespace std;

class LinearHex8 : public BaseElement
{
    
    public:
    static const int num_edofs = 24;
    static const int num_nodes = 8;
    static const int num_node_dof = 3;
    public:
        string ReturnElementType() { return "FEM"; };
        void ComputeStiffness(double nodes_coordinates[20][3],
                              std::vector<double> &displacement,
                              std::vector<double> &du,
                              std::vector<double> &ddu,
                              vector<double> &elementmat);
        void ComputeInternalForce() {};
        void SetElement();
        void getShapeFunction(double nodes_coordinate[20][3],
                              std::vector<std::vector<double>> &,
                              vector<vector<double>> &ShapeFunction,
                              vector<vector<vector<double>>> &ShapeFunction_dxy,
                              vector<double> &value_jkb,
                              const int num_GP);

        void ComputeStiffness(ObjectElementData & element_data,
                              std::vector<double> & elementmat);
        ~LinearHex8() {};


        void getShapeFunctionOnIntegrationPoint(std::vector<double> & p_coord,
                                                std::vector<double> & sf);
    
    
};

#endif