#include "pre.h"


void Pre::preprocess(Input * pinput, Mesh * pmesh)
{
    const double eps = 1.E-10;
    int node_count = pmesh->d_actual_node_count;
    const auto & coord = pmesh->d_nodes_coordinate;
    for (int i = 0; i < node_count; i++)
    {
        int node_id = pmesh->d_node_list[i];
        int node_order = pmesh->getNodeLocalId(node_id);
        double x = coord[node_order][0];
        double y = coord[node_order][1];
        double z = coord[node_order][2];

        if (std::fabs(z - 4.0) < eps) std::cout << node_id << ", ";

    }
}