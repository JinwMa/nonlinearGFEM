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
#include "SolverInterface.h"


Eigen::VectorXd linear_solver(Eigen::SparseMatrix<double> K,
                              Eigen::VectorXd P,
                              Eigen::SparseMatrix<double> C,
                              Eigen::VectorXd G,
                              const std::string type = "L");
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

    BaseSolver * structsolver;
    structsolver = new LinearStaticSolver();
    structsolver->solve();
    // delete structsolver;
    // exit(0);

    Input input(argv[1]); // 读入和解析input文件


    // std::clock_t c_start_mesh = std::clock();
    Mesh mesh(input.db["mesh_file_name"][0]);

    Dof_Map DofMap(mesh);
    DofMap.BuildDofMap(mesh);

    auto constraint_manager = new ConstraintManager();

    constraint_manager->takeDB(input, mesh);
    Eigen::SparseMatrix<double> C = constraint_manager->buildConstrintMatrix(mesh);
    Eigen::VectorXd G = constraint_manager->buildConstrintForce(mesh);
    delete constraint_manager;


    int num_dofs = mesh.actual_node_count * 3;
    // int num_dof_constrain = Equations.size();
    // int num_all = num_dofs + num_dof_constrain;
    Eigen::SparseMatrix<double> K(num_dofs, num_dofs);
    // Eigen::SparseMatrix<double> C(num_dof_constrain, num_dofs);

    K.setZero();
    Eigen::VectorXd b(num_dofs);
    b.setZero();

    std::vector<Eigen::Triplet<double>> tripletList;

    vector<vector<double>> GaussPoint;
    auto elem = new LinearHex8;
    elem->SetGaussIntegration(GaussPoint);
    delete elem;
    for (int element_now = 0; element_now < mesh.actual_element_count; element_now++)
    {
        int element_id = mesh.ElementIdList[element_now];
        int element_location = mesh.ElementOrderInList[element_id];
        vector<int> node_ids_in_a_element = mesh.NodesOnElements[element_location - 1];
        auto elem = new LinearHex8;
        double nodes_coordinates[8][3];
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
            {
                int node_id = node_ids_in_a_element[i];
                int node_location = mesh.NodeOrderInList[node_id];
                nodes_coordinates[i][j] = mesh.NodesCoordinate[node_location - 1][j];
            }

        double elementmat[24][24];
        elem->ComputeStiffness(nodes_coordinates, GaussPoint, elementmat);
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                for (int ii = 0; ii < 3; ii++)
                {
                    for (int jj = 0; jj < 3; jj++)
                    {
                        int iii = i * 3 + ii;
                        int jjj = j * 3 + jj;
                        double value = elementmat[iii][jjj];
                        int row = (node_ids_in_a_element[i] - 1) * 3 + ii;
                        int col = (node_ids_in_a_element[j] - 1) * 3 + jj;
                        tripletList.push_back(Eigen::Triplet<double>(row, col, value));
                    }
                }
            }
        }
        delete elem;
    }
    for (const auto &triplet : tripletList)
    {
        K.coeffRef(triplet.row(), triplet.col()) += triplet.value();
    }

    // Eigen::VectorXd solution = linear_solver(K, b, C, G);
    Eigen::VectorXd solution = structsolver->linear_solver(K, b, C, G);
    delete structsolver;

    std::cout << solution;

    Post post("tecplot");
    post.onlymesh(mesh);

    std::vector<double> displacement(solution.data(), solution.data() + num_dofs);
    post.ShowDisplacement(mesh, DofMap, displacement);



    // std::cout << G;

    // std::cout << C << std::endl;

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
    std::cout << C_col << "  C_size  " << C_row << std::endl;
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
