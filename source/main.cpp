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


int main(int argc, char *argv[])
{
    Input input(argv[1]); // 读入和解析input文件
    Mesh mesh(input.db["mesh_file_name"][0]);

    Input * p_input = &input;
    Mesh * p_mesh = &mesh;

    Dof_Map DofMap(p_mesh);
    DofMap.BuildDofMap(p_mesh);
    BaseSolver * structsolver;
    structsolver = new LinearStaticSolver;
    structsolver->solve();

    


    // std::clock_t c_start_mesh = std::clock();
    

    auto constraint_manager = new ConstraintManager();

    constraint_manager->takeDB(p_input, p_mesh);
    Eigen::SparseMatrix<double> C = constraint_manager->buildConstrintMatrix(p_mesh);
    Eigen::VectorXd G = constraint_manager->buildConstrintForce(p_mesh);
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

    // std::cout << solution;

    Post post("tecplot");
    post.onlymesh(mesh);

    std::vector<double> displacement(solution.data(), solution.data() + num_dofs);
    post.ShowDisplacement(mesh, DofMap, displacement);



    // std::cout << G;

    // std::cout << C << std::endl;

    return 0;
}