
#include "input.h"
#include "mesh.h"
#include "BaseFlowSolver.h"
#include "ObjectTime.h"
void solve(std::shared_ptr<DataBase>solve_db, std::shared_ptr<Mesh> mesh)
{
    auto solver = std::make_shared<BaseFlowSolver>(solve_db, mesh);
    solver->init();
    solver->solve();
    TimeMater::getInstance()->getPassedCpuTimeFromLast("time used in solve");
}