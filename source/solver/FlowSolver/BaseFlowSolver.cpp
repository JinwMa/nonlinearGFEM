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
    d_atom_solver = std::make_shared<BaseAtomSolver>(d_solve_db, d_mesh);    
}

void BaseFlowSolver::initializeSolver()
{
    d_atom_solver->initializeSolver();
}

void BaseFlowSolver::inOneTimeStep()
{
    d_atom_solver->solveOneTimeStep();
}

void BaseFlowSolver::timePassOn()
{

}

void BaseFlowSolver::initializeTimeStep()
{

}
