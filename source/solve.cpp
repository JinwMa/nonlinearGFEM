
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
    for (int i = 0; i < mesh.actual_node_count; i++) //循环所有的节点
    {
        double x = mesh.mesh_node[i][0];
        double y = mesh.mesh_node[i][1];
        double z = mesh.mesh_node[i][2];
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
        double x = mesh.mesh_node[i][0];
        double y = mesh.mesh_node[i][1];
        double z = mesh.mesh_node[i][2];
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

    int num_dofs = mesh.actual_node_count * 3;
    int num_dof_constrain = Equations.size();
    int num_all = num_dofs + num_dof_constrain;
    Eigen::SparseMatrix<double> K(num_all, num_all);
    K.setZero();
    Eigen::VectorXd b(num_all);
    b.setZero();

    std::vector<Eigen::Triplet<double>> tripletList;
    vector<vector<double>> GaussPoint;
    auto elem = new LinearHex8;
    elem->SetGaussIntegration(3, GaussPoint);
    delete elem;
    for (int element_now = 0; element_now < mesh.actual_element_count; element_now++)
    {
        vector<int> node_ids_in_a_element = mesh.mesh_element[element_now];
        auto elem = new LinearHex8;
        double nodes_coordinates[8][3];
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
                nodes_coordinates[i][j] = mesh.mesh_node[node_ids_in_a_element[i] - 1][j];

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

    std::map<std::string, int> dof_map3 = {{"ux", 0}, {"uy", 1}, {"uz", 2}};

    for (int i = 0; i < Equations.size(); i++)
    {
        auto equation = Equations[i]; //取出一个约束方程
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
    // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;
    // Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

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
}