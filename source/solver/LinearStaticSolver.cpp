#include <omp.h>
#include <chrono>
#include "LinearStaticSolver.h"
#include "ConstraintManager.h"
#include "elementassembler.h"
#include "LoadManger.h"
#include "post.h"

#include "dof_map.h"

void LinearStaticSolver::takeDB()
{
}
void LinearStaticSolver::solve(Input *pinput, Mesh *pmesh)
{
    Eigen::SparseMatrix<double> C;
    Eigen::SparseMatrix<double> K;
    Eigen::VectorXd G;
    Eigen::VectorXd P;

    // 定义和初始化自由度映射列表
    Dof_Map DofMap(pmesh);
    Dof_Map * pdofmap = &DofMap;    

    // 构造约束
    auto start_constraint = std::chrono::high_resolution_clock::now();
    std::cout << "building constraint" << std::endl;
    auto constraint_manager = new ConstraintManager();
    constraint_manager->takeDB(pinput, pmesh);
    // 创建自由度映射列表
    auto start_dof_map = std::chrono::high_resolution_clock::now();
    std::cout << "building DofMap" << std::endl;    
    pdofmap->BuildDofMap(pmesh);
    std::cout << "complete DofMap" << std::endl;
    auto end_dof_map = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_dof_map = end_dof_map - start_dof_map;
    std::cout << "build DofMap time: " << duration_dof_map.count() << " ms" << std::endl;

    // 生成约束矩阵和右端项
    C = constraint_manager->buildConstrintMatrix(pmesh, pdofmap);
    G = constraint_manager->buildConstrintForce(pmesh, pdofmap);
    delete constraint_manager;
    std::cout << "complete constraint" << std::endl;
    auto end_constraint = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_constraint = end_constraint - start_constraint;
    std::cout << "build Constraint time: " << duration_constraint.count() << " ms" << std::endl;

    
    /// 组装刚度矩阵**********************************************************************************************************
    /*
    *******************************************
    *******************************************
    *******************************************
    */
    auto * pelementassembler = new ElementAssembler;
    pelementassembler->takeDB(pinput, pmesh, pdofmap); //读单元列表
    // 构造刚度矩阵,构造右端项
    std::cout << "building stiffness" << std::endl;
    pelementassembler->assembleElementStiffness(pinput, pmesh, pdofmap, K);
    std::cout << "complete the stiffness " << std::endl;
    // 节点内力向量   
    pelementassembler->assembleElementForce(pinput, pmesh, pdofmap, P);
    //释放组装器指针
    delete pelementassembler;


    /// TODO:组装节点力向量 

    auto loadmanger = new LoadManger;

    loadmanger->takeDB(pinput, pmesh, pdofmap);
    
    delete loadmanger;
    exit(0);


    /// 求解线性方程组
    std::cout << "solving the linear equations" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Eigen::VectorXd solution;
    linear_solver(K, P, C, G, solution);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "complete the solve" << std::endl;
    // 计算持续时间并转换为毫秒
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "solve Ax=b time: " << duration.count() << " ms" << std::endl;
    
    

    // 进行后处理
    Post post("tecplot");
    // 输出网格:
    post.onlymesh(pinput, pmesh);
    // 输出位移场:
    std::vector<double> displacement(solution.data(), solution.data() + P.size());
    post.ShowDisplacement(pinput, pmesh, pdofmap, displacement);
    if (std::getenv("CHECKSOLUTION") != nullptr)post.check_error(pinput, pmesh, pdofmap);
}


