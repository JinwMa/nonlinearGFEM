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
    void takeDB(Input * pinput) {};
    void init(Input * pinput, Mesh * pmesh) override;
    
    private:
    // std::vector<ObjectElement> d_elements_data;
    std::vector<double> d_u;
    std::vector<double> d_du;
    std::vector<double> d_ddu;

    std::vector<double> d_internal_force;

    
    std::shared_ptr<ConstraintManager> d_constraint_manager;
    std::shared_ptr<LoadManger> d_load_manager;
    std::shared_ptr<ElementAssembler> d_element_assembler;
    std::shared_ptr<Dof_Map> d_dof_map;
    std::shared_ptr<Post> d_post;

    Eigen::SparseMatrix<double> d_C;
    Eigen::SparseMatrix<double> d_K;
    Eigen::VectorXd d_G;
    Eigen::VectorXd d_P;

};





#endif
