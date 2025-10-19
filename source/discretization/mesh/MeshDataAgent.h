#ifndef MESHDATAAGENT_H
#define MESHDATAAGENT_H
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include "input.h"
#include "toolbox.h"

using namespace std;
class MeshDataAgent
{

public:
    MeshDataAgent(std::shared_ptr<DataBase> mesh_db)
    {
        d_mesh_db = mesh_db;
        d_mesh_filename = mesh_db->getString("mesh_file");
        readNodeInfo();
        readElementInfo();
    }
    ~MeshDataAgent() {};

    // 内外节点Id相互映射
    inline int getNodeInternalId(int nodeExternalId){
        return d_externalNodeId_to_internalNodeId[nodeExternalId];
    }
    inline int getNodeExternalId(int nodeInternalId)
    {
        return d_internalNodeId_to_externalNodeId[nodeInternalId];
    }
    
    // 获取节点坐标
    inline std::vector<double> getNodeCoordinate(int nodeInternalId)
    {
        return d_nodes_coordinate[nodeInternalId];
    }

    // 获取单元节点列表
    inline void getElementNodes(int elementInternalId, std::vector<int> & nodes)
    {
        nodes = d_element_connect_to_nodes[elementInternalId];
        // return d_element_connect_to_nodes[elementInternalId];
    }
    
    // 获取当前模型中的单元数量
    inline int getElementNum()
    {
        return d_actual_element_count;
    }

    // 获取当前模型中节点数量
    inline int getNodeNum()
    {
        return d_actual_node_count;
    }

    // 获取当前模型中部件编号列表 -- 用于分类计算和组装刚度矩阵
    inline std::vector<int> getPartIdList()
    {
        return d_part_ids;
    }

    // 根据partId获取单元类型
    inline std::string getElementTypeByPartId(int partId)
    {
        if (d_part_element_type.find(partId) != d_part_element_type.end())
            return d_part_element_type[partId];
        else
            toolbox::error("do not have this pardId in model");
        return "unknown";
    }

    // 根据partId获取单元列表
    inline void getElementListByPardId(int partId, std::vector<int> &elements)
    {
        if (d_part_connect_elements.find(partId) != d_part_connect_elements.end())
            elements = d_part_connect_elements[partId];
        else
            toolbox::error("do not have this pardId in model");
    }

private:
    int d_maxnum_element = 300000;
    int d_maxnum_node = 300000;
    std::string d_mesh_filename;
    std::shared_ptr<DataBase> d_mesh_db;

    // node
    int d_actual_node_count = 0;
    int d_actual_node_count_original = 0; // 针对加点情况特别设置原始模型中的点数
    std::vector<std::vector<double>> d_nodes_coordinate;
    std::vector<int> d_internalNodeId_to_externalNodeId;
    std::unordered_map<int, int> d_externalNodeId_to_internalNodeId;
    unordered_map<int, vector<int>> d_node_connect_to_elements;

    // element
    int d_actual_element_count = 0;
    std::vector<int> d_element_type;
    vector<vector<int>> d_element_connect_to_nodes;
    std::vector<int> d_internalElementId_to_externalElementId;
    std::unordered_map<int, int> d_externalElementId_to_internalElementId;

    // part
    std::map<int, std::vector<int>> d_part_connect_elements;
    std::map<int, std::string> d_part_element_type;
    std::vector<int> d_part_ids;

    // face -- todo
    // edge -- todo

    //
    void readmeshfile();
    void readNodeInfo();
    void readElementInfo();

    inline int getElementTypeIndex(std::string typeName)
    {
        if (typeName.substr(0, 4) == "C3D4")
           return 2;
        else if (typeName.substr(0, 4) == "C3D8")
           return 1;
        else
        {
            toolbox::error("not support type of element " + typeName);
        }
        return 0;
    }
};

#endif // MESH1_H
