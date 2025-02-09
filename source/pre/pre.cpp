#include "pre.h"

void Pre::preprocess(Input *pinput, Mesh *pmesh)
{
    if (!pinput->ifExist("preprocessing_id"))
        return;
    std::string name = pinput->getString("preprocessing_id");
    if (name == "necking-bar")
    {
        std::cout << "preprocessing necking bar" << std::endl;
        neckingBar(pinput, pmesh);
    }
    else if (name == "Kirchhoff_hyperelasticity_nonlinear-cantileverbeam")
    {
        std::cout << "preprocessing Kirchhoff_hyperelasticity_nonlinear-cantileverbeam" << std::endl;
        kirchhoffHyperelasticityNonlinearCantileverbeam(pinput, pmesh);
    }
}

void Pre::neckingBar(Input *pinput, Mesh *pmesh)
{
    double qx = 0.982;
    double zl = 26.667;
    int num_node = pmesh->d_actual_node_count;
    // std::cout << "x" << std::endl;
    // for (int i = 0; i < num_node; i++)
    // {
    //     int node_global_id = pmesh->d_node_list[i];
    //     int node_local_id = pmesh->getNodeLocalId(node_global_id);
    //     double &x = pmesh->d_nodes_coordinate[node_local_id][0];
    //     double &y = pmesh->d_nodes_coordinate[node_local_id][1];
    //     double &z = pmesh->d_nodes_coordinate[node_local_id][2];
    //     if (std::fabs(x) < 1.E-10)
    //         std::cout << node_global_id << " ,";
    // }
    // std::cout << std::endl;

    // std::cout << "y" << std::endl;

    // for (int i = 0; i < num_node; i++)
    // {
    //     int node_global_id = pmesh->d_node_list[i];
    //     int node_local_id = pmesh->getNodeLocalId(node_global_id);
    //     double &x = pmesh->d_nodes_coordinate[node_local_id][0];
    //     double &y = pmesh->d_nodes_coordinate[node_local_id][1];
    //     double &z = pmesh->d_nodes_coordinate[node_local_id][2];
    //     if (std::fabs(y) < 1.E-10)
    //         std::cout << node_global_id << " ,";
    // }
    // std::cout << std::endl;

    // std::cout << "z" << std::endl;
    // for (int i = 0; i < num_node; i++)
    // {
    //     int node_global_id = pmesh->d_node_list[i];
    //     int node_local_id = pmesh->getNodeLocalId(node_global_id);
    //     double &x = pmesh->d_nodes_coordinate[node_local_id][0];
    //     double &y = pmesh->d_nodes_coordinate[node_local_id][1];
    //     double &z = pmesh->d_nodes_coordinate[node_local_id][2];
    //     if (std::fabs(z) < 1.E-10)
    //         std::cout << node_global_id << " ,";
    // }
    // std::cout << std::endl;

    // std::cout << "zl" << std::endl;
    // for (int i = 0; i < num_node; i++)
    // {
    //     int node_global_id = pmesh->d_node_list[i];
    //     int node_local_id = pmesh->getNodeLocalId(node_global_id);
    //     double & x = pmesh->d_nodes_coordinate[node_local_id][0];
    //     double & y = pmesh->d_nodes_coordinate[node_local_id][1];
    //     double & z = pmesh->d_nodes_coordinate[node_local_id][2];
    //     if (std::fabs(z - zl) < 1.E-2) std::cout << node_global_id << " ,";
    // }

    for (int i = 0; i < num_node; i++)
    {
        int node_global_id = pmesh->d_node_list[i];
        int node_local_id = pmesh->getNodeLocalId(node_global_id);
        double &x = pmesh->d_nodes_coordinate[node_local_id][0];
        double &y = pmesh->d_nodes_coordinate[node_local_id][1];
        double &z = pmesh->d_nodes_coordinate[node_local_id][2];
        double e = qx + (1.0 - qx) * (z / zl);
        x = x * e;
        y = y * e;
    }
}

void Pre::kirchhoffHyperelasticityNonlinearCantileverbeam(Input * pinput, Mesh * pmesh)
{
    std::vector<double> random_nums;
    std::string random_num_file_name = pinput->getString("input_random_number_file");
    double distorted_degree = pinput->getDouble("preprocessing_distorted_degree");
    std::ifstream random_num_file(random_num_file_name, std::ios::in);
    if (!random_num_file.is_open()) {
        toolbox::error(random_num_file_name + " failed to open");
    }
    std::string line;
    for (int i = 0; i < pmesh->d_actual_node_count; i++)
    {
        std::getline(random_num_file, line);
        std::istringstream iss(line);
        double random_num;
        iss >> random_num;
        random_nums.push_back(random_num);
    }

    std::set<set<int>> aaa;
    pmesh->getOuterFaceOfElementSet(pmesh->d_element_list, aaa);
    
}