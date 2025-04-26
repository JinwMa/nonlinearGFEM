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
#include "ConstraintManager.h"
#include "DofMap.h"
#include "ObjectElementData.h"
#include "ObjectControlParam.h"
#include "ElementAssembler.h"
#include "ObjectSparseFactor.h"
#include "ObjectSparseVector.h"

// 原子求解器基类：只定义成员变量和提供基础实现
class BaseAtomSolver
{
    public:
    BaseAtomSolver(std::shared_ptr<DataBase> solve_db, std::shared_ptr<Mesh> mesh)
    {
        d_solve_db = solve_db;
        d_mesh = mesh;
    }
    virtual ~BaseAtomSolver(){};

    // 成员变量
    std::vector<ElementData> d_elementData;
    std::shared_ptr<DataBase> d_solve_db;
    std::shared_ptr<Mesh> d_mesh;

    // Manager
    std::shared_ptr<ConstraintManager> d_constraint_manger;
    std::shared_ptr<DofMap> d_dof_map;
    std::shared_ptr<ObjectControlParam> d_control_param;
    std::shared_ptr<ElementAssembler> d_elementAssembler;


    // 稀疏矩阵
    std::map<std::string, std::shared_ptr<SparseFactor>> d_Matrixs;
    std::map<std::string, std::shared_ptr<SparseVector>> d_Vectors;

    // 成员函数
    /*
    读取参数
    */
    virtual void takeDB();
    /*
    初始化求解器：
    manager初始化
    数据初始化
    */
    virtual void initializeSolver();
    /*
    载荷步初始化
    根据时间信息更新载荷 约束等信息
    */
    virtual void initializeTimeStep(const double current_time);
    /*
    // 初始化迭代步
    */
    virtual void initializeIterationStep();    
    /*
    一个时间步内的求解过程
    */ 
    virtual void solveOneTimeStep(const double, const double, double &);
    /*
    // 后处理接口
    */
    virtual void afterSolve();                  

};




#endif