#ifndef OBJECTELEMENTDATA1_H
#define OBJECTELEMENTDATA1_H

#include<string>
#include<vector>
using namespace std;
struct ObjectElementData
{
    bool is_updated_interation = false;
    // element level
    bool is_initialized;
    bool is_alive;
    int element_id;
    int element_order;
    int element_layer;
    string element_type;

    // node level
    int num_nodes;
    vector<int> node_ids;
    vector<int> element_patch;
    vector<vector<double>> u;
    vector<vector<double>> du;
    vector<vector<double>> ddu;
    vector<vector<double>> coordinates;

    // integration_points
    int num_integration_points;
    vector<vector<double>>sf;
    vector<vector<vector<double>>> sfdxyz;
    vector<vector<vector<double>>> F;
    vector<vector<vector<double>>> F_n;
    vector<vector<vector<double>>> F_n1;

    vector<double> weights;
    vector<double> JKB;
    vector<double> jkb;
    vector<double> jkb_n;
    vector<double> jkb_n1;

    vector<vector<vector<double>>> Finv;
    vector<vector<vector<double>>> Finv_n;
    vector<vector<vector<double>>> Finv_n1;    

    vector<vector<vector<double>>> stress;
    vector<vector<vector<double>>> stress_n;
    vector<vector<vector<double>>> stress_n1;   

    vector<double> eff_p_strain;
    vector<double> eff_p_strain_n;
    vector<double> eff_p_strain_n1;

    vector<double> kappa_n;
    vector<double> kappa_n1;

    // BbarElement
    vector<double> centroid_sf;
    vector<vector<double>> centroid_sfdxy;
    vector<vector<double>> centroid_F_n;
    vector<vector<double>> centroid_F_n1;
    vector<vector<double>> centroid_Finv_n;
    vector<vector<double>> centroid_Finv_n1;
};







#endif