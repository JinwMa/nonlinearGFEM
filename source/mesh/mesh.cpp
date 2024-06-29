#include "mesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <stdexcept>
void Mesh::readmeshfile()
{    
    std::streampos pos;
    std::ifstream inputFile(meshfilename); // 打开文件
    if (!inputFile)
    {
        std::cerr << "无法打开文件:" << meshfilename << std::endl;
    }
    std::string line;
    while (std::getline(inputFile, line))
    {        
        // 去除前后的空白字符
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        std::string line_lower = line;
        std::transform(line_lower.begin(), line_lower.end(), line_lower.begin(), ::tolower);


        if (line_lower.substr(0, 8) == "*element")
        {
            while (true)
            {
                pos = inputFile.tellg();
                if(!std::getline(inputFile, line))break;
                // 去除前后的空白字符
                line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
                line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
                if (line.empty())
                    continue;

                if (line[0] == '*')
                {
                    inputFile.clear();
                    inputFile.seekg(pos);
                    break;
                }
                // 移除所有逗号
                line.erase(std::remove(line.begin(), line.end(), ','), line.end());
                std::istringstream iss(line);
                int element_id;
                iss >> element_id;

                if (element_id >= maxnum_element || element_id < 1)
                {
                    std::cerr << "错误:元素ID超出范围: (过大或出现0及负值)" << element_id << std::endl;
                    exit(0);
                    continue;
                }

                int node_id;
                std::vector<int> aelement;
                while (iss >> node_id)
                {
                    // mesh_element[element_id - 1].push_back(node_id);
                    aelement.push_back(node_id);
                }                
                if (!aelement.empty())
                {         
                    NodesOnElements.push_back(aelement);           
                    ++actual_element_count;
                    // element_ids[actual_element_count - 1] = element_id;
                    ElementIdList.push_back(element_id);
                    ElementOrderInList[element_id] = actual_element_count;
                    if (element_id > max_elementid) max_elementid = element_id;
                }
            }
        }
        else if (line_lower.substr(0, 5) == "*node")
        {
            while (true)
            {
                pos = inputFile.tellg();
                if (!std::getline(inputFile, line))break;
                line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
                line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
                if (line.empty()) continue;
                if (line[0] == '*')
                {
                    inputFile.clear();
                    inputFile.seekg(pos);
                    break;
                }
                 // 移除所有逗号
                line.erase(std::remove(line.begin(), line.end(), ','), line.end());
                std::istringstream iss(line);
                int node_id;
                iss >> node_id;
                if (node_id >= maxnum_node || node_id < 1)
                {
                    std::cerr << "错误:节点ID超出范围:" << node_id << std::endl;
                    exit(0);
                    continue;
                }

                double coordinate;
                vector<double> coordinates_of_one_node;
                while (iss >> coordinate)
                {
                    // mesh_node[node_id - 1].push_back(coordinate);
                    coordinates_of_one_node.push_back(coordinate);
                }                
                if (!coordinates_of_one_node.empty())
                {
                    NodesCoordinate.push_back(coordinates_of_one_node);
                    ++actual_node_count;
                    // node_ids[actual_node_count - 1] = node_id;
                    NodeIdList.push_back(node_id);
                    NodeOrderInList[node_id] = actual_node_count;
                    if (node_id > max_nodeid) max_nodeid = node_id;
                }  
            }
        }
    }
    inputFile.close();
}

void Mesh::checkmesh()
{
    if (!actual_node_count == NodeIdList.size())
    throw std::runtime_error("单元中节点数目无法对齐");

    if (!actual_element_count == ElementIdList.size())
    throw std::runtime_error("单元中单元数目无法对齐");
}