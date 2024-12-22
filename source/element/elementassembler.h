#ifndef ELEMENTASSEMBLER_H
#define ELEMENTASSEMBLER_H

#include <iostream>
#include <string>
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <omp.h>
#include <chrono>
#include "elementinterface.h"
#include "input.h"
#include "mesh.h"
#include "dof_map.h"
#include "toolbox.h"
class ElementAssembler
{
private:
    int max_threads = 1;

public:
    ElementAssembler() {};
    ~ElementAssembler() {};
    void takeDB(Input *pinput, Mesh *pmesh, Dof_Map *pdofmap);

    void assembleElementStiffness(Input *pinput,
                                  Mesh *pmesh,
                                  Dof_Map *pdofmap,
                                  std::vector<ObjectElementData> &Element_Data,
                                  Eigen::SparseMatrix<double> &Matrix,
                                  ObjectContralParam * contral_param = nullptr);   

    void assembleElementVector(Input *pinput,
                               Mesh *pmesh,
                               Dof_Map *pdofmap,
                               std::vector<ObjectElementData> &Element_Data,
                               std::vector<double> & Element_Force,
                               ObjectContralParam * contral_param = nullptr);

private:

private:
    std::vector<std::string> d_element_list;
    std::vector<std::vector<int>> d_element_sets;

    void assembleGroupElements(Input *pinput,
                               Mesh *pmesh,
                               Dof_Map *pdofmap,
                               std::vector<int> &element_ids,
                               BaseElement *pelement,
                               std::vector<ObjectElementData> &ElementData,
                               std::vector<std::vector<Eigen::Triplet<double>>> &,
                               ObjectContralParam *contral_param = nullptr);

    void assembleAElement(Dof_Map *pdofmap,
                          std::vector<int> &nodes_ids,
                          std::vector<std::string> &dofs,
                          std::vector<double> &elementmat,
                          std::vector<Eigen::Triplet<double>> &local_tripletLists);

    void assembleGroupElementVector(Input *pinput,
                                    Mesh *pmesh,
                                    Dof_Map *pdofmap,
                                    std::vector<int> &element_ids,
                                    BaseElement *pelement,
                                    std::vector<ObjectElementData> &ElementData,
                                    std::vector<double> &Element_Force,
                                    ObjectContralParam *contral_param = nullptr);

    void setLocalVectorToGlobalVector(Dof_Map *pdofmap,
                                     std::vector<int> &nodes_ids,
                                     std::vector<std::string> &dofs,
                                     std::vector<double> & elementvector,
                                     std::vector<double> & ElementVector);
};

#endif