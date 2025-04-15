#ifndef BASEFLOWSOLVER_H
#define BASEFLOWSOLVER_H

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

#include "BaseAtomSolver.h"



class BaseFlowSolver
{
    public:
    BaseFlowSolver(std::shared_ptr<DataBase> solve_db, std::shared_ptr<Mesh> mesh)
    {
        d_solve_db = solve_db;
        d_mesh = mesh;
    }
    virtual ~BaseFlowSolver();


    // 成员变量：
    //input && mesh
    std::shared_ptr<DataBase> d_solve_db;
    std::shared_ptr<Mesh> d_mesh;
    //Timer
    double d_predict_dt; //预测时间步长
    double d_actural_dt; //真实时间步长
    double d_last_time;  //上一步时间
    double d_current_time; //当前时间
    double d_next_time;  //下一步时间
    double d_growth_size; //时间步缩放比例

    bool d_continue_flag = true;  //是否停止迭代
    std::shared_ptr<BaseAtomSolver> d_atom_solver;


    virtual void init();
    virtual void solve();
        
    virtual void takeDB();
    virtual void initializeSolver();
    virtual void initializeTimeStep();

    virtual void inOneTimeStep();
    virtual void timePassOn();


    

    //Manger
};

#endif