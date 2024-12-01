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

    d_internal_force.resize(dof_size);
    d_rhs.resize(dof_size);



    // 构造刚度矩阵,构造右端项
    d_element_assembler->takeDB(pinput, pmesh, d_dof_map.get()); //读单元列表
    std::cout << "building stiffness" << std::endl;
    // d_element_assembler->assembleNonLinearElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_u, d_du, d_ddu, d_K);
    d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K, d_contral_param.get());

    std::cout << "complete the stiffness " << std::endl; 

    d_load_manager->takeDB(pinput, pmesh, d_dof_map.get());
    d_load_manager->buildLoadForce(pinput, pmesh, d_dof_map.get(), d_P);    

}

void NonLinearStaticSolver::solve(Input * pinput, Mesh * pmesh)
{
    for (int index = 0; index < d_dof_map->dof_size; index++)
    {
        d_u[index] = 0.0;
        d_du[index] = 0.0;
        d_ddu[index] = 0.0;
    }


    while(true)
    {
        d_contral_param->load_step++;    
        d_contral_param->iteration_step = 0;
        for (int index = 0; index < d_dof_map->dof_size; index++)
        {
            d_du[index] = 0.0;
        }
        while(true)
        {
            d_contral_param->iteration_step++;
            for (int index = 0; index < d_dof_map->dof_size; index++)
            {
                d_ddu[index] = 0.0;
            }
            Eigen::VectorXd rhs = Eigen::Map<Eigen::VectorXd>(d_internal_force.data(), d_internal_force.size());
            rhs = d_P - rhs;
            Eigen::VectorXd solution;
            linear_solver(d_K, rhs, d_C, d_G, solution);

            for (int index = 0; index < d_dof_map->dof_size; index++) d_ddu[index] = solution[index];

            for (int index = 0; index < d_dof_map->dof_size; index++) d_du[index] += d_ddu[index];
            setVectorToElementData(d_du, d_dof_map.get(), d_element_data, "du");           
            d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K, d_contral_param.get());

            d_element_assembler->assembleElementVector(pinput, pmesh, d_dof_map.get(), d_element_data, d_internal_force, d_contral_param.get());
        }        
    }

}