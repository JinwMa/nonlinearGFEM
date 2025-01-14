#include "NonLinearStaticSolver.h"


void NonLinearStaticSolver::takeDB(Input * pinput, const std::string & name)
{
    if (pinput->ifExist(name + "_load_steps")) d_num_load_step = pinput->getInt(name + "_load_steps");
    std::cout << "number of load steps is " << d_num_load_step << std::endl;
    if (pinput->ifExist(name + "_step_growth_size")) d_growth_size = pinput->getDouble(name + "_step_growth_size");

    if (pinput->ifExist(name + "_eps")) d_eps = pinput->getDouble(name + "_eps");
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
    // 初始化约束
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
    d_rhs_G.resize(num_constrain_equations); 

    d_u.setZero();
    d_du.setZero();
    d_ddu.setZero();
    d_lambda.setZero();

    d_internal_force.setZero();
    d_rhs.setZero();



    // 初始化刚度矩阵,初始化右端项
    d_element_assembler->takeDB(pinput, pmesh, d_dof_map.get()); //读单元列表
    std::cout << "building stiffness" << std::endl;
    // d_element_assembler->assembleNonLinearElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_u, d_du, d_ddu, d_K);
    d_element_assembler->assembleElementStiffness(pinput, pmesh, d_dof_map.get(), d_element_data, d_K, d_contral_param.get());

    std::cout << "complete the stiffness " << std::endl; 
    d_load_manager->takeDB(pinput, pmesh, d_dof_map.get());
    d_load_manager->buildLoadForce(pinput, pmesh, d_dof_map.get(), d_P);      


    //adaptive load step
    d_real_time = 0.0;
    d_actural_dt = 0.0;
    d_trial_time = 0.0;
    d_predict_dt = 1.0 / d_num_load_step;
}

void NonLinearStaticSolver::solve(Input * pinput, Mesh * pmesh)
{
    if (std::getenv("ONLY_SHOW_MESH"))
    {
        d_post->onlymesh(pinput, pmesh);
        toolbox::error("only show mesh is process");
    }
    while(d_real_time < 1.0 - 1.e-10)
    {
        initializeLoadStep();
        while(true)
        {
            initializeInterationStep(); 
            d_contral_param->iteration_step++;
            d_ddu.setZero();
            std::cout << "    ------- iteration " << d_contral_param->iteration_step;  

            d_rhs = d_P_trial - d_internal_force;
            d_rhs_G = d_G_trial - d_C * (d_u + d_du);
            d_convergence_state = checkConvergence();           
            if(d_convergence_state)
            {
                break;                 
            }     
               
            Eigen::VectorXd solution;
            linear_solver(d_K, d_rhs, d_C, d_rhs_G, solution);
            d_ddu = solution.head(d_ddu.size());
            d_lambda = solution.tail(d_lambda.size());
            d_du = d_du + d_ddu;

            // std::vector<double> temp2(d_du.data(), d_du.data() + d_du.size());
            // d_post->ShowDisplacementOnDeformedConfigration(pinput, pmesh, d_dof_map.get(), temp2);

            setEigenVectorToElementData(d_du, d_dof_map.get(), d_element_data, "du"); 
            

            //更新刚度
            d_element_assembler->assembleElementStiffness(pinput, pmesh,
                                                          d_dof_map.get(),
                                                          d_element_data, 
                                                          d_K,
                                                          d_contral_param.get());

            // 更新内力
            std::vector<double> temp;
            d_element_assembler->assembleElementVector(pinput, 
                                                       pmesh, 
                                                       d_dof_map.get(), 
                                                       d_element_data, 
                                                       temp, 
                                                       d_contral_param.get());
            Eigen::VectorXd temp_eigen = Eigen::Map<Eigen::VectorXd>(temp.data(), temp.size());
            d_internal_force = temp_eigen;
            //结束内力更新                       
        } 
        dealWithConvergenceStatus(pinput, pmesh);        
    }

}


int NonLinearStaticSolver::checkConvergence()
{
    //判断收敛需要执行两条标准：力和约束
    bool force_convergence = false;
    bool constraint_convergence = false;

    double eps = 1.E-8;
    if (d_eps > 1.E-20) eps = d_eps;
    Eigen::SparseMatrix<double> Ct = d_C.transpose(); 
    Eigen::VectorXd temp = d_rhs - Ct * d_lambda;
    double normal_rhs = toolbox::getEigenVectorNormal(temp);
    double normal_P = toolbox:: getEigenVectorNormal(d_P_trial);
    if (normal_P < eps)
    {
        std::cout << "\t" << " error = " << normal_rhs << std::endl;
        if (normal_rhs < eps * 1.e2) force_convergence = true;
    }
    else 
    {
        std::cout << "\t" << " the iteration error of force is " << normal_rhs / normal_P << std::endl;
        if (normal_rhs / normal_P < eps)
            force_convergence = true;
    }

    double normal_rhs_G = toolbox::getEigenVectorNormal(d_rhs_G);
    // std::cout << "\t" << " the iteration error of constrain is " << normal_rhs_G << std::endl;
    if (normal_rhs_G < eps) constraint_convergence = true;

    if (constraint_convergence && force_convergence)
    {
        if (d_contral_param->iteration_step < 3)
            return 2;
        else
            return 1;
    }
    else if (d_contral_param->iteration_step > 15)
        return 3;
    else
        return 0;
}

void NonLinearStaticSolver::updateElementData()
{
    setEigenVectorToElementData(d_u, d_dof_map.get(), d_element_data, "u");
    for (auto & element_data : d_element_data)
    {
        // 更新等效塑性应变
        element_data.eff_p_strain_n = element_data.eff_p_strain_n1;
        // 更新变形梯度和逆
        element_data.F_n = element_data.F_n1;
        element_data.Finv_n = element_data.Finv_n1;
        // 更新雅可比
        element_data.jkb_n = element_data.jkb_n1;
        // 更新应力状态
        element_data.stress_n = element_data.stress_n1;
        if(element_data.centroid_sfdxy.size() != 0)
        {
            element_data.centroid_F_n = element_data.centroid_F_n1;
            element_data.centroid_Finv_n = element_data.centroid_Finv_n1;
        }
    }
}

void NonLinearStaticSolver::dealWithConvergenceStatus(Input * pinput, Mesh * pmesh)
{
    if (d_convergence_state == 1 || d_convergence_state == 2)
    {
        d_u = d_u + d_du;
        updateElementData();
        std::vector<double> temp(d_u.data(), d_u.data() + d_u.size());
        d_actural_dt = d_predict_dt;
        d_real_time = d_trial_time;
        d_post->ShowDisplacementOnDeformedConfigration(pinput, pmesh, d_dof_map.get(), temp);
        d_predict_dt = d_actural_dt * d_growth_size;
        if (d_convergence_state == 2) 
        {
            std::cout << "+++++++++++++++++ step size grow up +++++++++++++++" << std::endl;
            d_predict_dt = d_actural_dt * 1.2;
        }
    }
    else if (d_convergence_state == 3)
    {
        std::cout << "********** cut back ***********" << std::endl;
        d_predict_dt = d_predict_dt / 2.0;        
        processRollingBack(pinput, pmesh);
    }
    else
    {
        toolbox::error("not suppout convergence state");
    }
}


void NonLinearStaticSolver::processRollingBack(Input * pinput, Mesh * pmesh)
{
    d_du.setZero();
    setEigenVectorToElementData(d_du, d_dof_map.get(), d_element_data, "du");
    d_element_assembler->assembleElementStiffness(pinput, pmesh,
                                                  d_dof_map.get(),
                                                  d_element_data,
                                                  d_K,
                                                  d_contral_param.get());

    std::vector<double> temp;
    d_element_assembler->assembleElementVector(pinput,
                                               pmesh,
                                               d_dof_map.get(),
                                               d_element_data,
                                               temp,
                                               d_contral_param.get());    
    Eigen::VectorXd temp_eigen = Eigen::Map<Eigen::VectorXd>(temp.data(), temp.size());
    d_internal_force = temp_eigen;
}

void NonLinearStaticSolver::initializeInterationStep()
{
    for (auto & element_data : d_element_data)
    {
        element_data.is_updated_interation = false;
    }    
}

void  NonLinearStaticSolver::initializeLoadStep()
{
    std::cout << "++++++++++++++++++ load step  " << d_contral_param->load_step + 1 << "  ++++++++" << std::endl;
    d_contral_param->load_step++;
    d_contral_param->iteration_step = 0;
    d_du.setZero();
    if (d_real_time + d_predict_dt > 1.0)
        d_predict_dt = 1.0 - d_real_time;
    d_trial_time = d_real_time + d_predict_dt;
    std::cout << "++++++++++++++++++ trial time  " << d_trial_time << "  ++++++++" << std::endl;
    // 仅适用于保守载荷
    d_P_trial = d_P * d_trial_time;
    // 仅适用于保守约束
    d_G_trial = d_G * d_trial_time;
    d_convergence_state = 0;
}