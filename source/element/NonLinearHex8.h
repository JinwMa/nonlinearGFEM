#ifndef NONLINEARHEX81_H
#define NONLINEARHEX81_H

#include "BaseElement.h"

class NonLinearHex8 : public BaseElement
{
    public:
    static const int num_edofs = 24;
    static const int num_nodes = 8;
    static const int num_node_dof = 3; 
    public:
        ~NonLinearHex8() {};
        void SetElement();

        string ReturnElementType() { return "NonLinearFEM"; };
        virtual void ComputeStiffness(double nodes_coordinates[20][3],
                                      std::vector<double> &displacement,
                                      std::vector<double> &du,
                                      std::vector<double> &ddu,
                                      std::vector<double> &elementmat);

        virtual void getShapeFunction(double nodes_coordinate[20][3],
                                      std::vector<std::vector<double>> &,
                                      vector<vector<double>> &ShapeFunction,
                                      vector<vector<vector<double>>> &ShapeFunction_dxy,
                                      vector<double> &value_jkb,
                                      const int num_GP);
};



#endif