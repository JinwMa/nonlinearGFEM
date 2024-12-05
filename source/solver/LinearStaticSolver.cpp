#include "LinearStaticSolver.h"







void LinearStaticSolver::init(Input * pinput, Mesh * pmesh)
{
    BaseSolver::init(pinput, pmesh);
    d_constraint_manager = std::make_shared<ConstraintManager>();
    d_element_assembler = std::make_shared<ElementAssembler>();
    d_load_manager = std::make_shared<LoadManger>();
    d_dof_map = std::make_shared<Dof_Map>(pmesh);
}

void LinearStaticSolver::takeDB(Input * pinput, std::string & name)
{
}


void LinearStaticSolver::solve(Input *pinput, Mesh *pmesh)
{
    // 构造约束
    std::cout << "building constraint" << std::endl;
    auto start_constraint = std::chrono::high_resolution_clock::now();
    d_constraint_manager->takeDB(pinput, pmesh);
    // 创建自由度映射列表
    std::cout << "building DofMap" << std::endl;    
    auto start_dof_map = std::chrono::high_resolution_clock::now();
    d_dof_map->BuildDofMap(pmesh);
    std::cout << "complete DofMap" << std::endl;
    auto end_dof_map = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_dof_map = end_dof_map - start_dof_map;
    std::cout << "build DofMap time: " << duration_dof_map.count() << " ms" << std::endl;

    // 生成约束矩阵和右端项
    d_C = d_constraint_manager->buildConstrintMatrix(pmesh, d_dof_map.get());
    d_G = d_constraint_manager->buildConstrintForce(pmesh, d_dof_map.get());
    std::cout << "complete constraint" << std::endl;
    auto end_constraint = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_constraint = end_constraint - start_constraint;
    std::cout << "build Constraint time: " << duration_constraint.count() << " ms" << std::endl;

    
    /// 组装刚度矩阵**********************************************************************************************************
    std::cout << "building stiffness" << std::endl;
    d_element_assembler->takeDB(pinput, pmesh, d_dof_map.get()); //读单元列表
    d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K);
    std::cout << "complete the stiffness " << std::endl;

    //组装节点力向量 
    d_load_manager->takeDB(pinput, pmesh, d_dof_map.get());
    d_load_manager->buildLoadForce(pinput, pmesh, d_dof_map.get(), d_P);    


    
    /// 求解线性方程组
    std::cout << "solving the linear equations" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Eigen::VectorXd solution;
    linear_solver(d_K, d_P, d_C, d_G, solution);
    // linear_solver2(K, P, C, G, solution);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "complete the solve" << std::endl;
    std::chrono::duration<double, std::milli> duration = end - start;    // 计算持续时间并转换为毫秒
    std::cout << "solve Ax=b time: " << duration.count() << " ms" << std::endl;
    
    

    // 进行后处理
    Post post("tecplot");
    // 输出网格:
    post.onlymesh(pinput, pmesh);
    // 输出位移场:
    std::vector<double> displacement(solution.data(), solution.data() + d_P.size());
    post.ShowDisplacement(pinput, pmesh, d_dof_map.get(), displacement);
    if (std::getenv("CHECKSOLUTION") != nullptr)post.check_error(pinput, pmesh, d_dof_map.get());
}


