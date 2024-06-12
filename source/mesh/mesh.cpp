#include "mesh.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <algorithm>
void Mesh::readmeshfile()
{
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

        // std::cout << line_lower << std::endl;

        if (line_lower.substr(0, 8) == "*element")
        {
            while (std::getline(inputFile, line))
            {
                // 去除前后的空白字符
                line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
                line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
                if (line.empty())
                    continue;

                if (line[0] == '*')
                {
                    // 如果遇到新的块开头,停止读取
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
                while (iss >> node_id)
                {
                    mesh_element[element_id - 1].push_back(node_id);
                }
                if (!mesh_element[element_id - 1].empty())
                {
                    ++actual_element_count;
                }
            }
        }
        else if (line_lower.substr(0, 5) == "*node")
        {
            while (std::getline(inputFile, line))
            {
                line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
                line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
                if (line.empty()) continue;
                if (line[0] == '*')
                {
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
                while (iss >> coordinate)
                {
                    mesh_node[node_id - 1].push_back(coordinate);
                }
                if (!mesh_node[node_id - 1].empty())
                {
                    ++actual_node_count;
                }

            }
        }
    }
    inputFile.close();
}