

#include<Eigen/Dense>
#include "input.h"
#include "mesh.h"
#include "DofMap.h"
#include "ObjectTime.h"
void solve(std::shared_ptr<DataBase>solve_db, std::shared_ptr<Mesh> mesh)
{
    std::vector<string> solver_list = solve_db->getVectorString("solver_list");
    std::cout << solver_list[0] << std::endl;
}