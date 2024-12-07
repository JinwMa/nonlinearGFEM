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

    map<int, vector<vector<int>>> d_element_face_node_order;
    std::vector<int> d_element_type;
    std::vector<int> d_node_list;
    std::vector<int> d_element_list;
    std::vector<std::vector<double>> d_nodes_coordinate;
    std::vector<std::vector<int>> d_nodes_on_elements;

    map<int, vector<int>> d_node_sets;
    map<int, vector<int>> d_element_sets;
    map<int, vector<vector<int>>> d_segment_sets;
    unordered_map<int, string> d_element_set_name;

    map<int, ObjectBody> d_bodies;
    map<std::string, int> d_body_name_map_to_id;

    unordered_map<int, int> d_node_order_in_list;
    unordered_map<int, int> d_element_order_in_list;

    unordered_map<int, vector<int>>d_elements_of_nodes;
    Mesh(Input * pinput, const std::string &filename) : d_mesh_filename(filename)
    {
        buildElementFaceNodeOrder();
        readmeshfile();
        checkmesh();
        getElementSetName(pinput);
        buildBodies(pinput);
    }
    void checkmesh();

    int getNodeLocalId(const int node_global_id)
    {
        return d_node_order_in_list[node_global_id] - 1;
    }
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
};

#endif // FILE1_H
