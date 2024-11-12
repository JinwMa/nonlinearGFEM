#ifndef LINEARSTATICSOLVER1_H
#define LINEARSTATICSOLVER1_H

#include<memory>
#include"BaseSolver.h"

using namespace std;
class LinearStaticSolver : public BaseSolver
{
    public:
    LinearStaticSolver(){
        std::cout << "build LinearStaticSolver" << std::endl;
    }
    ~LinearStaticSolver(){
        std::cout << "delete LinearStaticSolver" << std::endl;
    }
    void solve(Input * pinput, Mesh * pmesh);
    void takeDB(Input * pinput);
    void init(Input * pinput, Mesh * pmesh);
    
    private:
    std::shared_ptr<ConstraintManager> d_constraint_manager;
    std::shared_ptr<LoadManger> d_load_manager;
    std::shared_ptr<ElementAssembler> d_element_assembler;
    std::shared_ptr<Dof_Map> d_dof_map;

    Eigen::SparseMatrix<double> d_C;
    Eigen::SparseMatrix<double> d_K;
    Eigen::VectorXd d_G;
    Eigen::VectorXd d_P;

};

#endif 