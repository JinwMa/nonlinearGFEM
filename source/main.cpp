#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <chrono>

#include "mesh.h"
#include "input.h"
#include "dof_map.h"
#include "post.h"
#include "SolverInterface.h"
#include "ConstraintManager.h"
#include "element.h"
#include "toolbox.h"


Eigen::VectorXd linear_solver(Eigen::SparseMatrix<double> K,
                              Eigen::VectorXd P,
                              Eigen::SparseMatrix<double> C,
                              Eigen::VectorXd G,
                              const std::string type);
// void solve(Input &input, Mesh &mesh);
// void test();
// Eigen::MatrixXd computeNullSpace(const Eigen::SparseMatrix<double>& C);
void solve_equation(Eigen::SparseMatrix<double> &K,
                    Eigen::SparseMatrix<double> &C,
                    Eigen::VectorXd &b,
                    Eigen::VectorXd &g,
                    Eigen::VectorXd &a);
// Eigen::MatrixXd computeNullSpace2(const Eigen::SparseMatrix<double>& C);

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

void a_eigen_test();

int main(int argc, char *argv[])
{

    // a_eigen_test();
    Eigen::SparseMatrix<double> K(2, 2);
    K.setZero();
    K.coeffRef(0, 0) = 1.0;
    K.coeffRef(1, 1) = 3.0;
    Eigen::SparseMatrix<double> C(1, 2);
    C.setZero();
    C.coeffRef(0, 0) = 2.0;

    Eigen::VectorXd P(2);
    P(0) = 1.0;
    P(1) = 1.0;
    Eigen::VectorXd G(1);
    G(0) = 3.0;
    const std::string type = "largin";
    Eigen::VectorXd u = linear_solver(K, P, C, G, type);

    std::cout << u << std::endl;
    // test();

    // Input input(argv[1]); // 读入和解析input文件

    // std::cout << "The input parameters for the current problem:" << std::endl;
    // for (auto & it : input.db)
    // {
    //   std::cout << "##########-- a piece of db --##########" <<std::endl;
    //   std::cout << it.first << " \n";// << it.second;
    //   for (size_t i = 0; i < it.second.size(); i++)
    //     std::cout << it.second[i] << std::endl;
    // }

    // // auto solver = new LinearStaticSolver;
    // // solver->solve();

    // std::clock_t c_start_mesh = std::clock();
    // Mesh mesh(input.db["mesh_file_name"][0]);

    // Dof_Map DofMap(mesh);
    // DofMap.BuildDofMap(mesh);

    // auto constraint_manager = new ConstraintManager();

    // constraint_manager->takeDB(input, mesh);
    // Eigen::SparseMatrix<double> C = constraint_manager->buildConstrintMatrix(mesh);

    return 0;
}

void solve_equation(Eigen::SparseMatrix<double> &K,
                    Eigen::SparseMatrix<double> &C,
                    Eigen::VectorXd &b,
                    Eigen::VectorXd &g,
                    Eigen::VectorXd &a)
{
    int rowsK = K.rows();
    int colsK = K.cols();
    int rowsC = K.rows();
    int colsC = K.cols();

    // 创建一个大的稀疏矩阵
    Eigen::SparseMatrix<double> bigMatrix(rowsK + rowsC, colsK + colsC);

    bigMatrix.setZero();

    // 填充左上角为 k
    // bigMatrix.topLeftCorner(rowsK, colsK) = K;

    // // 填充右上角为 c
    // bigMatrix.topRightCorner(rowsK, colsC) = C;

    // Eigen::SparseMatrix<double> temp = C.transpose(); // 先存储转置结果
    // bigMatrix.bottomLeftCorner(rowsC, colsK) = temp;  // 再进行赋值
};

void a_eigen_test()
{
    std::cout << "pass here!!!" << std::endl;
    Eigen::SparseMatrix<double> A(3, 2); // 3x2 矩阵
    Eigen::SparseMatrix<double> B(3, 2); // 3x2 矩阵

    const int num = 1000000;

    Eigen::SparseMatrix<double> K(num, num);
    K.setZero();

    // std::cout << K << std::endl;

    for (int i = 0; i < num; i++)
        K.coeffRef(i, i) = 2.0;

    Eigen::VectorXd P(num);

    for (int i = 0; i < num; i++)
        P(i) = i + 1.0;

    // std::cout << P << std::endl;


    Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;

    solver.compute(K);

    if (solver.info() != Eigen::Success)
    {
        // 分解失败
        std::cerr << "分解失败" << std::endl;
        exit(0);
    }
    Eigen::VectorXd x = solver.solve(P);
    if (solver.info() != Eigen::Success)
    {
        // 求解失败
        std::cerr << "求解失败" << std::endl;
        // exit(0);
    }

    std::cout << " the solution is " << std::endl;

    std::cout << x << std::endl;


    A.coeffRef(0, 0) = 1.0;
    A.coeffRef(1, 1) = 2.0;

    B.coeffRef(0, 0) = 3.0;
    B.coeffRef(2, 1) = 4.0;

    // 创建一个新的稀疏矩阵来存储拼接结果，维度为 (3, 4)
    Eigen::SparseMatrix<double> C(3, 4);

    // 将 A 的元素插入到 C 的前两列
    for (int k = 0; k < A.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(A, k); it; ++it)
        {
            C.coeffRef(it.row(), it.col()) = it.value();
        }
    }

    // 将 B 的元素插入到 C 的后两列
    for (int k = 0; k < B.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(B, k); it; ++it)
        {
            C.coeffRef(it.row(), it.col() + A.cols()) = it.value();
        }
    }

    // 输出拼接结果
    for (int k = 0; k < C.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<double>::InnerIterator it(C, k); it; ++it)
        {
            std::cout << "Element (" << it.row() << ", " << it.col()
                      << ") = " << it.value() << std::endl;
        }
    }

    // return 0;
}


Eigen::VectorXd linear_solver(Eigen::SparseMatrix<double> K,
                              Eigen::VectorXd P,
                              Eigen::SparseMatrix<double> C,
                              Eigen::VectorXd G,
                              const std::string type)
{
    int K_row = K.rows();
    int K_col = K.cols();
    int C_row = C.rows();
    int C_col = C.cols();
    // 判断K是否为方阵
    if (K_row != K_col) toolbox::error("K size is wrong in linear solver");
    // 判断C和K是否列数相同
    if (K_col != C_col) toolbox::error("C size is wrong in linear solver");
    // 检查P和G的正确性
    if (P.size() != K_row) toolbox::error("P size is wrong in linear solver");
    if (G.size() != C_row) toolbox::error("G size is wrong in linear solver");

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

    //求解 K_AL * x = PG
    Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;

    solver.compute(K_AL);

    if (solver.info() != Eigen::Success)
    {
        // 分解失败
        std::cerr << "分解失败" << std::endl;
        exit(0);
    }
    Eigen::VectorXd x = solver.solve(PG);
    if (solver.info() != Eigen::Success)
    {
        // 求解失败
        std::cerr << "求解失败" << std::endl;
        // exit(0);
    }

    std::cout << " the solution is " << std::endl;

    std::cout << x << std::endl;

    return x;

}
