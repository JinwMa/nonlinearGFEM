#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/IterativeLinearSolvers>
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
    auto start = std::chrono::high_resolution_clock::now();
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
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "assembleK_AL: " << duration.count() << " ms" << std::endl;
    

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


void BaseSolver::linear_solver2(const Eigen::SparseMatrix<double> &K,
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

    Eigen::SparseMatrix<double> CT = C.transpose();
    Eigen::SparseMatrix<double> CTC;
    sparseMatrixMultiply(CT, C, CTC);
    CTC = CTC * 1.E11;
    Eigen::SparseMatrix<double> K_AL = K + CTC;

    

    // 设置PG向量
    Eigen::VectorXd PG = CT * G * 1.E11 + P;
    // 求解 K_AL * x = PG
    // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;
    Eigen::PardisoLDLT<Eigen::SparseMatrix<double>> solver;            // 23.6s
    // Eigen::ConjugateGradient<Eigen::SparseMatrix<double>> solver;   //121s



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

void BaseSolver::sparseMatrixMultiply(const Eigen::SparseMatrix<double> &A, const Eigen::SparseMatrix<double> &B, Eigen::SparseMatrix<double> &C)
{
    assert(A.cols() == B.rows() && "Matrix dimensions must agree.");
    C.resize(A.rows(), B.cols());

    Eigen::SparseMatrix<double> B_transposed = B.transpose();
    C.reserve(Eigen::VectorXi::Constant(A.rows(), num_nonzero));  // 假设每行最多有n个非零项,可以根据实际情况调整 

     for (int i = 0; i < A.outerSize(); ++i) {
        for (Eigen::SparseMatrix<double>::InnerIterator itA(A, i); itA; ++itA) {
            // itA.row() 是行号, itA.col() 是列号, itA.value() 是值
            int rowA = itA.row();
            int colA = itA.col();
            double valueA = itA.value();

            // 遍历 B 的第 colA 行(即 B 转置后的第 colA 列)
            for (Eigen::SparseMatrix<double>::InnerIterator itB(B_transposed, colA); itB; ++itB) {
                int colB = itB.row();  // B 转置后的列号(实际是 B 的列号)
                double valueB = itB.value();
                // 将结果加入到三元组列表中
                C.coeffRef(rowA, colB) += valueA * valueB;
            }
        }
    }

}
