#ifndef SPC_H
#define SPC_H


#include<iostream>
#include<string>
#include<vector>
#include"ConstraintManager.h"
#include"BaseConstraint.h"
#include"input.h"
#include"mesh.h"


//input文件的输入格式:
/*
boundary_conditions = SPC0, SPC1, ...
SPC0_type = SPC
SPC0_nodes_type = defalut // geometry // node_ids  (defalut = node_ids)
// defalut or node_ids
SPC0_node_ids = 1, 2, 3, ...
// geometry
SPC0_geometry_expression = 0.0, 1.0, ...    a0 + a1*x + a2*y + a3*z + ...
SPC0_value = 0.0
SPC0_value_expression = 0.0, 1.0, ...    a0 + a1*x + a2*y + a3*z + ...
SPC0_dof = "ux"
*/



using namespace std;

class SPC : public BaseConstraint
{
    string name;
    public:    
    SPC(string SPC_name) : name(SPC_name){};
    ~SPC(){};
    void takeDB(Input * pinput, Mesh * pmesh);
    void buildDofMap(){
        // std::cout << "calling buildDofMap" << std::endl;
    };
    vector<ConstraintEquation> buildEquations(Mesh * pmesh);

    private:
    vector<int> d_node_ids;
    vector<double> d_geometry_expression;
    string d_dof;    
    double d_value;
    // vector<double> d_values;
    vector<double> d_value_expression;
    bool d_is_value = false;
    bool d_is_value_expression = false;
    void getNodesFromGeometry(Mesh * pmesh);
};




#endif