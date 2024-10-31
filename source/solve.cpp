#include <string>
#include <vector>
#include "input.h"
#include "mesh.h"
#include "SolverInterface.h"

using namespace std;
void solve(Input * pinput, Mesh * pmesh)
{
    BaseSolver *structsolver;
    vector<string> solver_name = pinput->getVectorString("solver_list");
    assert(solver_name != 1);
    for (size_t i = 0; i < solver_name.size(); i++)
    {
        string solver_type = pinput->getString(solver_name[i] + "_type");
        if (solver_type == "LinearStaticSolver")
        {
            structsolver = new LinearStaticSolver;
        }
        else
        {
            toolbox::error("not support type of solver: " + solver_type);
        }
        structsolver->solve(pinput, pmesh);
        delete structsolver;
    }
    
    
}
























// #include <ctime>
// #include <string>
// #include <iostream>
// #include <iomanip>
// #include <vector>
// #include <Eigen/Sparse>
// #include <Eigen/PardisoSupport>
// #include <map>
// #include <Eigen/Dense>

// // #include <dof_map.h>
// #include"dof_map.h"
// #include "post.h"
// #include "mesh.h"
// #include "input.h"
// #include "elementassembler.h"
// using namespace std;

// struct Equation
// {
//     int id;
//     vector<int> node_ids;
//     vector<string> dofs;
//     vector<double> factors;
//     double rhs;
// };

// Eigen::MatrixXd computeNullSpace(const Eigen::SparseMatrix<double>& C)
// {
//     Eigen::MatrixXd A = Eigen::MatrixXd(C);
//     Eigen::CompleteOrthogonalDecomposition<Eigen::MatrixXd> cod(A);
//     cod.compute(A);
//     Eigen::MatrixXd V = cod.matrixZ().transpose();
//     Eigen::MatrixXd Null_space;// = V.block(0, cod.rank(), V.rows(), V.cols() - cod.rank());
//     Eigen::MatrixXd P = cod.colsPermutation();
//     Null_space = P * Null_space;
//     return Null_space;
// }

// Eigen::MatrixXd computeNullSpace2(const Eigen::SparseMatrix<double>& C)
// {
//     Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> qr;
//     qr.compute(C.transpose());
//     // 检查分解是否成功
//     if(qr.info() != Eigen::Success) {
//         std::cerr << "QR decomposition failed.\n";
//     }

//     auto Q=qr.matrixQ();
//     // 获取矩阵秩和零空间的维度
//     int rank = qr.rank();
//     int nullity = C.cols() - rank;
//     Eigen::MatrixXd identity = Eigen::MatrixXd::Identity(C.cols(), C.cols());   

//     std::cout << "Rank of matrix C: " << rank << "\n";
//     std::cout << "Nullity of matrix A (dimension of null space): " << nullity << "\n";

//     // 通过计算零空间,构建正交基
//     Eigen::MatrixXd Q_rightCols = (Q * identity).rightCols(nullity);
//     return Q_rightCols;//.transpose();
// }

// // void solve(Input &input, Mesh &mesh)
// // {
// //     Dof_Map DofMap(mesh);
// //     DofMap.BuildDofMap(mesh);
// //     const double eps = 1.E-10;
// //     int equation_id = 0;
// //     vector<Equation> Equations;
// //     vector<int> constrained_node_ids;

// //     for (int i = 0; i < mesh.actual_node_count; i++) // 循环所有的节点
// //     {
// //         double x = mesh.NodesCoordinate[mesh.NodeOrderInList[mesh.NodeIdList[i]] - 1][0];
// //         double y = mesh.NodesCoordinate[mesh.NodeOrderInList[mesh.NodeIdList[i]] - 1][1];
// //         double z = mesh.NodesCoordinate[mesh.NodeOrderInList[mesh.NodeIdList[i]] - 1][2];
// //         if (std::fabs(x) < eps)
// //         {
// //             constrained_node_ids.push_back(i + 1);
// //         }
// //         else if (std::fabs(y) < eps)
// //         {
// //             constrained_node_ids.push_back(i + 1);
// //         }
// //         else if (std::fabs(z) < eps)
// //         {
// //             constrained_node_ids.push_back(i + 1);
// //         }
// //         else if (std::fabs(x - 2.0) < eps)
// //         {
// //             constrained_node_ids.push_back(i + 1);
// //         }
// //         else if (std::fabs(y - 2.0) < eps)
// //         {
// //             constrained_node_ids.push_back(i + 1);
// //         }
// //         else if (std::fabs(z - 2.0) < eps)
// //         {
// //             constrained_node_ids.push_back(i + 1);
// //         }
// //     }
// //     for (int i = 0; i < constrained_node_ids.size(); i++)
// //     {
// //         int n = constrained_node_ids[i];  // 节点编号
// //         int nn = mesh.NodeOrderInList[n]; // 节点坐标的存储位置

// //         double x = mesh.NodesCoordinate[nn - 1][0];
// //         double y = mesh.NodesCoordinate[nn - 1][1];
// //         double z = mesh.NodesCoordinate[nn - 1][2];
// //         for (int j = 0; j < 3; j++)
// //         {
// //             Equation equation;
// //             equation.id = (equation_id++);
// //             equation.node_ids.push_back(constrained_node_ids[i]);
// //             equation.rhs = 0.0;
// //             if (j == 0)
// //             {
// //                 equation.dofs.push_back("ux");
// //                 equation.factors.push_back(0.1 + 0.1 * x + 0.2 * y + 0.2 * z);
// //             }
// //             else if (j == 1)
// //             {
// //                 equation.dofs.push_back("uy");
// //                 equation.factors.push_back(0.05 + 0.1 * x + 0.1 * y + 0.2 * z);
// //             }
// //             else if (j == 2)
// //             {
// //                 equation.dofs.push_back("uz");
// //                 equation.factors.push_back(0.05 + 0.1 * x + 0.2 * y + 0.2 * z);
// //             }
// //             Equations.push_back(equation);
// //         }
// //     }

// //     int num_dofs = mesh.actual_node_count * 3;
// //     int num_dof_constrain = Equations.size();
// //     int num_all = num_dofs + num_dof_constrain;
// //     Eigen::SparseMatrix<double> K(num_all, num_all);
// //     Eigen::SparseMatrix<double> C(num_dof_constrain, num_dofs);

// //     K.setZero();
// //     C.setZero();
// //     Eigen::VectorXd b(num_all);
// //     Eigen::VectorXd g(num_dof_constrain);
// //     b.setZero();
// //     g.setZero();

// //     std::vector<Eigen::Triplet<double>> tripletList;

// //     vector<vector<double>> GaussPoint;
// //     auto elem = new LinearHex8;
// //     elem->SetGaussIntegration(GaussPoint);
// //     delete elem;
// //     for (int element_now = 0; element_now < mesh.actual_element_count; element_now++)
// //     {
// //         int element_id = mesh.ElementIdList[element_now];
// //         int element_location = mesh.ElementOrderInList[element_id];
// //         vector<int> node_ids_in_a_element = mesh.NodesOnElements[element_location - 1];
// //         auto elem = new LinearHex8;
// //         double nodes_coordinates[8][3];
// //         for (int i = 0; i < 8; i++)
// //             for (int j = 0; j < 3; j++)
// //             {
// //                 int node_id = node_ids_in_a_element[i];
// //                 int node_location = mesh.NodeOrderInList[node_id];
// //                 nodes_coordinates[i][j] = mesh.NodesCoordinate[node_location - 1][j];
// //             }

// //         double elementmat[24][24];
// //         elem->ComputeStiffness(nodes_coordinates, GaussPoint, elementmat);
// //         for (int i = 0; i < 8; i++)
// //         {
// //             for (int j = 0; j < 8; j++)
// //             {
// //                 for (int ii = 0; ii < 3; ii++)
// //                 {
// //                     for (int jj = 0; jj < 3; jj++)
// //                     {
// //                         int iii = i * 3 + ii;
// //                         int jjj = j * 3 + jj;
// //                         double value = elementmat[iii][jjj];
// //                         int row = (node_ids_in_a_element[i] - 1) * 3 + ii;
// //                         int col = (node_ids_in_a_element[j] - 1) * 3 + jj;
// //                         tripletList.push_back(Eigen::Triplet<double>(row, col, value));
// //                     }
// //                 }
// //             }
// //         }
// //         delete elem;
// //     }
// //     for (const auto &triplet : tripletList)
// //     {
// //         K.coeffRef(triplet.row(), triplet.col()) += triplet.value();
// //     }

// //     std::map<std::string, int> dof_map3 = {{"ux", 0}, {"uy", 1}, {"uz", 2}};

// //     for (int i = 0; i < Equations.size(); i++)
// //     {
// //         auto equation = Equations[i]; // 取出一个约束方程
// //         int row = num_dofs + equation.id;
// //         int col = (equation.node_ids[0] - 1) * 3 + dof_map3[equation.dofs[0]];
// //         K.coeffRef(row, col) = 1.0;
// //         K.coeffRef(col, row) = 1.0;
// //         b(row) = equation.factors[0];
// //         C.coeffRef(equation.id, col) = 1.0;
// //         g(equation.id) = equation.factors[0];
// //     }
// //     Eigen::SparseMatrix<double> Ko = K.block(0, 0, num_dofs, num_dofs);
// //     Eigen::VectorXd bb = b.segment(0, num_dofs);
    
// //     // 找到 C 的零空间的基 P
// //     // Eigen::MatrixXd C_Dense = Eigen::MatrixXd(C);
// //     // Eigen::FullPivLU<Eigen::MatrixXd> lu(C_Dense);
// //     // Eigen::MatrixXd P = lu.kernel();
// //     Eigen::MatrixXd P = computeNullSpace2(C);
// //     Eigen::MatrixXd jc = Eigen::MatrixXd(C) * P;
// //     // 使用 SparseQR 分解求解 Cx=g
// //     Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> solver;
// //     C.makeCompressed();
// //     solver.compute(C);
// //     if (solver.info() != Eigen::Success)
// //     {
// //         std::cerr << "分解失败!" << std::endl;
// //     }

// //     Eigen::VectorXd xx = solver.solve(g);
// //     if (solver.info() != Eigen::Success)
// //     {
// //         std::cerr << "求解失败!" << std::endl;
// //     }

// //     // 输出特解 x
// //     // std::cout << "The solution x is:\n" << xx << std::endl;

// //     Eigen::SparseMatrix<double> P_sparse = P.sparseView();
// //     Eigen::SparseMatrix<double> PT_sparse = P.transpose().sparseView();

// //     Eigen::SparseMatrix<double> KoP = Ko * P_sparse;
// //     Eigen::SparseMatrix<double> PTKP = PT_sparse * KoP;

// //     // Eigen::MatrixXd PT = P.transpose();
// //     // Eigen::SparseMatrix<double> KoP = Ko * P.sparseView();
// //     // Eigen::SparseMatrix<double> PTKP = PT.sparseView() * KoP;
// //     // std::cout << xx.size() <<std::endl;
// //     Eigen::VectorXd PTf = P.transpose() * (bb - Ko * xx);

// //     // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver2;
// //     Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Upper> solver2;

// //     solver2.compute(PTKP);
// //     if (solver2.info() != Eigen::Success)
// //     {
// //         // 分解失败
// //         std::cerr << "分解失败" << std::endl;
// //         exit(0);
// //     }
// //     Eigen::VectorXd x = solver2.solve(PTf);
// //     if (solver.info() != Eigen::Success)
// //     {
// //         // 求解失败
// //         std::cerr << "求解失败" << std::endl;
// //         // exit(0);
// //     }

// //     // 还原解:

// //     Eigen::VectorXd xxx = P * x + xx;

// //     Post post("tecplot");
// //     post.onlymesh(mesh);

// //     std::vector<double> displacement(xxx.data(), xxx.data() + xxx.size());
// //     post.ShowDisplacement(mesh, DofMap, displacement);

// //     // exit(0);

// //     // // std::cout << "b" << b << std::endl;

// //     // // std::cout << Eigen::MatrixXd(K) << std::endl;

// //     // // Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Upper> solver;
// //     // // Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;

// //     // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;
// //     // // Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
// //     // // Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

// //     // // solver.setTolerance(1e-3);  // 设置收敛准则

// //     // solver.compute(K);
// //     // if (solver.info() != Eigen::Success)
// //     // {
// //     //     // 分解失败
// //     //     std::cerr << "分解失败" << std::endl;
// //     //     exit(0);
// //     // }
// //     // Eigen::VectorXd x = solver.solve(b);
// //     // if (solver.info() != Eigen::Success)
// //     // {
// //     //     // 求解失败
// //     //     std::cerr << "求解失败" << std::endl;
// //     //     // exit(0);
// //     // }
// //     // std::cout << "解 x:\n" << x << std::endl;
// // }