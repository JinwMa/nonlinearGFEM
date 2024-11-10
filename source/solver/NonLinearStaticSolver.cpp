#include "NonLinearStaticSolver.h"


void NonLinearStaticSolver::initialize_solver(Input * pinput, Mesh * pmesh)
{
    int elements_num = pmesh->actual_element_count;
    d_elements_data.resize(elements_num);
    for (int i = 0; i < elements_num; i++)
    {
        d_elements_data[i].if_element_is_initialized = false;
    }
}


void NonLinearStaticSolver::solve(Input * pinput, Mesh * pmesh)
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

    
    int dof_size = pdofmap->dof_size;
    U.resize(dof_size);
    dU.resize(dof_size);
    ddU.resize(dof_size);


    initialize_solver(pinput, pmesh);
    auto * pelementassembler = new ElementAssembler;
    pelementassembler->takeDB(pinput, pmesh, pdofmap); //读单元列表
    // 构造刚度矩阵,构造右端项
    std::cout << "building stiffness" << std::endl;
    pelementassembler->assembleNonLinearElementStiffness(pinput, pmesh, pdofmap, d_elements_data, U, dU, ddU, K);
    std::cout << "complete the stiffness " << std::endl;   
    //释放组装器指针
    delete pelementassembler;

    auto loadmanger = new LoadManger;
    loadmanger->takeDB(pinput, pmesh, pdofmap);
    loadmanger->buildLoadForce(pinput, pmesh, pdofmap, P);    
    delete loadmanger;

    /// 求解线性方程组
    std::cout << "solving the linear equations" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Eigen::VectorXd solution;
    linear_solver(K, P, C, G, solution);
    // linear_solver2(K, P, C, G, solution);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "complete the solve" << std::endl;
    // 计算持续时间并转换为毫秒
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "solve Ax=b time: " << duration.count() << " ms" << std::endl;

    std::vector<double> displacement(solution.data(), solution.data() + P.size());


    
    // 进行后处理
    Post post("tecplot");
    // 输出网格:
    post.onlymesh(pinput, pmesh);
    // 输出位移场:
    post.ShowDisplacement(pinput, pmesh, pdofmap, displacement);
    if (std::getenv("CHECKSOLUTION") != nullptr)post.check_error(pinput, pmesh, pdofmap);




}