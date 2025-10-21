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

#include "ElementInterFace.h"
#include "input.h"
#include "MeshDataAgent.h"
#include "DofMap.h"
#include "toolbox.h"
#include "ElementStrategy.h"
#include "OmpStrategy.h"

/*
   执行所有需要循环单元的操作，如计算刚度等
*/


class ElementAssembler
{

public:
    ElementAssembler(std::shared_ptr<DataBase> db,
                     std::shared_ptr<MeshDataAgent> mda,
                     std::shared_ptr<DofMap> dofMap) : d_db(db), d_mda(mda), d_dofMap(dofMap) {};
    ~ElementAssembler() {};

    // 基于单元创建自由度
    void createDofbyElements();



private:
    int max_threads = 6;
    std::shared_ptr<DataBase> d_db;
    std::shared_ptr<MeshDataAgent> d_mda;
    std::shared_ptr<DofMap> d_dofMap;
};

#endif