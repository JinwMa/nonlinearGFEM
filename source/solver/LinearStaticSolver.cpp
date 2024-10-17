#include <omp.h>
#include <chrono>
#include "LinearStaticSolver.h"
#include "ConstraintManager.h"
#include "element.h"
#include "post.h"

#include "dof_map.h"

void LinearStaticSolver::takeDB()
{
}
void LinearStaticSolver::solve(Input *pinput, Mesh *pmesh)
{
    Eigen::SparseMatrix<double> C;
    Eigen::SparseMatrix<double> K;
    Eigen::VectorXd G;
    Eigen::VectorXd P;
    // 自由度映射列表
    auto start_dof_map = std::chrono::high_resolution_clock::now();
    std::cout << "building DofMap" << std::endl;
    Dof_Map DofMap(pmesh);
    DofMap.BuildDofMap(pmesh);
    std::cout << "complete DofMap" << std::endl;
    auto end_dof_map = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_dof_map = end_dof_map - start_dof_map;
    std::cout << "build DofMap time: " << duration_dof_map.count() << " ms" << std::endl;

    // 构造约束
    auto start_constraint = std::chrono::high_resolution_clock::now();
    std::cout << "building constraint" << std::endl;
    auto constraint_manager = new ConstraintManager();
    constraint_manager->takeDB(pinput, pmesh);
    C = constraint_manager->buildConstrintMatrix(pmesh);
    G = constraint_manager->buildConstrintForce(pmesh);
    delete constraint_manager;
    std::cout << "complete constraint" << std::endl;
    auto end_constraint = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_constraint = end_constraint - start_constraint;
    std::cout << "build Constraint time: " << duration_constraint.count() << " ms" << std::endl;

    // 构造刚度矩阵,构造右端项
    std::cout << "building stiffness" << std::endl;
    this->assembleElementStiffness(pinput, pmesh, K);
    std::cout << "complete the stiffness " << std::endl;
    this->vector_assembler(pinput, pmesh, P);

    std::cout << "solving the linear equations" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Eigen::VectorXd solution = linear_solver(K, P, C, G);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "complete the solve" << std::endl;
    // 计算持续时间并转换为毫秒
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "solve Ax=b time: " << duration.count() << " ms" << std::endl;

    Post post("tecplot");
    post.onlymesh(pmesh);

    std::vector<double> displacement(solution.data(), solution.data() + P.size());
    post.ShowDisplacement(pmesh, DofMap, displacement);
}

void LinearStaticSolver::assembleElementStiffness(Input *pinput, Mesh *pmesh, Eigen::SparseMatrix<double> &K)
{
    int num_dofs = pmesh->actual_node_count * 3;
    // Eigen::SparseMatrix<double> K;
    K.resize(num_dofs, num_dofs);
    K.setZero();
    // std::vector<Eigen::Triplet<double>> tripletList;
    vector<vector<double>> GaussPoint;
    auto elem = new LinearHex8;
    elem->SetGaussIntegration(GaussPoint);
    delete elem;
    omp_set_num_threads(6);
    std::vector<std::vector<Eigen::Triplet<double>>> tripletLists(omp_get_max_threads());

    std::cout << "the max num of threads is " << omp_get_max_threads() << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel for
    for (int element_now = 0; element_now < pmesh->actual_element_count; element_now++)
    {
        int thread_id = omp_get_thread_num();
        auto &tripletList = tripletLists[thread_id];

        // std::cout << element_now << std::endl;
        int element_id = pmesh->ElementIdList[element_now];
        int element_location = pmesh->ElementOrderInList[element_id];
        vector<int> node_ids_in_a_element = pmesh->NodesOnElements[element_location - 1];
        auto elem = new LinearHex8;
        double nodes_coordinates[8][3];
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
            {
                int node_id = node_ids_in_a_element[i];
                int node_location = pmesh->NodeOrderInList[node_id];
                nodes_coordinates[i][j] = pmesh->NodesCoordinate[node_location - 1][j];
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
    // 合并所有线程的tripletLists
    std::vector<Eigen::Triplet<double>> finalTripletList;
    for (const auto &localList : tripletLists)
    {
        finalTripletList.insert(finalTripletList.end(), localList.begin(), localList.end());
    }
    // 设置稀疏矩阵
    K.setFromTriplets(finalTripletList.begin(), finalTripletList.end());

    auto end = std::chrono::high_resolution_clock::now();
    // 计算持续时间并转换为毫秒
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "assemble stiffness time: " << duration.count() << " ms" << std::endl;
}

void LinearStaticSolver::vector_assembler(Input *pinput, Mesh *pmesh, Eigen::VectorXd &P)
{
    int num_dofs = pmesh->actual_node_count * 3;
    P.resize(num_dofs);
    P.setZero();
}
