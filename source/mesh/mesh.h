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
    int d_maxnum_element = 300000;
    int d_maxnum_node = 300000;
    std::string d_mesh_filename;
    std::shared_ptr<DataBase> d_mesh_db;

public:
    int d_actual_element_count = 0;
    int d_actual_node_count = 0;

    map<int, vector<vector<int>>> d_element_face_node_order; // element_face_node
    std::vector<int> d_element_type;                         // element_type
    std::vector<int> d_node_external_ids;                      // node outer ids in order
    std::vector<int> d_element_external_ids;                   // element outer ids in order
    std::vector<std::vector<double>> d_nodes_coordinate;     // nodes' coordinate
    std::vector<std::vector<int>> d_element_connectivity;    // elements' connectivity

    map<int, vector<int>> d_node_sets;                       // 节点集 : 点集编号和节点编号都是外部编号
    map<int, vector<int>> d_element_sets;                    // 单元集 : 单元集编号和单元编号都是外部编号
    map<int, vector<vector<int>>> d_segment_sets;            // 面片集 : 存储形式: 一个片段代表一个面片
    

    unordered_map<int, int> d_node_internal_ids;
    unordered_map<int, int> d_element_internal_ids;

    unordered_map<int, vector<int>>d_elements_of_nodes;      // 节点关联的单元

    Mesh(std::shared_ptr<DataBase> mesh_db) 
    {
        d_mesh_db = mesh_db;
        buildElementFaceNodeOrder();                         // 创建单元上节点的序号
        readmeshfile();                                      // 读网格 节点集 单元集 面片集 
        checkmesh();                                         // 检查网格
    }
    void checkmesh();

    int getNodeInternalId(const int node_global_id)
    {
        return d_node_internal_ids[node_global_id];
    }

    
    int getElementInternalId(const int element_global_id)
    {
        return d_element_internal_ids[element_global_id];
    }

    int getElementExternalId(const int element_local_id)
    {
        return d_element_external_ids[element_local_id];
    }

    int getNodeExternalId(const int node_local_id)
    {
        return d_node_external_ids[node_local_id];
    }





private:
    void readmeshfile();
    void buildElementsOfNodes();
    void buildElementFaceNodeOrder();

public:
    void getOuterFaceOfElementSet(const vector<int> & element_set,
                                  set<set<int>> & element_set_outer_faces); // 获取单元集的外表面面片
    
};

#endif // MESH1_H
