#ifndef MESH1_H
#define MESH1_H
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include "input.h"
#include "toolbox.h"


using namespace std;
class Mesh
{    
private:
    static const int maxnum_element = 100000;
    static const int maxnum_node = 100000;
    std::string meshfilename;

public:
    int actual_element_count = 0;
    int actual_node_count = 0;
    int max_nodeid = 0;
    int max_elementid = 0;
    std::vector<int> NodeIdList;
    std::vector<int> ElementIdList;
    std::vector<std::vector<double>> NodesCoordinate;
    std::vector<std::vector<int>> NodesOnElements;

    map<int, vector<int>> node_sets;
    map<int, vector<int>> element_sets;
    map<int, vector<vector<int>>> segment_sets;
    unordered_map<int, string> element_setname;
    // map<int, int> NodeOrderInList;
    // map<int, int> ElementOrderInList;

    int NodeOrderInList[maxnum_node] = {};
    int ElementOrderInList[maxnum_element] = {};


    // std::vector<std::vector<int>> mesh_element;
    // std::vector<std::vector<double>> mesh_node;
    // std::vector<int> node_ids;
    // std::vector<int> element_ids;
    Mesh(Input * pinput, const std::string &filename) : meshfilename(filename)//,
                                        // mesh_element(maxnum_element),
                                        // mesh_node(maxnum_node),
                                        // node_ids(maxnum_node),
                                        // element_ids(maxnum_element)
    {
        readmeshfile();
        checkmesh();
        getElementSetName(pinput);
    }
    void checkmesh();

private:
    void readmeshfile();
    void getElementSetName(Input * pinput);
};

#endif // FILE1_H
