#ifndef CONSTRAINTMANAGER_H
#define CONSTRAINTMANAGER_H



#include<string>
#include<iostream>
#include<vector>

#include"input.h"
#include"mesh.h"
// #include"SPC.h"
#include"BaseConstraint.h"
#include"DofMap.h"
#include<Eigen/Sparse>
using namespace std;



class ConstraintManager
{
    public:    
    // ConstraintManager(){};
    // ~ConstraintManager(){};
    // void takeDB(shared_ptr<DataBase> db);
    // vector<ConstraintEquation> buildConstrintEquation();
    // Eigen::SparseMatrix<double> buildConstrintMatrix(Mesh * pmesh, Dof_Map * pdofmap);
    // Eigen::VectorXd buildConstrintForce(Mesh * pmesh, Dof_Map * pdofmap);
    // Eigen::SparseMatrix<double> buildConstrintStiffness(){};
    // void buildMsetDofs(Dof_Map * pdofmap);
    
    // int d_equations_num = 0;
    // vector<vector<ConstraintEquation>> FinalConstraintEquations;
    // vector<string> d_boundary_conditions;

    // unordered_set<int> all_m_set;
    // unordered_set<int> d_m_set_dofs;
};


#endif