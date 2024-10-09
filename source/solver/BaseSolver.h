#ifndef BASESOLVER_H
#define BASESOLVER_H

#include<string>
#include<iostream>
#include<vector>

#include"input.h"
#include"mesh.h"
#include"SPC.h"
#include"BaseConstraint.h"
#include<Eigen/Sparse>
class BaseSolver
{
    public:
    BaseSolver(){};
    ~BaseSolver(){
        std::cout << "delete BaseSolver" << std::endl;
    };

    virtual void solve() = 0;
    // virtual void takeDB();
    Eigen::VectorXd linear_solver(Eigen::SparseMatrix<double> K,
                              Eigen::VectorXd P,
                              Eigen::SparseMatrix<double> C,
                              Eigen::VectorXd G,
                              const std::string type = "L");

};





#endif