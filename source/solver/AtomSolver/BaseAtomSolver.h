#ifndef BASEATOMSOLVER_H
#define BASEATOMSOLVER_H

#include <string>
#include <iostream>
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <omp.h>
#include <chrono>
#include <sstream>
#include <map>
#include <unordered_set>
#include <thread>
#include <Eigen/IterativeLinearSolvers>

#include "input.h"
#include "mesh.h"
#include "DofMap.h"
class BaseAtomSolver
{
    public:
    BaseAtomSolver(std::shared_ptr<DataBase> solve_db, std::shared_ptr<Mesh> mesh)
    {
        d_solve_db = solve_db;
        d_mesh = mesh;
    }
    virtual ~BaseAtomSolver();

    // 成员变量
    std::shared_ptr<DataBase> d_solve_db;
    std::shared_ptr<Mesh> d_mesh;

    // 成员函数
    virtual void takeDB();
    virtual void initializeSolver();
    virtual void initializeIterationStep();     // 初始化迭代步
    virtual void solveOneTimeStep();

    virtual void afterSolve();                  // 后处理接口

};




#endif