#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <chrono>
#include <thread>

#include "BaseSolver.h"
#include "mesh.h"
#include "input.h"
#include "dof_map.h"
#include "post.h"
#include "SolverInterface.h"
#include "ConstraintManager.h"
#include "elementassembler.h"
#include "toolbox.h"
#include "SolverInterface.h"

void BaseSolver::linear_solver(const Eigen::SparseMatrix<double> &K,
                                          Eigen::VectorXd &P,
                                          const Eigen::SparseMatrix<double> &C,
                                          Eigen::VectorXd &G,
                                          Eigen::VectorXd & x,
                                          const std::string type)
{
    int K_row = K.rows();
    int K_col = K.cols();
    int C_row = C.rows();
    int C_col = C.cols();
    // 判断K是否为方阵
    if (K_row != K_col)
        toolbox::error("K size is wrong in linear solver");
    // 判断C和K是否列数相同
    if (K_col != C_col)
        toolbox::error("C size is wrong in linear solver");
    // 检查P和G的正确性
    if (P.size() != K_row)
        toolbox::error("P size is wrong in linear solver");
    if (G.size() != C_row)
        toolbox::error("G size is wrong in linear solver");

    Eigen::SparseMatrix<double> K_AL(K_row + C_row, K_row + C_row);
    K_AL.setZero();

    // 将K塞到增广矩阵中
    for (int ii = 0; ii < K.outerSize(); ++ii)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(K, ii); it; ++it)
        {
            K_AL.coeffRef(it.row(), it.col()) = it.value();
        }
    }
    // 将C塞到增广矩阵中
    for (int ii = 0; ii < C.outerSize(); ++ii)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(C, ii); it; ++it)
        {
            K_AL.coeffRef(it.row() + K_row, it.col()) = it.value();
        }
    }
    // 将C的转置塞到增广矩阵中
    Eigen::SparseMatrix<double> CT = C.transpose();
    for (int ii = 0; ii < CT.outerSize(); ++ii)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(CT, ii); it; ++it)
        {
            K_AL.coeffRef(it.row(), it.col() + K_col) = it.value();
        }
    }

    // 设置PG向量
    Eigen::VectorXd PG(P.size() + G.size());
    PG.setZero();

    PG.head(P.size()) = Eigen::Map<Eigen::VectorXd>(P.data(), P.size());
    // 将 B 的元素拷贝到 result 的后半部分
    PG.tail(G.size()) = Eigen::Map<Eigen::VectorXd>(G.data(), G.size());

    // 求解 K_AL * x = PG
    Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;

    solver.compute(K_AL);

    if (solver.info() != Eigen::Success)
        toolbox::error("分解失败");
    x = solver.solve(PG);
    if (solver.info() != Eigen::Success)
        toolbox::error("求解失败");


    if(std::getenv("CHECKSOLVER") != nullptr)
    {
        std::cout << "checking the error of linear_equation_solver:" << std::endl;
        Eigen::VectorXd test = K_AL * x;
        std::vector<double> a(PG.size());
        std::vector<double> b(test.size());
        for (int i = 0; i < PG.size(); i++) a[i] = PG[i];
        for (int i = 0; i < test.size(); i++) b[i] = test[i];
        toolbox::checkvector(a, b);
    }
}