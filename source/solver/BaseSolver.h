#ifndef BASESOLVER_H
#define BASESOLVER_H

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



class BaseSolver
{
    public:
    BaseSolver(std::shared_ptr<DataBase> solve_db, std::shared_ptr<Mesh> mesh)
    {
        d_solve_db = solve_db;
        d_mesh = mesh;
    }
    ~BaseSolver();


    
    void takeDB();
    void initializeSolver();
    void initializeLoadStep();
    void initializeIterationStep();

    void solve();
    void solveOneTimeStep();
    void solveOneIterationStep();


    //input && mesh
    std::shared_ptr<DataBase> d_solve_db;
    std::shared_ptr<Mesh> d_mesh;

    //Timer
    double d_predict_dt; //预测时间步长
    double d_actural_dt; //真实时间步长
    double d_last_time;  //上一步时间
    double d_current_time; //当前时间
    double d_next_time;  //下一步时间
    double d_growth_size; //载荷步缩放比例

    //Manger
};

#endif