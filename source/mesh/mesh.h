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
private:
    static const int maxnum_element = 300000;
    static const int maxnum_node = 300000;
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

    map<int, ObjectBody> bodies;

    unordered_map<int, int> NodeOrderInList;
    unordered_map<int, int> ElementOrderInList;

    unordered_map<int, vector<int>>ElementsOfNodes;
    Mesh(Input * pinput, const std::string &filename) : meshfilename(filename)
    {
        readmeshfile();
        checkmesh();
        getElementSetName(pinput);
    }
    void checkmesh();

    int getNodeLocalId(const int node_global_id)
    {
        return NodeOrderInList[node_global_id] - 1;
    }
    int getElementId(const int element_global_id)
    {
        return ElementOrderInList[element_global_id] - 1;
    }



private:
    void readmeshfile();
    void getElementSetName(Input * pinput);
    void buildElementsOfNodes();
};

#endif // FILE1_H
