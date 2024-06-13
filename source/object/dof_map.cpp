#include <iostream>
#include <vector>
#include <string>
#include "dof_map.h"
#include "mesh.h"

using namespace std;
void Dof_Map::get_nodes_dofs(const std::vector<int>& vec, const int length)
{
    vector<string> alldofs = {"ux", "uy", "uz"};
    std::cout << alldofs.size() << std::endl;

}