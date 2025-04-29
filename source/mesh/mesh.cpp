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
    std::string mesh_file_name = d_mesh_db->getString("mesh_file");
    std::ifstream inputFile(mesh_file_name); // 打开文件
    if (!inputFile)
    {
        std::cerr << "无法打开文件:" << mesh_file_name << std::endl;
    }
    std::string line;
    while (std::getline(inputFile, line))
    {        
        // 去除前后的空白字符
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        std::string line_lower = line;
        // 将整行变成小写
        std::transform(line_lower.begin(), line_lower.end(), line_lower.begin(), ::tolower);

        
        // 读单元
        if (line_lower.substr(0, 8) == "*element" && line_lower.substr(8, 8) != "_")
        {
            //从这里开始是为了读单元的类型，如果读不到就会报错
            // ########################################
            // ########################################
            int element_type = -1;
            std::string typeValue;
            size_t typePos = line_lower.find("type=");
            if (typePos != std::string::npos)
            {
                // 提取从 "type=" 开始的子字符串，并跳过 "type=" 的长度
                size_t start = typePos + 5;         // 5是"type="的长度
                size_t end = line.find(',', start); // 找到下一个逗号的位置
                if (end != std::string::npos)
                {
                    // 如果找到逗号，截取到逗号前的内容
                    typeValue = line.substr(start, end - start);
                }
                else
                {
                    // 如果没有逗号，直接截取到字符串末尾
                    typeValue = line.substr(start);
                }
            }
            else
            {
               toolbox::error("Type not found in the line! " +  line);
            }
            if (typeValue.substr(0, 4) == "C3D4") element_type = 2;
            else if (typeValue.substr(0, 4) == "C3D8") element_type = 1;
            else
            {
                toolbox::error("not support type of element " + typeValue);
            }
            // #############################################################
            // #############################################################
            // 单元类型读取完毕
            while (true)
            {
                pos = inputFile.tellg();  // 用于记录当前文件读到的位置，做好标记，方便seekg返回这里重新读
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

                if (element_id >= d_maxnum_element || element_id < 1)
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
                    d_element_connectivity.push_back(aelement);           
                    ++d_actual_element_count;
                    // element_ids[d_actual_element_count - 1] = element_id;
                    d_element_external_ids.push_back(element_id);
                    d_element_type.push_back(element_type);
                    d_element_internal_ids[element_id] = d_actual_element_count - 1;
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
                if (node_id >= d_maxnum_node || node_id < 1)
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
                    d_nodes_coordinate.push_back(coordinates_of_one_node);
                    ++d_actual_node_count;
                    d_node_external_ids.push_back(node_id);
                    d_node_internal_ids[node_id] = d_actual_node_count - 1;
                }  
            }
        }
        else if (line_lower.substr(0, 5) == "*nset")
        {
            int node_set_id;
            
            // 读取下一行
            if (!std::getline(inputFile, line))
            {
                throw std::runtime_error("Error: Expected an integer on the next line, but no more lines are available.");
            }
            // 去除前后的空白字符
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            // 检查是否为空行
            if (line.empty())
            {
                throw std::runtime_error("Error: Expected an integer, but the line is empty.");
            }
            try
            {
                node_set_id = std::stoi(line);
            }
            catch (const std::invalid_argument &)
            {
                throw std::runtime_error("Error: Expected an integer, but found a non-integer value.");
            }
            catch (const std::out_of_range &)
            {
                throw std::runtime_error("Error: Integer value out of range.");
            }

            if (d_node_sets.find(node_set_id) != d_node_sets.end())
            {
                throw std::runtime_error("Error: Key already exists in the map.");
            }

            if (line.find_first_not_of("0123456789+-") != std::string::npos)
            {
                std::cout << line << std::endl;
                throw std::runtime_error("Error: Expected a single integer, but found additional characters.");
            }
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
                int node_id;
                while (iss >> node_id)
                {
                    d_node_sets[node_set_id].push_back(node_id);
                }
            }
        }
        else if (line_lower.substr(0, 6) == "*elset" )
        {
            int element_set_id;
            
            // 读取下一行
            if (!std::getline(inputFile, line))
            {
                throw std::runtime_error("Error: Expected an integer on the next line, but no more lines are available.");
            }
            // 去除前后的空白字符
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            // 检查是否为空行
            if (line.empty())
            {
                throw std::runtime_error("Error: Expected an integer, but the line is empty.");
            }
            try
            {
                element_set_id = std::stoi(line);
            }
            catch (const std::invalid_argument &)
            {
                throw std::runtime_error("Error: Expected an integer, but found a non-integer value.");
            }
            catch (const std::out_of_range &)
            {
                throw std::runtime_error("Error: Integer value out of range.");
            }

            if (d_element_sets.find(element_set_id) != d_element_sets.end())
            {
                throw std::runtime_error("Error: Key already exists in the ElementSet map.");
            }

            if (line.find_first_not_of("0123456789+-") != std::string::npos)
            {
                std::cout << line << std::endl;
                throw std::runtime_error("Error: Expected a single integer, but found additional characters.");
            }
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
                while (iss >> element_id)
                {
                    d_element_sets[element_set_id].push_back(element_id);
                }
            }
        }
        else if (line_lower.substr(0, 12) == "*segment_set")
        {
            int segment_set_id;
            
            // 读取下一行
            if (!std::getline(inputFile, line))
            {
                throw std::runtime_error("Error: Expected an integer on the next line, but no more lines are available.");
            }
            // 去除前后的空白字符
            line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
            line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
            // 检查是否为空行
            if (line.empty())
            {
                throw std::runtime_error("Error: Expected an integer, but the line is empty.");
            }
            try
            {
                segment_set_id = std::stoi(line);
            }
            catch (const std::invalid_argument &)
            {
                throw std::runtime_error("Error: Expected an integer, but found a non-integer value.");
            }
            catch (const std::out_of_range &)
            {
                throw std::runtime_error("Error: Integer value out of range.");
            }

            if (d_segment_sets.find(segment_set_id) != d_segment_sets.end())
            {
                throw std::runtime_error("Error: Key already exists in the map.");
            }

            if (line.find_first_not_of("0123456789+-") != std::string::npos)
            {
                std::cout << line << std::endl;
                throw std::runtime_error("Error: Expected a single integer, but found additional characters.");
            }
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
                int node_id;
                std::vector<int> asegment;
                while (iss >> node_id)
                {
                    // mesh_element[element_id - 1].push_back(node_id);
                    asegment.push_back(node_id);
                }                
                if (!asegment.empty())
                {         
                    d_segment_sets[segment_set_id].push_back(asegment);
                }
            }
        }
    }
    inputFile.close();
    buildElementsOfNodes();
    std::cout << "当前模型中有: " << std::endl;
    std::cout << "    单元" << d_actual_element_count << std::endl;
    std::cout << "    节点" << d_actual_node_count << std::endl;
}

void Mesh::checkmesh()
{
    if (!d_actual_node_count == d_node_external_ids.size())
    throw std::runtime_error("单元中节点数目无法对齐");

    if (!d_actual_element_count == d_element_external_ids.size())
    throw std::runtime_error("单元中单元数目无法对齐");

    if (!d_element_type.size() == d_actual_element_count)
    throw std::runtime_error("单元类型数目没有对齐");

    for (int i = 0; i < d_actual_element_count; i++)
    {
        if (d_element_type[i] == -1) toolbox::error("单元类型错误");
    }
}


void Mesh::buildElementsOfNodes()
{
    //循环所有的单元
    for (int i = 0; i < d_actual_element_count; i++)
    {
        int element_id = d_element_external_ids[i];
        int element_order = getElementInternalId(element_id);
        // 循环单元上的节点
        for (int j = 0; j < d_element_connectivity[element_order].size(); j++)
        {
            int node_id = d_element_connectivity[element_order][j];
            d_elements_of_nodes[node_id].push_back(element_id);
        }
    }
}




void Mesh::buildElementFaceNodeOrder()
{
    /*
    1 -- Hex8
    2 -- Tet4
    */
   std::vector<std::vector<int>> Hex8;
   Hex8.resize(6);
   Hex8[0] = {0, 3, 2, 1};
   Hex8[1] = {4, 5, 6, 7};
   Hex8[2] = {0, 1, 5, 4};
   Hex8[3] = {1, 2, 6, 5};
   Hex8[4] = {2, 3, 7, 6};
   Hex8[5] = {0, 4, 7, 3};
   std::vector<std::vector<int>> Tet4;
   Tet4.resize(4);
   Tet4[0] = {0, 2, 1};
   Tet4[1] = {0, 1, 3};
   Tet4[2] = {0, 3, 2};
   Tet4[3] = {1, 2, 3};

   d_element_face_node_order[1] = Hex8;
   d_element_face_node_order[2] = Tet4;
}

void Mesh::getOuterFaceOfElementSet(const std::vector<int> & element_set,
                                    std::set<std::set<int>> & element_set_outer_faces)
{
    element_set_outer_faces.clear();
    int num_elements = element_set.size();

    //循环单元集中所有的单元
    for (int i = 0; i < num_elements; i++)
    {
        int element_id = element_set[i];
        int element_order = getElementInternalId(element_id);
        int element_type = d_element_type[element_order];
        auto element_faces = d_element_face_node_order[element_type];
        int num_faces = element_faces.size();

        // 一般来说，对于固定单元，每个面上的节点数目是相同的
        int num_nodes_on_faces = element_faces[0].size();
        //循环每个单元的每个面
        for (int ii = 0; ii < num_faces; ii++)
        {
            std::set<int> nodes_on_face;
            for (int iii = 0; iii < num_nodes_on_faces; iii++)
            {
                int node_id = d_element_connectivity[element_order][element_faces[ii][iii]];
                nodes_on_face.insert(node_id);
            }  
            if (element_set_outer_faces.find(nodes_on_face) == element_set_outer_faces.end())
                element_set_outer_faces.insert(nodes_on_face);
            else
                element_set_outer_faces.erase(nodes_on_face);
        }
    }    
}


void Mesh::buildElementsConnectivityInternal()
{
    d_element_connectivity_internal.resize(d_actual_element_count);
    for (int i = 0; i < d_actual_element_count; i++)
    {
        int numNodes = d_element_connectivity[i].size();
        d_element_connectivity_internal[i].resize(numNodes);
        // 
        for (int j = 0; j < numNodes; j++)
        {
            int nodeExtId = d_element_connectivity[i][j];
            d_element_connectivity_internal[i][j] = getNodeInternalId(nodeExtId);
            // std::cout << d_element_connectivity_internal[i][j] << " ";
        }
        // std::cout << std::endl;
    }
}