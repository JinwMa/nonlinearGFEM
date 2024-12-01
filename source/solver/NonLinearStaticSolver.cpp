#include "NonLinearStaticSolver.h"


void NonLinearStaticSolver::init(Input * pinput, Mesh * pmesh)
{
    BaseSolver::init(pinput, pmesh);
    d_constraint_manager = std::make_shared<ConstraintManager>();
    d_element_assembler = std::make_shared<ElementAssembler>();
    d_load_manager = std::make_shared<LoadManger>();
    d_dof_map = std::make_shared<Dof_Map>(pmesh);
    d_post = std::make_shared<Post>("tecplot");


    initData(pinput, pmesh);
}


void NonLinearStaticSolver::initData(Input * pinput, Mesh * pmesh)
{
    // 构造约束
    auto start_constraint = std::chrono::high_resolution_clock::now();
    std::cout << "building constraint" << std::endl;
    d_constraint_manager->takeDB(pinput, pmesh);
    // 创建自由度映射列表
    auto start_dof_map = std::chrono::high_resolution_clock::now();
    std::cout << "building DofMap" << std::endl;    
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

    
    int dof_size = d_dof_map->dof_size;
    d_u.resize(dof_size);
    d_du.resize(dof_size);
    d_ddu.resize(dof_size);
    int num_constrain_equations = d_C.rows();
    d_lambda.resize(num_constrain_equations);
    d_dlambda.resize(num_constrain_equations);
    d_ddlambda.resize(num_constrain_equations);

    d_element_assembler->takeDB(pinput, pmesh, d_dof_map.get()); //读单元列表
    // 构造刚度矩阵,构造右端项
    std::cout << "building stiffness" << std::endl;
    // d_element_assembler->assembleNonLinearElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_u, d_du, d_ddu, d_K);
    d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K, d_contral_param.get());

    std::cout << "complete the stiffness " << std::endl; 

    d_load_manager->takeDB(pinput, pmesh, d_dof_map.get());
    d_load_manager->buildLoadForce(pinput, pmesh, d_dof_map.get(), d_P);    

}

void NonLinearStaticSolver::solve(Input * pinput, Mesh * pmesh)
{

    d_u.clear();
    d_du.clear();
    d_ddu.clear();
    d_lambda.clear();
    d_dlambda.clear();
    d_ddlambda.clear();
    while(true)
    {
        d_contral_param->load_step++;    
        d_contral_param->iteration_step = 0;      
        d_du.clear();
        d_ddlambda.clear();
        while(true)
        {
            d_contral_param->iteration_step++;
            d_ddu.clear();
            d_ddlambda.clear();

            

            Eigen::VectorXd solution;
            linear_solver(d_K, d_P, d_C, d_G, solution);
            std::copy(solution.data(), solution.data() + solution.size(); d_ddu.begin());

            for (int index = 0; index < d_dof_map->dof_size; index++) d_du[index] += d_ddu[index];
            setVectorToElementData(d_du, d_dof_map.get(), d_element_data, "du");

            d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K, d_contral_param.get());
            d_element_assembler->assembleElementVector(pinput, pmesh, d_dof_map.get(), d_element_data, d_internal_force, d_contral_param.get());
            Eigen::VectorXd rhs = Eigen::Map<Eigen::VectorXd>(d_internal_force.data(), d_internal_force.size());
            Eigen::SparseMatrix<double> CT = d_C.transpose();
            Eigen::VectorXd dlambda = solution.segment(displacement.size(), d_C.rows());
            Eigen::VectorXd rhs_lambda = CT * dlambda;
            rhs = d_P - rhs - rhs_lambda;
            linear_solver(d_K, rhs, d_C, d_G, solution);
        }        
    }

    /// 求解线性方程组
    std::cout << "solving the linear equations" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    Eigen::VectorXd solution;
    linear_solver(d_K, d_P, d_C, d_G, solution);
    // linear_solver2(K, P, C, G, solution);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "complete the solve" << std::endl;
    // 计算持续时间并转换为毫秒
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "solve Ax=b time: " << duration.count() << " ms" << std::endl;

    std::vector<double> displacement(solution.data(), solution.data() + d_P.size());
    
    // 进行后处理
    d_post->onlymesh(pinput, pmesh);
    // 输出位移场:
    d_post->ShowDisplacement(pinput, pmesh, d_dof_map.get(), displacement);
    if (std::getenv("CHECKSOLUTION") != nullptr)d_post->check_error(pinput, pmesh, d_dof_map.get());


    d_contral_param->iteration_step++;    
    setVectorToElementData(displacement, d_dof_map.get(), d_element_data, "u");

    d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K, d_contral_param.get());

    d_element_assembler->assembleElementVector(pinput, pmesh, d_dof_map.get(), d_element_data, d_internal_force, d_contral_param.get());


    Eigen::VectorXd rhs = Eigen::Map<Eigen::VectorXd>(d_internal_force.data(), d_internal_force.size());
    Eigen::SparseMatrix<double> CT = d_C.transpose();
    Eigen::VectorXd dlambda = solution.segment(displacement.size(), d_C.rows());
    Eigen::VectorXd rhs_lambda = CT * dlambda;    
    rhs = d_P - rhs - rhs_lambda;
    linear_solver(d_K, rhs, d_C, d_G, solution);

    std::vector<double> displacement2(solution.data(), solution.data() + rhs.size());
    // 进行后处理
    Post post2("tecplot2");
    // 输出网格:
    post2.onlymesh(pinput, pmesh);
    // 输出位移场:
    post2.ShowDisplacement(pinput, pmesh, d_dof_map.get(), displacement2);
    if (std::getenv("CHECKSOLUTION") != nullptr)post2.check_error(pinput, pmesh, d_dof_map.get());



   


}