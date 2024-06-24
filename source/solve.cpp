#include <ctime>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include "mesh.h"
#include "input.h"
#include "LinearHex8.h"
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <map>
using namespace std;

struct Equation
{
    int id;
    vector<int> node_ids;
    vector<string> dofs;
    vector<double> factors;
    double rhs;
};

void solve(Input &input, Mesh &mesh)
{
    const double eps = 1.E-10;
    int equation_id = 0;
    vector<Equation> Equations;
    vector<int> constrained_node_ids;
    std::clock_t c_start_1 = std::clock();

    for (int i = 0; i < mesh.actual_node_count; i++) // 循环所有的节点
    {
        double x = mesh.NodesCoordinate[mesh.NodeOrderInList[mesh.NodeIdList[i]] - 1][0];
        double y = mesh.NodesCoordinate[mesh.NodeOrderInList[mesh.NodeIdList[i]] - 1][1];
        double z = mesh.NodesCoordinate[mesh.NodeOrderInList[mesh.NodeIdList[i]] - 1][2];
        if (std::fabs(x) < eps)
        {
            constrained_node_ids.push_back(i + 1);
        }
        else if (std::fabs(y) < eps)
        {
            constrained_node_ids.push_back(i + 1);
        }
        else if (std::fabs(z) < eps)
        {
            constrained_node_ids.push_back(i + 1);
        }
        else if (std::fabs(x - 2.0) < eps)
        {
            constrained_node_ids.push_back(i + 1);
        }
        else if (std::fabs(y - 2.0) < eps)
        {
            constrained_node_ids.push_back(i + 1);
        }
        else if (std::fabs(z - 2.0) < eps)
        {
            constrained_node_ids.push_back(i + 1);
        }
    }
    for (int i = 0; i < constrained_node_ids.size(); i++)
    {
        int n = constrained_node_ids[i]; // 节点编号
        int nn = mesh.NodeOrderInList[n]; // 节点坐标的存储位置

        double x = mesh.NodesCoordinate[nn - 1][0];
        double y = mesh.NodesCoordinate[nn - 1][1];
        double z = mesh.NodesCoordinate[nn - 1][2];
        for (int j = 0; j < 3; j++)
        {
            Equation equation;
            equation.id = (equation_id++);
            equation.node_ids.push_back(constrained_node_ids[i]);
            equation.rhs = 0.0;
            if (j == 0)
            {
                equation.dofs.push_back("ux");
                equation.factors.push_back(0.1 + 0.1 * x + 0.2 * y + 0.2 * z);
            }
            else if (j == 1)
            {
                equation.dofs.push_back("uy");
                equation.factors.push_back(0.05 + 0.1 * x + 0.1 * y + 0.2 * z);
            }
            else if (j == 2)
            {
                equation.dofs.push_back("uz");
                equation.factors.push_back(0.05 + 0.1 * x + 0.2 * y + 0.2 * z);
            }
            Equations.push_back(equation);
        }
    }
    std::clock_t c_start_2 = std::clock();
    std::cout << "time of solve in 21 " << 1000.0 * (c_start_2 - c_start_1) / CLOCKS_PER_SEC << std::endl;



    int num_dofs = mesh.actual_node_count * 3;
    int num_dof_constrain = Equations.size();
    int num_all = num_dofs + num_dof_constrain;
    Eigen::SparseMatrix<double> K(num_all, num_all);
    K.setZero();
    Eigen::VectorXd b(num_all);
    b.setZero();

    std::vector<Eigen::Triplet<double>> tripletList;
    std::clock_t c_start_3 = std::clock();
    std::cout << "time of solve in 32 " << 1000.0 * (c_start_3 - c_start_2) / CLOCKS_PER_SEC << std::endl;

    vector<vector<double>> GaussPoint;
    auto elem = new LinearHex8;
    elem->SetGaussIntegration(3, GaussPoint);
    delete elem;
    for (int element_now = 0; element_now < mesh.actual_element_count; element_now++)
    {
        std::clock_t c_start_111 = std::clock();
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
        std::clock_t c_start_222 = std::clock();
        elem->ComputeStiffness(nodes_coordinates, GaussPoint, elementmat);
        std::clock_t c_start_333 = std::clock();

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
        std::clock_t c_start_444 = std::clock();
        delete elem;
        std::cout << "time of solve in 222-111 " << 1000.0 * (c_start_222 - c_start_111) / CLOCKS_PER_SEC << std::endl;
        std::cout << "time of solve in 333-222 " << 1000.0 * (c_start_333 - c_start_222) / CLOCKS_PER_SEC << std::endl;
        std::cout << "time of solve in 444-333 " << 1000.0 * (c_start_444 - c_start_333) / CLOCKS_PER_SEC << std::endl;

    }
    std::clock_t c_start_4 = std::clock();
    std::cout << "time of solve in 43 " << 1000.0 * (c_start_4 - c_start_3) / CLOCKS_PER_SEC << std::endl;

    for (const auto &triplet : tripletList)
    {
        K.coeffRef(triplet.row(), triplet.col()) += triplet.value();
    }

    std::map<std::string, int> dof_map3 = {{"ux", 0}, {"uy", 1}, {"uz", 2}};

    for (int i = 0; i < Equations.size(); i++)
    {
        auto equation = Equations[i]; // 取出一个约束方程
        int row = num_dofs + equation.id;
        int col = (equation.node_ids[0] - 1) * 3 + dof_map3[equation.dofs[0]];
        // std::cout << row << " " << col << std::endl;
        K.coeffRef(row, col) = 1.0;
        K.coeffRef(col, row) = 1.0;
        b(row) = equation.factors[0];
    }

    // std::cout << "b" << b << std::endl;

    // std::cout << Eigen::MatrixXd(K) << std::endl;

    // Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Upper> solver;
    // Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    std::clock_t c_start_solve = std::clock();
    std::cout << "time of solve in 54 " << 1000.0 * (c_start_solve - c_start_4) / CLOCKS_PER_SEC << std::endl;

    Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;
    // Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
    // Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

    // solver.setTolerance(1e-3);  // 设置收敛准则

    solver.compute(K);
    if (solver.info() != Eigen::Success)
    {
        // 分解失败
        std::cerr << "分解失败" << std::endl;
        exit(0);
    }
    Eigen::VectorXd x = solver.solve(b);
    if (solver.info() != Eigen::Success)
    {
        // 求解失败
        std::cerr << "求解失败" << std::endl;
        // exit(0);
    }
    std::cout << "解 x:\n"
              << x << std::endl;
    std::clock_t c_start_exit = std::clock();
    std::cout << "time of solve in XXX " << 1000.0 * (c_start_exit - c_start_solve) / CLOCKS_PER_SEC << std::endl;
}