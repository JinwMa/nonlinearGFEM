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
        else if (element_type == "NonLinearHex8")
        {
            pelem = new NonLinearHex8;
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

        auto node_ids_in_a_element = element_data.element_patch;

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
