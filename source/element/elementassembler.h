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
                                  Eigen::SparseMatrix<double> &Matrix);

    void assembleNonLinearElementStiffness(Input *pinput,
                                           Mesh *pmesh,
                                           Dof_Map *pdofmap,
                                           std::vector<ObjectElement> &ElementData,
                                           std::vector<double> &U,
                                           std::vector<double> &dU,
                                           std::vector<double> &ddU,
                                           Eigen::SparseMatrix<double> &Matrix);
    

    void assembleGroupElements(Input *pinput,
                               Mesh *pmesh,
                               Dof_Map *pdofmap,
                               std::vector<int> &element_ids,
                               BaseElement *pelement,
                               std::vector<ObjectElementData> & ElementData,
                               std::vector<std::vector<Eigen::Triplet<double>>> &);

    void assembleAElement(Dof_Map *pdofmap,
                          std::vector<int> &nodes_ids,
                          std::vector<std::string> &dofs,
                          std::vector<double> &elementmat,
                          std::vector<Eigen::Triplet<double>> &local_tripletLists);

    void NonLinearElementSetStiffnessAssemble(Input *pinput,
                                              Mesh *pmesh,
                                              Dof_Map *pdofmap,
                                              std::vector<int> &element_ids,
                                              BaseElement *pelement,
                                              std::vector<ObjectElement> &ElementData,
                                              std::vector<double> &U,
                                              std::vector<double> &dU,
                                              std::vector<double> &ddU,
                                              std::vector<std::vector<Eigen::Triplet<double>>> &);

private:
    std::vector<std::string> d_element_list;
};

#endif