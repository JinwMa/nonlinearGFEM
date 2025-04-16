
#include "input.h"
#include "mesh.h"
#include "BaseFlowSolver.h"
void solve(std::shared_ptr<DataBase>solve_db, std::shared_ptr<Mesh> mesh)
{
    auto solver = std::make_shared<BaseFlowSolver>(solve_db, mesh);
    solver->init();
    solver->solve();
}