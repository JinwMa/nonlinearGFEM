#ifndef OBJECTELEMENTDATA1_H
#define OBJECTELEMENTDATA1_H

#include<string>
#include<vector>
using namespace std;
struct ElementData
{
    // 留下的都是必要的

    // element_parameter
    bool very_first = true;                       // 是否初始化该单元信息
    bool step_first = true;                       // 是否在载荷步中初始化该单元信息
    bool iteration_first = true;                  // 是否在迭代步中初始化该单元信息  

    // 单元基本信息
    vector<int> node_local_ids;                   // 单元上节点的局部编号
    vector<vector<double>> nodes_coords;          // 节点坐标信息
    vector<vector<double>> u;                     // 节点位移
    vector<vector<double>> du;                    // 节点增量位移

    // integration_points_info
    int num_integration_points;                   // 积分点数目
    vector<double> weights;                       // 积分点权重
    vector<double> JKB;                           // 积分点对初始构型的雅可比
    vector<double> jkb;                           // 积分点对当前构型的雅可比
    vector<double> jkb_n;                         // 积分点对平衡构型的雅可比
    vector<double> jkb_n1;                        // 积分点对试探构型的雅可比

    // shape_function_info
    vector<vector<double>>sf;                     // 形函数信息
    vector<vector<vector<double>>> sfdxyz;        // 形函数导数信息
    // BbarElement
    vector<double> centroid_sf;                   // 单元中心处的形函数信息
    vector<vector<double>> centroid_sfdxy;        // 单元中心处的形函数导数信息

    // element_internal_variables 
    // 应力  
    vector<vector<vector<double>>> stress_n;      // 平衡构型下的积分点上的应力
    vector<vector<vector<double>>> stress_n1;     // 试探构型下的积分点上的应力 
    // 等效塑性应变 
    vector<double> eff_p_strain_n;                // 等效塑性应变
    vector<double> eff_p_strain_n1;               
    // 屈服半径
    vector<double> kappa_n;
    vector<double> kappa_n1;
    
};







#endif