#include "elementassembler.h"
#include <omp.h>

void ElementAssembler::takeDB(Input *pinput, Mesh *pmesh, Dof_Map *pdofmap)
{    
    d_element_list = pinput->getVectorString("element_list");    
    for (auto name : d_element_list)
    {
        std::string element_set_type = pinput->getString(name + "_set_type");
        if (element_set_type == "range")
        {
            std::vector<int> element_ids;
            std::vector<int> range = pinput->getVectorInt(name + "_range");
            if (range.size() != 2)
                toolbox::error("the range of " + name + "_range" + "is given wrong");
            int start = range[0];
            int end = range[1];
            int size = end - start + 1;
            element_ids.resize(size);
            for (int i = 0; i < size; i++)
                element_ids[i] = start + i;

            d_element_sets.push_back(element_ids);
        }
        else
        {
            toolbox::error("not support the type of " + element_set_type + " for element ids");
        }
    }
    
}

void ElementAssembler::assembleElementStiffness(Input *pinput, Mesh *pmesh, Dof_Map *pdofmap,
                                                std::vector<ObjectElementData> & ElementData,
                                                Eigen::SparseMatrix<double> &K,
                                                ObjectContralParam * contral_param)
{
    // 开始计时:
    auto start = std::chrono::high_resolution_clock::now();
    
    if (contral_param != nullptr)
    {
        if ((contral_param->iteration_step == 0 && contral_param->load_step == 0) || contral_param == nullptr)
            K.resize(pdofmap->dof_size, pdofmap->dof_size);
    }
    else
    {
        K.resize(pdofmap->dof_size, pdofmap->dof_size);
    }   

    K.setZero();
    std::vector<Eigen::Triplet<double>> finalTripletList;

    int iloop = 0;

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
        else if (element_type == "LinearHex8Bbar")
        {
            pelem = new LinearHex8Bbar;
        }
        else
        {
            toolbox::error("not supprot this type of element: " + element_type);
        }

        // 读单元参数和设置
        pelem->takeDB(pinput, pmesh, name);
        // std::vector<int> element_ids = pelem->element_ids;
        auto & element_ids = d_element_sets[iloop]; 
        std::vector<std::vector<Eigen::Triplet<double>>> tripletLists;
        assembleGroupElements(pinput, pmesh, pdofmap, element_ids, pelem, ElementData, tripletLists, contral_param);
        delete pelem;
        for (const auto &localList : tripletLists)
        {
            finalTripletList.insert(finalTripletList.end(), localList.begin(), localList.end());
        }

        iloop++;
    }

    // 设置稀疏矩阵
    K.setFromTriplets(finalTripletList.begin(), finalTripletList.end());
    auto end = std::chrono::high_resolution_clock::now();
    // 计算持续时间并转换为毫秒
    std::chrono::duration<double, std::milli> duration = end - start;
    if(std::getenv("CHECKSOLVER") != nullptr)
        std::cout << "assemble stiffness time: " << duration.count() << " ms" << std::endl;
}



void ElementAssembler::assembleGroupElements(Input *pinput,
                                             Mesh *pmesh,
                                             Dof_Map *pdofmap,
                                             std::vector<int> &element_ids,
                                             BaseElement *pelement,
                                             std::vector<ObjectElementData> & ElementData,
                                             std::vector<std::vector<Eigen::Triplet<double>>> &tripletLists,
                                             ObjectContralParam * contral_param)
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

        int element_location = pmesh->d_element_order_in_list[element_id];
        int element_index = element_location - 1;
        auto & element_data = ElementData[element_index];
        pelement->initializeElement(element_data);
        std::vector<double> elementmat;
        pelement->ComputeStiffness(element_data, elementmat, contral_param);

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



void ElementAssembler::assembleElementVector(Input *pinput,
                               Mesh *pmesh,
                               Dof_Map *pdofmap,
                               std::vector<ObjectElementData> &Element_Data,
                               std::vector<double> & Element_Force,
                               ObjectContralParam * contral_param)
{
    int num_all_dofs = pdofmap->dof_size;
    Element_Force.resize(num_all_dofs);
    for (int index = 0; index < num_all_dofs; index++) Element_Force[index] = 0.0;
    int iloop = 0;
    for (auto name : d_element_list)
    {
        BaseElement *pelem;
        vector<vector<double>> GaussPoint;
        std::string element_type = pinput->getString(name + "_type");
        if (element_type == "NonLinearHex8")
        {
            pelem = new NonLinearHex8;
        }
        else
        {
            toolbox::error("not supprot this type of element: " + element_type + "for integration element vector");
        }

        // 读单元参数和设置
        pelem->takeDB(pinput, pmesh, name);
        // std::vector<int> element_ids = pelem->element_ids;
        auto & element_ids = d_element_sets[iloop];
        this->assembleGroupElementVector(pinput, pmesh, pdofmap, element_ids, pelem, Element_Data, Element_Force, contral_param);
        delete pelem;
        iloop++;        
    }    
}

void ElementAssembler::assembleGroupElementVector(Input *pinput,
                                                  Mesh *pmesh,
                                                  Dof_Map *pdofmap,
                                                  std::vector<int> &element_ids,
                                                  BaseElement *pelement,
                                                  std::vector<ObjectElementData> &ElementData,
                                                  std::vector<double> &Element_Force,
                                                  ObjectContralParam *contral_param)
{    
    int num_threads = omp_get_max_threads();
    int num_alldof_size = Element_Force.size();
    std::vector<std::vector<double>> Element_Force_Vectors;
    Element_Force_Vectors.resize(num_threads);
    for (int i = 0; i < num_threads; i++)
    {
        Element_Force_Vectors[i].resize(num_alldof_size);  
    }
    pelement->SetElement();
#pragma omp parallel for
    for (int element_now = 0; element_now < element_ids.size(); element_now++)
    {
        int thread_id = omp_get_thread_num();
        int element_id = element_ids[element_now];
        int element_location = pmesh->d_element_order_in_list[element_id];
        int element_index = element_location - 1;
        auto & element_data = ElementData[element_index];
        pelement->initializeElement(element_data);
        std::vector<double> elementvector;
        pelement->ComputeInternalForce(element_data, elementvector, contral_param);
        auto node_ids_in_a_element = element_data.element_patch;  
        setLocalVectorToGlobalVector(pdofmap, node_ids_in_a_element, pelement->dofs, elementvector, Element_Force_Vectors[thread_id]);
    }    
    for (int i = 0; i < num_threads; i++)
    {
        for (int j = 0; j < num_alldof_size; j++)
        {
            Element_Force[j] += Element_Force_Vectors[i][j];    
        }          
    }
}

void ElementAssembler::setLocalVectorToGlobalVector(Dof_Map *pdofmap,
                                 std::vector<int> &nodes_ids,
                                 std::vector<std::string> &dofs,
                                 std::vector<double> &elementvector,
                                 std::vector<double> &ElementVector)
{
    // toolbox::printvector(elementvector);
    int num_node = nodes_ids.size();
    int num_dof = dofs.size();   
    // std::cout << num_dof << std::endl; 
    for (int i = 0; i < num_node; i++)
    {
        int node_id = nodes_ids[i];
        for (int j = 0; j < num_dof; j++)
        {
            int dof_index_local = i * num_dof + j;
            std::string dof = dofs[j];
            int dof_index_global = pdofmap->getDofIndex(node_id, dof);
            // std::cout << dof_index_global << " " << dof_index_local << std::endl;
            ElementVector[dof_index_global] += elementvector[dof_index_local];
        }
    }
}