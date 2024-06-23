
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include "mesh.h"
#include "input.h"
#include "LinearHex8.h"
using namespace std;

void solve(Input &input, Mesh &mesh)
{
    vector<int> node_ids_in_a_element = mesh.mesh_element[0];
    vector<vector<double>> GaussPoint;
    auto elem = new LinearHex8;
    elem->SetGaussIntegration(3, GaussPoint);
    double nodes_coordinates[8][3];
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            nodes_coordinates[i][j] = mesh.mesh_node[node_ids_in_a_element[i] - 1][j];

    double elementmat[24][24];
    elem->ComputeStiffness(nodes_coordinates, GaussPoint, elementmat);
    for (int i = 0; i < 24; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            std::cout << std::setw(10) << elementmat[i][j];
        }
        std::cout << "\n";
    }
}