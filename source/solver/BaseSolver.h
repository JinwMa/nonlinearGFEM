#ifndef BASESOLVER_H
#define BASESOLVER_H


#include <string>
#include <iostream>
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <omp.h>
#include <chrono>
#include <sstream>
#include <map>
#include <unordered_set>
#include <thread>
#include <Eigen/IterativeLinearSolvers>



#include "dof_map.h"
#include "input.h"
#include "mesh.h"
#include "ConstraintManager.h"
#include "elementassembler.h"
#include "LoadManger.h"
#include "post.h"
#include "ObjectElementData.h"
#include "toolbox.h"



class BaseSolver
{
    private:
    const int num_nonzero = 100;
public:
    BaseSolver()
    {
        std::cout << "****************   build BaseSolver     ******************" << std::endl;
    };
    virtual ~BaseSolver()
    {
        std::cout << "****************   delete BaseSolver     ******************" << std::endl;
    };

    virtual void solve(Input * pinput, Mesh * pmesh) = 0;
    virtual void takeDB() {};
    virtual void takeDB(Input * pinput) {};
    virtual void init(Input * pinput, Mesh * pmesh);
    virtual void deallocate(){};

protected:
    std::vector<ObjectElementData> d_element_data;

protected:
    void linear_solver(const Eigen::SparseMatrix<double> & K,
                                  Eigen::VectorXd & P,
                                  const Eigen::SparseMatrix<double> & C,
                                  Eigen::VectorXd & G,
                                  Eigen::VectorXd & x,
                                  const std::string type = "L");

    void linear_solver2(const Eigen::SparseMatrix<double> &K,
                       Eigen::VectorXd &P,
                       const Eigen::SparseMatrix<double> &C,
                       Eigen::VectorXd &G,
                       Eigen::VectorXd &x,
                       const std::string type = "P");

    Eigen::MatrixXd computeNullSpace(const Eigen::SparseMatrix<double> &C)
    {
        Eigen::MatrixXd A = Eigen::MatrixXd(C);
        Eigen::CompleteOrthogonalDecomposition<Eigen::MatrixXd> cod(A);
        cod.compute(A);
        Eigen::MatrixXd V = cod.matrixZ().transpose();
        Eigen::MatrixXd Null_space; // = V.block(0, cod.rank(), V.rows(), V.cols() - cod.rank());
        Eigen::MatrixXd P = cod.colsPermutation();
        Null_space = P * Null_space;
        return Null_space;
    }

    Eigen::MatrixXd computeNullSpace2(const Eigen::SparseMatrix<double> &C)
    {
        Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> qr;
        qr.compute(C.transpose());
        // 检查分解是否成功
        if (qr.info() != Eigen::Success)
        {
            std::cerr << "QR decomposition failed.\n";
        }

        auto Q = qr.matrixQ();
        // 获取矩阵秩和零空间的维度
        int rank = qr.rank();
        int nullity = C.cols() - rank;
        Eigen::MatrixXd identity = Eigen::MatrixXd::Identity(C.cols(), C.cols());

        std::cout << "Rank of matrix C: " << rank << "\n";
        std::cout << "Nullity of matrix A (dimension of null space): " << nullity << "\n";

        // 通过计算零空间,构建正交基
        Eigen::MatrixXd Q_rightCols = (Q * identity).rightCols(nullity);
        return Q_rightCols; //.transpose();
    }
    void sparseMatrixMultiply(const Eigen::SparseMatrix<double> & A, const Eigen::SparseMatrix<double> & B, Eigen::SparseMatrix<double> & C);

};

#endif