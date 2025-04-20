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
    std::string solver_type = d_solve_db->getString("type");
    if (true)
    {
        d_atom_solver = make_shared<BaseAtomSolver>(d_solve_db, d_mesh);
    }
}

void BaseFlowSolver::initializeSolver()
{
    if (d_atom_solver == nullptr) toolbox::error("atom solver is not init");
    d_atom_solver->takeDB();
    d_atom_solver->initializeSolver();
}

void BaseFlowSolver::inOneTimeStep()
{
    d_atom_solver->solveOneTimeStep(d_current_time,
                                    d_predict_dt,
                                    d_actual_dt);
}

void BaseFlowSolver::timePassOn()
{
    d_current_time += d_actual_dt;
    if (d_current_time >= d_end_time) d_continue_flag = false;
}

void BaseFlowSolver::initializeTimeStep()
{
    d_atom_solver->initializeTimeStep(d_current_time);
}
