#ifndef LINEARTET41_H
#define LINEARTET41_H


#include "BaseElement.h"

using namespace std;

class LinearTet4 : public BaseElement
{
    private:
    static const int num_edofs = 12;
    static const int num_nodes = 4;
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
        ~LinearTet4() {};

    public:
        void getShapeFunctionOnIntegrationPoint(std::vector<double> &p_coord,
                                                std::vector<double> &sf);
};



#endif