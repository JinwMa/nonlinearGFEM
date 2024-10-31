#ifndef OBJECTELEMENT1_H
#define OBJECTELEMENT1_H

#include<vector>

using namespace std;
struct ObjectElement
{
        vector<int> element_connect_nodes;
        int num_Gauss_points;
        vector<vector<double>> sf_on_Gauss_points;   // 关联节点在高斯点上的形函数
        vector<vector<double>> sfdx_on_Gauss_points; 
        vector<vector<double>> sfdy_on_Gauss_points; 
        vector<vector<double>> sfdz_on_Gauss_points;
        vector<double> effecitve_plastic_strain_on_Gauss_points;
        vector<vector<double>> stress_vector_on_Gauss_points;    
        bool if_element_sf_is_exist; 
};




#endif