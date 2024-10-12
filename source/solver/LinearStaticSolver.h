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

    void vector_assembler(Input * pinput, Mesh * pmesh, Eigen::VectorXd & vector);
    // void matrix_assembler(Input * pinput, Mesh * pmesh);

    void assembleElementStiffness(Input * pinput, Mesh * pmesh, Eigen::SparseMatrix<double> & Matrix);




    private:
    // Ku = P; CTu = G

};

#endif 