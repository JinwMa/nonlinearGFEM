#include "BaseFlowSolver.h"



void BaseFlowSolver::init()
{
    this->takeDB();
    this->initializeSolver();
}


void BaseFlowSolver::solve()
{
    d_continue_flag = true;
    while(d_continue_flag)
    {
        this->initializeTimeStep();
        this->inOneTimeStep();
        this->timePassOn();
    }
}



void BaseFlowSolver::takeDB()
{
    
}

void BaseFlowSolver::initializeSolver()
{

}

void BaseFlowSolver::inOneTimeStep()
{
    d_atom_solver->solveOneTimeStep();
}