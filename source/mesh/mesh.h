#ifndef MESH1_H
#define MESH1_H
#include <vector>
#include <string>
class Mesh
{
    //TODO:
    //目前的网格格式仅支持节点编号从1到n连续分布
    //目前的网格格式仅支持单元编号从1到n连续分布

private:
    const int maxnum_element = 100000;
    const int maxnum_node = 100000;
    std::string meshfilename;

public:
    int actual_element_count = 0;
    int actual_node_count = 0;
    int max_nodeid = 0;
    int max_elementid = 0;
    std::vector<std::vector<int>> mesh_element;
    std::vector<std::vector<double>> mesh_node;
    std::vector<int> node_ids;
    std::vector<int> element_ids;
    Mesh(const std::string &filename)
        : meshfilename(filename),
          mesh_element(maxnum_element),
          mesh_node(maxnum_node),
          node_ids(maxnum_node),
          element_ids(maxnum_element)
    {
        readmeshfile();
    }
    void checkmesh();

private:
    void readmeshfile();
};

#endif // FILE1_H
