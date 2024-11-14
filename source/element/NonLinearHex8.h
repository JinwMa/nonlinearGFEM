#ifndef NONLINEARHEX81_H
#define NONLINEARHEX81_H

#include "BaseElement.h"

class NonLinearHex8 : public BaseElement
{
    private:
    const int d_num_edofs = 24;
    const int d_num_nodes = 8;
    const int d_num_node_dof = 3; 
    public:
        ~NonLinearHex8() {};
        void SetElement();

        string ReturnElementType() { return "NonLinearFEM"; };

        void ComputeStiffness(ObjectElementData &elementdata,
                              std::vector<double> &elementmat) override;

        void getShapeFunction(double nodes_coordinate[20][3],
                                      std::vector<std::vector<double>> & GaussPoints,
                                      vector<vector<double>> &ShapeFunction,
                                      vector<vector<vector<double>>> &ShapeFunction_dxy,
                                      vector<double> &value_jkb,
                                      const int num_GP);

    private:        

        void initialize_element(double nodes_coordinate[20][3],
                                std::vector<std::vector<double>> &GaussPoints,
                                vector<vector<double>> &ShapeFunction,
                                vector<vector<vector<double>>> &ShapeFunction_dxy,
                                vector<double> &value_jkb,
                                const int num_GP,
                                ObjectElementData &elementdata);

       
        void updateF_Finv(ObjectElementData & elementdata);
};



#endif