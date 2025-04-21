#include "BaseAtomSolver.h"


void BaseAtomSolver::takeDB()
{

}

void BaseAtomSolver::initializeSolver()
{
    d_dof_map = make_shared<DofMap>(d_mesh);
    d_control_param = make_shared<ObjectControlParam>();
    d_constraint_manger = make_shared<ConstraintManager>(d_solve_db, d_mesh, d_dof_map);
    d_constraint_manger->init();
    d_elementAssembler = make_shared<ElementAssembler>(d_solve_db, d_mesh, d_dof_map);
}


void BaseAtomSolver::initializeIterationStep()
{

}

void BaseAtomSolver::solveOneTimeStep(const double current_time,
                                      const double predict_dt,
                                      double & actual_dt)
{

}

void BaseAtomSolver::afterSolve()
{

}

void BaseAtomSolver::initializeTimeStep(const double current_time)
{

}