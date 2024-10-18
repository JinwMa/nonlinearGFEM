#include "elementassembler.h"

void ElementAssembler::takeDB(Input * pinput, Mesh * pmesh, Dof_Map * pdofmap)
{
    elementList = pinput->getVectorString("element_list");
}


void ElementAssembler::assembleElementStiffness(Input *pinput, Mesh *pmesh, Dof_Map * pdofmap, Eigen::SparseMatrix<double> &K)
{    
    //开始计时:
    auto start = std::chrono::high_resolution_clock::now();
    K.resize(pdofmap->dof_size, pdofmap->dof_size);
    K.setZero();
    std::vector<Eigen::Triplet<double>>finalTripletList;    

    for (auto name : elementList)
    {
        BaseElement * elem;
        vector<vector<double>> GaussPoint;
        std::string element_type = pinput->getString(name + "_type");
        if (element_type == "LinearHex8")
        {
            elem = new LinearHex8;
        }
        else
        {
            toolbox::error("not supprot this type of element: " + element_type);
        }
        elem->takeDB(pinput, pmesh, name);
        elem->SetGaussIntegration(GaussPoint);
        std::vector<int> element_ids = elem->element_ids;
        std::vector<std::vector<Eigen::Triplet<double>>> tripletLists;
        elementSetStiffnessAssemble(pinput, pmesh, pdofmap, element_ids, elem, tripletLists);
        delete elem;
        for (const auto &localList : tripletLists)
        {
            finalTripletList.insert(finalTripletList.end(), localList.begin(), localList.end());
        }
    }    
    // 设置稀疏矩阵
    K.setFromTriplets(finalTripletList.begin(), finalTripletList.end());
    auto end = std::chrono::high_resolution_clock::now();
    // 计算持续时间并转换为毫秒
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "assemble stiffness time: " << duration.count() << " ms" << std::endl;
}

void ElementAssembler::assembleElementForce(Input *pinput, Mesh *pmesh, Dof_Map * pdofmap, Eigen::VectorXd &P)
{
    int num_dofs = pmesh->actual_node_count * 3;
    P.resize(num_dofs);
    P.setZero();
}





void ElementAssembler::elementSetStiffnessAssemble(Input * pinput, 
                                                   Mesh * pmesh,
                                                   Dof_Map * pdofmap,
                                                   std::vector<int> & element_ids,
                                                   BaseElement * pelement,
                                                   std::vector<std::vector<Eigen::Triplet<double>>> & tripletLists)
{
    //openmp 并行设置
    omp_set_num_threads(max_threads);    
    tripletLists.resize(omp_get_max_threads());
    vector<vector<double>> GaussPoint;
    pelement->SetGaussIntegration(GaussPoint);

    #pragma omp parallel for
    for (int element_now = 0; element_now < element_ids.size(); element_now++)
    {
        int element_id = element_ids[element_now];
        int thread_id = omp_get_thread_num();
        auto &tripletList = tripletLists[thread_id];

        int element_location = pmesh->ElementOrderInList[element_id];
        vector<int> node_ids_in_a_element = pmesh->NodesOnElements[element_location - 1];
        // auto elem = new LinearHex8;
        double nodes_coordinates[8][3];
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 3; j++)
            {
                int node_id = node_ids_in_a_element[i];
                int node_location = pmesh->NodeOrderInList[node_id];
                nodes_coordinates[i][j] = pmesh->NodesCoordinate[node_location - 1][j];
            }

        double elementmat[24][24];
        pelement->ComputeStiffness(nodes_coordinates, GaussPoint, elementmat);
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
                        int row = pdofmap->dofmap[(node_ids_in_a_element[i] - 1) * 6 + ii];
                        int col = pdofmap->dofmap[(node_ids_in_a_element[j] - 1) * 6 + jj];
                        tripletList.push_back(Eigen::Triplet<double>(row, col, value));
                    }
                }
            }
        }
        // delete elem;
    }
}