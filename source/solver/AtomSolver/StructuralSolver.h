#ifndef STRUCTURALSOLVER_H
#define STRUCTURALSOLVER_H

#include "BaseAtomSolver.h"

class StructuralSolver : public BaseAtomSolver
{
    public:
    StructuralSolver(std::shared_ptr<DataBase> solve_db, std::shared_ptr<Mesh> mesh) 
       :BaseAtomSolver(solve_db, mesh){};
    virtual ~StructuralSolver(){};

    // 成员变量
    Eigen::VectorXd d_u;
    Eigen::VectorXd d_du;
    Eigen::VectorXd d_ddu;
    Eigen::VectorXd d_lambda;    
    Eigen::VectorXd d_internal_force;
    Eigen::VectorXd d_rhs;  
    Eigen::VectorXd d_rhs_G;

    Eigen::VectorXd d_G;
    Eigen::VectorXd d_P;
    Eigen::VectorXd d_P_trial;
    Eigen::VectorXd d_G_trial;

    Eigen::SparseMatrix<double> d_C;
    Eigen::SparseMatrix<double> d_K;

};




#endif