#ifndef LINEARSTATICSOLVER1_H
#define LINEARSTATICSOLVER1_H


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
    void takeDB();

};

#endif // POST1_H