#ifndef MESH1_H
#define MESH1_H
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include "input.h"
#include "toolbox.h"
#include "ObjectBody.h"


using namespace std;
class Mesh
{    

    public:
    ~Mesh(){};
private:
    static const int d_maxnum_element = 300000;
    static const int d_maxnum_node = 300000;
    std::string d_mesh_filename;

public:
    int d_actual_element_count = 0;
    int d_actual_node_count = 0;
    int d_max_nodeid = 0;
    int d_max_elementid = 0;

    map<int, vector<vector<int>>> d_element_face_node_order; // 单元面片上的节点
    std::vector<int> d_element_type;                         //每个单元的单元类型
    std::vector<int> d_node_list;
    std::vector<int> d_element_list;
    std::vector<std::vector<double>> d_nodes_coordinate;
    std::vector<std::vector<int>> d_nodes_on_elements;

    map<int, vector<int>> d_node_sets;             // 节点集
    map<int, vector<int>> d_element_sets;          // 单元集
    map<int, vector<vector<int>>> d_segment_sets;  // 面片集
    unordered_map<int, string> d_element_set_name;

    map<int, ObjectBody> d_bodies;
    map<std::string, int> d_body_name_map_to_id;

    unordered_map<int, int> d_node_order_in_list;
    unordered_map<int, int> d_element_order_in_list;

    unordered_map<int, vector<int>>d_elements_of_nodes;
    std::set<std::set<int>> d_all_outer_faces;
    std::unordered_set<int> d_all_outer_nodes;


    Mesh(Input * pinput, const std::string &filename) : d_mesh_filename(filename)
    {
        buildElementFaceNodeOrder(); // 创建单元上节点的序号
        readmeshfile();              // 读网格 节点集 单元集 面片集 
        checkmesh();                 // 检查网格
        getElementSetName(pinput);   // 反向获取每个单元的input信息
        buildBodies(pinput);         // 建立网格中的实体集
    }
    void checkmesh();

    // 所谓局部id，指的是节点的全局编号在所有节点中的顺序
    int getNodeLocalId(const int node_global_id)
    {
        return d_node_order_in_list[node_global_id] - 1;
    }
    // 所谓局部id，指的是单元的全局编号在所有单元中的顺序
    int getElementLocalId(const int element_global_id)
    {
        return d_element_order_in_list[element_global_id] - 1;
    }



private:
    void readmeshfile();
    void getElementSetName(Input * pinput);
    void buildElementsOfNodes();
    void buildBodies(Input * pinput);
    void buildElementFaceNodeOrder();

public:
    void getOuterFaceOfElementSet(const vector<int> & element_set,
                                  set<set<int>> & element_set_outer_faces); // 获取单元集的外表面面片
    
};

#endif // FILE1_H
