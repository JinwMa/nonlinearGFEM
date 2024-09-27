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
    vector<vector<ConstraintEquation>> FinalConstraintEquations;
    ConstraintManager(){};
    ~ConstraintManager(){};
    void takeDB(Input & input, Mesh & mesh);
    void buildDofMap();
    vector<ConstraintEquation> buildConstrintEquation();
    Eigen::SparseMatrix<double> buildConstrintMatrix();
    Eigen::SparseMatrix<double> buildConstrintStiffness(){};
    Eigen::SparseMatrix<double> buildConstrintForce(){};
};








#endif