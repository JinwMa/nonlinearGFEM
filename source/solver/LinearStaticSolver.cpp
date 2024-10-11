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
    auto constraint_manager = new ConstraintManager();
    std::cout << "pass here" << std::endl;

    
    Dof_Map DofMap(pmesh);
    DofMap.BuildDofMap(pmesh);

    constraint_manager->takeDB(pinput, pmesh);

    Eigen::SparseMatrix<double> C = constraint_manager->buildConstrintMatrix(pmesh);
    Eigen::VectorXd G = constraint_manager->buildConstrintForce(pmesh);

    int num_dofs = pmesh->actual_node_count * 3;
    Eigen::SparseMatrix<double> K(num_dofs, num_dofs);
    K.setZero();
    Eigen::VectorXd b(num_dofs);
    b.setZero();

    std::vector<Eigen::Triplet<double>> tripletList;

    vector<vector<double>> GaussPoint;
    auto elem = new LinearHex8;
    elem->SetGaussIntegration(GaussPoint);
    delete elem;
    for (int element_now = 0; element_now < pmesh->actual_element_count; element_now++)
    {
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
    for (const auto &triplet : tripletList)
    {
        K.coeffRef(triplet.row(), triplet.col()) += triplet.value();
    }

    // Eigen::VectorXd solution = linear_solver(K, b, C, G);
    Eigen::VectorXd solution = this->linear_solver(K, b, C, G);

    delete constraint_manager;



    Post post("tecplot");
    post.onlymesh(pmesh);

    std::vector<double> displacement(solution.data(), solution.data() + num_dofs);
    post.ShowDisplacement(pmesh, DofMap, displacement);
}
