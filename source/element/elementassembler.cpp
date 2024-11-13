#include "elementassembler.h"
#include <omp.h>

void ElementAssembler::takeDB(Input *pinput, Mesh *pmesh, Dof_Map *pdofmap)
{
    d_element_list = pinput->getVectorString("element_list");
}

void ElementAssembler::assembleElementStiffness(Input *pinput, Mesh *pmesh, Dof_Map *pdofmap,
                                                std::vector<ObjectElementData> & ElementData,
                                                Eigen::SparseMatrix<double> &K)
{
    // 开始计时:
    auto start = std::chrono::high_resolution_clock::now();
    K.resize(pdofmap->dof_size, pdofmap->dof_size);
    K.setZero();
    std::vector<Eigen::Triplet<double>> finalTripletList;

    for (auto name : d_element_list)
    {
        BaseElement *pelem;
        vector<vector<double>> GaussPoint;
        std::string element_type = pinput->getString(name + "_type");
        if (element_type == "LinearHex8")
        {
            pelem = new LinearHex8;
        }
        else if (element_type == "LinearTet4")
        {
            pelem = new LinearTet4;
        }
        else
        {
            toolbox::error("not supprot this type of element: " + element_type);
        }

        // 读单元参数和设置
        pelem->takeDB(pinput, pmesh, name);
        std::vector<int> element_ids = pelem->element_ids;
        std::vector<std::vector<Eigen::Triplet<double>>> tripletLists;
        assembleGroupElements(pinput, pmesh, pdofmap, element_ids, pelem, ElementData, tripletLists);
        delete pelem;
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



void ElementAssembler::assembleGroupElements(Input *pinput,
                                             Mesh *pmesh,
                                             Dof_Map *pdofmap,
                                             std::vector<int> &element_ids,
                                             BaseElement *pelement,
                                             std::vector<ObjectElementData> & ElementData,
                                             std::vector<std::vector<Eigen::Triplet<double>>> &tripletLists)
{
    // openmp 并行设置
    omp_set_num_threads(max_threads);
    tripletLists.resize(omp_get_max_threads());
    pelement->SetElement();

#pragma omp parallel for
    for (int element_now = 0; element_now < element_ids.size(); element_now++)
    {
        int element_id = element_ids[element_now];
        int thread_id = omp_get_thread_num();
        auto &tripletList = tripletLists[thread_id];

        int element_location = pmesh->ElementOrderInList[element_id];
        int element_index = element_location - 1;
        auto & element_data = ElementData[element_index];
        std::vector<double> elementmat;
        pelement->ComputeStiffness(element_data, elementmat);

        auto node_ids_in_a_element = element_data.node_ids;

        // 单刚组装
        std::vector<Eigen::Triplet<double>> local_tripletLists;
        assembleAElement(pdofmap, node_ids_in_a_element, pelement->dofs, elementmat, local_tripletLists);
        tripletList.insert(tripletList.end(), local_tripletLists.begin(), local_tripletLists.end());
    }
}

void ElementAssembler::assembleAElement(Dof_Map *pdofmap,
                                        std::vector<int> &nodes_ids,
                                        std::vector<std::string> &dofs,
                                        std::vector<double> &elementmat,
                                        std::vector<Eigen::Triplet<double>> &local_tripletLists)
{
    local_tripletLists.resize(nodes_ids.size() * dofs.size() * nodes_ids.size() * dofs.size());
    int numNode = nodes_ids.size();
    int numdof = dofs.size();

    int index = 0;
    for (int i = 0; i < numNode; i++)
    {
        for (int j = 0; j < numNode; j++)
        {
            for (int ii = 0; ii < numdof; ii++)
            {
                for (int jj = 0; jj < numdof; jj++)
                {
                    int iii = i * numdof + ii;
                    int jjj = j * numdof + jj;
                    double value = elementmat[iii * numNode * numdof + jjj];
                    int row = pdofmap->dofmap[(nodes_ids[i] - 1) * 6 + ii];
                    int col = pdofmap->dofmap[(nodes_ids[j] - 1) * 6 + jj];
                    local_tripletLists[index] = Eigen::Triplet<double>(row, col, value);
                    index++;
                }
            }
        }
    }
}

void ElementAssembler::assembleNonLinearElementStiffness(Input *pinput, Mesh *pmesh, Dof_Map *pdofmap,
                                                         std::vector<ObjectElement> &ElementData,
                                                         std::vector<double> & U,
                                                         std::vector<double> & dU,
                                                         std::vector<double> & ddU,
                                                         Eigen::SparseMatrix<double> &K)
{
    //开始计时:
    auto start = std::chrono::high_resolution_clock::now();
    K.resize(pdofmap->dof_size, pdofmap->dof_size);
    K.setZero();
    std::vector<Eigen::Triplet<double>> finalTripletList;

    for (auto name : d_element_list)
    {
        BaseElement *elem;
        vector<vector<double>> GaussPoint;
        std::string element_type = pinput->getString(name + "_type");
        if (element_type == "NonLinearHex8")
        {
            elem = new NonLinearHex8;
        }
        else
        {
            toolbox::error("not supprot this type of element: " + element_type);
        }

        // 读单元参数和设置
        elem->takeDB(pinput, pmesh, name);
        std::vector<int> element_ids = elem->element_ids;
        std::vector<std::vector<Eigen::Triplet<double>>> tripletLists;
        NonLinearElementSetStiffnessAssemble(pinput, pmesh, pdofmap,
                                             element_ids, elem,
                                             ElementData,
                                             U, dU, ddU,
                                             tripletLists);
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

void ElementAssembler::NonLinearElementSetStiffnessAssemble(Input *pinput,
                                                            Mesh *pmesh,
                                                            Dof_Map *pdofmap,
                                                            std::vector<int> &element_ids,
                                                            BaseElement *pelement,
                                                            std::vector<ObjectElement> &ElementData,
                                                            std::vector<double> &U,
                                                            std::vector<double> &dU,
                                                            std::vector<double> &ddU,
                                                            std::vector<std::vector<Eigen::Triplet<double>>> &tripletLists)
{
    //openmp 并行设置
    omp_set_num_threads(max_threads);
    tripletLists.resize(omp_get_max_threads());
    pelement->SetElement();

#pragma omp parallel for
    for (int element_now = 0; element_now < element_ids.size(); element_now++)
    {
        int element_id = element_ids[element_now];
        int thread_id = omp_get_thread_num();
        auto &tripletList = tripletLists[thread_id];

        int element_location = pmesh->ElementOrderInList[element_id];
        // printf("Thread ID: %d, Element ID: %d, element_location: %d\n", thread_id, element_id, element_location);
        // 拿到节点编号
        vector<int> node_ids_in_a_element = pmesh->NodesOnElements[element_location - 1];
        double nodes_coordinates[20][3] = {0.0, 0.0};

        // 拿到节点坐标
        for (int i = 0; i < pelement->numNodes; i++)
            for (int j = 0; j < pelement->dim; j++)
            {
                int node_id = node_ids_in_a_element[i];
                int node_location = pmesh->NodeOrderInList[node_id];
                nodes_coordinates[i][j] = pmesh->NodesCoordinate[node_location - 1][j];
            }

        // 计算单元矩阵
        std::vector<double> elementmat;
        std::vector<double> u;
        std::vector<double> du;
        std::vector<double> ddu;
        ObjectElement &elementdata = ElementData[element_location - 1];
        #pragma omp critical
        {
            // std::cout << "Thread " << thread_id << " is printing safely.\n";
            // pelement->ComputeStiffness(nodes_coordinates, u, du, ddu, elementdata, elementmat);
        }
        pelement->ComputeStiffness(nodes_coordinates, u, du, ddu, elementdata, elementmat);
        #pragma omp critical
        {
            // std::cout << element_id << std::endl;
            // for (int i = 0; i < elementmat.size(); i++)
            //    std::cout << elementmat[i] << "   ";
            // // pelement->ComputeStiffness(nodes_coordinates, u, du, ddu, elementdata, elementmat);
            // std::cout << std::endl;
        }
        // 单刚组装
        std::vector<Eigen::Triplet<double>> local_tripletLists;
        assembleAElement(pdofmap, node_ids_in_a_element, pelement->dofs, elementmat, local_tripletLists);
        tripletList.insert(tripletList.end(), local_tripletLists.begin(), local_tripletLists.end());
    }
}