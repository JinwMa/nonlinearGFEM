#ifndef OBJECTELEMENT1_H
#define OBJECTELEMENT1_H

#include<vector>

using namespace std;
struct ObjectElement
{
        vector<int> element_connect_nodes;
        int num_Gauss_points;
        vector<vector<double>> sf_on_Gauss_points;   // 关联节点在高斯点上的形函数
        vector<vector<vector<double>>> sfdxyz_on_Gauss_points;
        vector<double> effecitve_plastic_strain_on_Gauss_points;
        vector<std::array<std::array<double, 3>, 3>> stress_tensor_on_Gauss_points;    
        vector<std::array<std::array<double, 3>, 3>> F_on_Gauss_points;
        vector<std::array<std::array<double, 3>, 3>> Finv_on_Gauss_points;
        vector<double> jkb;
        vector<double> weights;

        bool if_element_is_initialized; 
        string test = "show for a test";
};




#endif