#ifndef STRUCTURALATOMSOLVER_H
#define STRUCTURALATOMSOLVER_H

#include "BaseAtomSolver.h"

class StructuralAtomSolver : public BaseAtomSolver
{
    public:
    using BaseClass = BaseAtomSolver;
    StructuralAtomSolver(std::shared_ptr<DataBase> solve_db, std::shared_ptr<Mesh> mesh) 
       :BaseClass(solve_db, mesh){};
    virtual ~StructuralAtomSolver(){};

    virtual void takeDB();

    virtual void initializeSolver();

    virtual void initializeTimeStep(const double current_time);

    virtual void initializeIterationStep();

    virtual void solveOneTimeStep(const double, const double, double &);

    virtual void afterSolve();  

    // 成员变量
};




#endif