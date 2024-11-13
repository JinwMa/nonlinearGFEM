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
    std::vector<ObjectElement> d_elements_data;
    std::vector<double> d_u;
    std::vector<double> d_du;
    std::vector<double> d_ddu;
};





#endif
