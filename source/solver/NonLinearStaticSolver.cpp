#include "NonLinearStaticSolver.h"


void NonLinearStaticSolver::takeDB(Input * pinput, const std::string & name)
{
    if (pinput->ifExist(name + "_load_steps")) d_num_load_step = pinput->getInt(name + "_load_steps");
    std::cout << "number of load steps is " << d_num_load_step << std::endl;
}

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
    d_internal_force.resize(dof_size);
    d_rhs.resize(dof_size);

    d_u.setZero();
    d_du.setZero();
    d_ddu.setZero();
    d_lambda.setZero();

    d_internal_force.setZero();
    d_rhs.setZero();



    // 构造刚度矩阵,构造右端项
    d_element_assembler->takeDB(pinput, pmesh, d_dof_map.get()); //读单元列表
    std::cout << "building stiffness" << std::endl;
    // d_element_assembler->assembleNonLinearElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_u, d_du, d_ddu, d_K);
    d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K, d_contral_param.get());

    std::cout << "complete the stiffness " << std::endl; 

    d_load_manager->takeDB(pinput, pmesh, d_dof_map.get());
    d_load_manager->buildLoadForce(pinput, pmesh, d_dof_map.get(), d_P);    
    d_dP = d_P / d_num_load_step;
    d_dG = d_G / d_num_load_step;
    d_rhs_G.resize(d_G.size());
}

void NonLinearStaticSolver::solve(Input * pinput, Mesh * pmesh)
{
    
    int ii = 0;
    while(ii < d_num_load_step)
    {
        std::cout << "++++++++++++++++++ load step" << ii + 1 << "++++++++"<< std::endl;   
        d_contral_param->load_step++;    
        d_contral_param->iteration_step = 0;
        d_du.setZero();
        d_P = d_dP * (ii + 1);
        d_G = d_dG * (ii + 1);
        while(true)
        {
            d_contral_param->iteration_step++;
            d_ddu.setZero();                        
            d_rhs = d_P - d_internal_force; 
            d_rhs_G = d_G - d_C * (d_u + d_du);
            std::cout << "    ------- iteration " << d_contral_param->iteration_step << std::endl;;           
            if(checkConvergence()) break;            
            Eigen::VectorXd solution;
            linear_solver(d_K, d_rhs, d_C, d_rhs_G, solution);
            d_ddu = solution.head(d_ddu.size());
            d_lambda = solution.tail(d_lambda.size());
            d_du = d_du + d_ddu;
            std::vector<double> temp(d_du.data(), d_du.data() + d_du.size());
            setVectorToElementData(temp, d_dof_map.get(), d_element_data, "du");           
            d_element_assembler->assembleElementStiffness(pinput, pmesh,
                                                          d_dof_map.get(),
                                                          d_element_data, 
                                                          d_K,
                                                          d_contral_param.get());

            d_element_assembler->assembleElementVector(pinput, 
                                                       pmesh, 
                                                       d_dof_map.get(), 
                                                       d_element_data, 
                                                       temp, 
                                                       d_contral_param.get());

            Eigen::VectorXd temp_eigen = Eigen::Map<Eigen::VectorXd>(temp.data(), temp.size());
            d_internal_force = temp_eigen;
        }   
        d_u = d_u + d_du;     
        std::vector<double> temp(d_u.data(), d_u.data() + d_u.size());
        setVectorToElementData(temp, d_dof_map.get(), d_element_data, "u");
        d_post->ShowDisplacementOnDeformedConfigration(pinput, pmesh, d_dof_map.get(), temp);
        ii++;
    }

}


bool NonLinearStaticSolver::checkConvergence()
{
    //判断收敛需要执行两条标准：力和约束
    bool force_convergence = false;
    bool constraint_convergence = false;

    double eps = 1.E-8;
    Eigen::SparseMatrix<double> Ct = d_C.transpose(); 
    Eigen::VectorXd temp = d_rhs - Ct * d_lambda;
    double normal_rhs = toolbox::getEigenVectorNormal(temp);
    double normal_P = toolbox:: getEigenVectorNormal(d_P);
    if (normal_P < eps)
    {
        std::cout << "\t" << " the iteration error of force is " << normal_rhs << std::endl;
        if (normal_rhs < eps * 1.e2) force_convergence = true;
    }
    else 
    {
        std::cout << "\t" << " the iteration error of force is " << normal_rhs / normal_P << std::endl;
        if (normal_rhs / normal_P < eps)
            force_convergence = true;
    }

    double normal_rhs_G = toolbox::getEigenVectorNormal(d_rhs_G);
    std::cout << "\t" << " the iteration error of constrain is " << normal_rhs_G << std::endl;
    if (normal_rhs_G < eps) constraint_convergence = true;

    return constraint_convergence && force_convergence;

}