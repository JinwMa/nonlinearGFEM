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

#include "LinearHex8.h"
#include "input.h"
#include "mesh.h"
#include "dof_map.h"
class ElementAssembler
{
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

};



#endif