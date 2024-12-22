#ifndef LINEARHEX8BBAR1_H
#define LINEARHEX8BBAR1_H

#include "BaseElement.h"


using namespace std;

class LinearHex8Bbar : public BaseElement
{
    
    public:
    static const int num_edofs = 24;
    static const int num_nodes = 8;
    static const int num_node_dof = 3;
    public:
        ~LinearHex8Bbar() {};
        void initializeElement(ObjectElementData & element_data);
        string ReturnElementType() { return "FEM"; };
        // void ComputeInternalForce() {};
        void SetElement();
        void getShapeFunction(double nodes_coordinate[20][3],
                              std::vector<std::vector<double>> &,
                              vector<vector<double>> &ShapeFunction,
                              vector<vector<vector<double>>> &ShapeFunction_dxy,
                              vector<double> &value_jkb,
                              const int num_GP);

        void ComputeStiffness(ObjectElementData & element_data,
                              std::vector<double> & elementmat,
                              ObjectContralParam * contral_param = nullptr);
        


        void getShapeFunctionOnIntegrationPoint(std::vector<double> & p_coord,
                                                std::vector<double> & sf);



        void getCenterGaussPointSF(double nodes_coordinate[20][3],
                                   std::vector<double> & CenterShapeFunction,
                                   std::vector<std::vector<double>> & CenterShapeFunction_dxy,
                                   double & Center_Value_jkb);
    
    
};

#endif