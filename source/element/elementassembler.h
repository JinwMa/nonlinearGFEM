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
    int max_threads = 6;
    public:
    ElementAssembler(){};
    ~ElementAssembler(){};
    void takeDB(Input * pinput, Mesh * pmesh, Dof_Map * pdofmap);

    std::vector<std::string> elementList;
    std::vector<std::string> elementType;

    void assembleElementStiffness(Input * pinput,
                                  Mesh * pmesh,
                                  Dof_Map * pdofmap, 
                                  Eigen::SparseMatrix<double> & Matrix);

    
    
    
    void assembleElementForce(Input * pinput,
                              Mesh * pmesh,
                              Dof_Map * pdofmap, 
                              Eigen::VectorXd & P);


     
    void elementSetStiffnessAssemble(Input * pinput,   
                                Mesh * pmesh,
                                Dof_Map * pdofmap,
                                std::vector<int> & element_ids,
                                BaseElement * pelement,
                                std::vector<std::vector<Eigen::Triplet<double>>> &);



    void assembleAElement(Dof_Map * pdofmap,
                          std::vector<int> & nodes_ids,
                          std::vector<std::string> & dofs,
                          std::vector<double> & elementmat,
                          std::vector<Eigen::Triplet<double>> & local_tripletLists);

};



#endif