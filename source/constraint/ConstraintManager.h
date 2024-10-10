#ifndef CONSTRAINTMANAGER_H
#define CONSTRAINTMANAGER_H

#include<string>
#include<iostream>
#include<vector>

#include"input.h"
#include"mesh.h"
#include"SPC.h"
#include"BaseConstraint.h"
#include<Eigen/Sparse>
using namespace std;

class ConstraintManager
{
    public:    
    ConstraintManager(){};
    ~ConstraintManager(){};
    void takeDB(Input * input, Mesh * mesh);
    void buildDofMap();
    vector<ConstraintEquation> buildConstrintEquation();
    Eigen::SparseMatrix<double> buildConstrintMatrix(Mesh * mesh);
    Eigen::VectorXd buildConstrintForce(Mesh * mesh);
    Eigen::SparseMatrix<double> buildConstrintStiffness(){};
    
    int d_equations_num = 0;
    vector<vector<ConstraintEquation>> FinalConstraintEquations;
    vector<string> d_boundary_conditions;
};








#endif