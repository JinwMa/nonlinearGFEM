#ifndef NONLINEARSTATICSOLVER_H
#define NONLINEARSTATICSOLVER_H

#include "BaseSolver.h"

class NonLinearStaticSolver : public BaseSolver
{
    public:
    NonLinearStaticSolver(){
        std::cout << "build NonLinearStaticSolver" << std::endl;
    }
    ~NonLinearStaticSolver(){
        std::cout << "delete NonLinearStaticSolver" << std::endl;
    }
    void solve(Input * pinput, Mesh * pmesh);
    void takeDB(Input * pinput, const std::string & name);
    void init(Input * pinput, Mesh * pmesh) override;

    void initData(Input * pinput, Mesh * pmesh);

    private:

    int checkConvergence();
    
    private:
    // std::vector<ObjectElement> d_elements_data;
    Eigen::VectorXd d_u;
    Eigen::VectorXd d_du;
    Eigen::VectorXd d_ddu;
    Eigen::VectorXd d_lambda;    
    Eigen::VectorXd d_internal_force;
    Eigen::VectorXd d_rhs;    
    std::shared_ptr<ConstraintManager> d_constraint_manager;
    std::shared_ptr<LoadManger> d_load_manager;
    std::shared_ptr<ElementAssembler> d_element_assembler;
    std::shared_ptr<Dof_Map> d_dof_map;
    std::shared_ptr<Post> d_post;

    Eigen::SparseMatrix<double> d_C;
    Eigen::SparseMatrix<double> d_K;


    // Eigen::SparseMatrix<double> d_K_n;
    // Eigen::VectorXd d_internal_force_n;



    Eigen::VectorXd d_G;
    Eigen::VectorXd d_P;
    Eigen::VectorXd d_P_trial;
    Eigen::VectorXd d_G_trial;

    //adaptive load step
    double d_real_time = 0.0;
    double d_actural_dt = 0.0;
    double d_trial_time = 0.0;
    double d_predict_dt = 0.0;
    int d_convergence_state = 0;

    Eigen::VectorXd d_rhs_G;
    int d_num_load_step = 10;

    void updateElementData();

    void dealWithConvergenceStatus(Input * pinput, Mesh * pmesh);
    void processRollingBack(Input * pinput, Mesh * pmesh);

    void initializeInterationStep();
    void initializeLoadStep();


};





#endif
