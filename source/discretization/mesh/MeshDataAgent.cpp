#include "MeshDataAgent.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <stdexcept>


// void MeshDataAgent::readmeshfile()
// {    
//     std::streampos pos;
//     std::string mesh_file_name = d_mesh_db->getString("mesh_file");
//     std::ifstream inputFile(mesh_file_name); // 打开文件
//     if (!inputFile)
//     {
//         std::cerr << "无法打开文件:" << mesh_file_name << std::endl;
//     }
//     std::string line;
//     while (std::getline(inputFile, line))
//     {        
//         // 去除前后的空白字符
//         line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//         line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//         std::string line_lower = line;
//         // 将整行变成小写
//         std::transform(line_lower.begin(), line_lower.end(), line_lower.begin(), ::tolower);

        
//         // 读单元
//         if (line_lower.substr(0, 8) == "*element" && line_lower.substr(8, 8) != "_")
//         {
//             //从这里开始是为了读单元的类型，如果读不到就会报错
//             // ########################################
//             // ########################################
//             int element_type = -1;
//             std::string typeValue;
//             size_t typePos = line_lower.find("type=");
//             if (typePos != std::string::npos)
//             {
//                 // 提取从 "type=" 开始的子字符串，并跳过 "type=" 的长度
//                 size_t start = typePos + 5;         // 5是"type="的长度
//                 size_t end = line.find(',', start); // 找到下一个逗号的位置
//                 if (end != std::string::npos)
//                 {
//                     // 如果找到逗号，截取到逗号前的内容
//                     typeValue = line.substr(start, end - start);
//                 }
//                 else
//                 {
//                     // 如果没有逗号，直接截取到字符串末尾
//                     typeValue = line.substr(start);
//                 }
//             }
//             else
//             {
//                toolbox::error("Type not found in the line! " +  line);
//             }
            
//             // #############################################################
//             // #############################################################
//             // 单元类型读取完毕
//             while (true)
//             {
//                 pos = inputFile.tellg();  // 用于记录当前文件读到的位置，做好标记，方便seekg返回这里重新读
//                 if(!std::getline(inputFile, line))break;
//                 // 去除前后的空白字符
//                 line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//                 line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//                 if (line.empty())
//                     continue;

//                 if (line[0] == '*')
//                 {
//                     inputFile.clear();
//                     inputFile.seekg(pos);
//                     break;
//                 }
//                 // 移除所有逗号
//                 line.erase(std::remove(line.begin(), line.end(), ','), line.end());
//                 std::istringstream iss(line);
//                 int element_id;
//                 iss >> element_id;

//                 if (element_id >= d_maxnum_element || element_id < 1)
//                 {
//                     std::cerr << "错误:元素ID超出范围: (过大或出现0及负值)" << element_id << std::endl;
//                     exit(0);
//                     continue;
//                 }
//                 int node_id;
//                 std::vector<int> aelement;
//                 while (iss >> node_id)
//                 {
//                     // mesh_element[element_id - 1].push_back(node_id);
//                     aelement.push_back(node_id);
//                 }                
//                 if (!aelement.empty())
//                 {         
//                     d_element_connectivity.push_back(aelement);           
//                     ++d_actual_element_count;
//                     // element_ids[d_actual_element_count - 1] = element_id;
//                     d_element_external_ids.push_back(element_id);
//                     d_element_type.push_back(element_type);
//                     d_element_internal_ids[element_id] = d_actual_element_count - 1;
//                 }
//             }
//         }
//         else if (line_lower.substr(0, 5) == "*node")
//         {
//             while (true)
//             {
//                 pos = inputFile.tellg();
//                 if (!std::getline(inputFile, line))break;
//                 line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//                 line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//                 if (line.empty()) continue;
//                 if (line[0] == '*')
//                 {
//                     inputFile.clear();
//                     inputFile.seekg(pos);
//                     break;
//                 }
//                  // 移除所有逗号
//                 line.erase(std::remove(line.begin(), line.end(), ','), line.end());
//                 std::istringstream iss(line);
//                 int node_id;
//                 iss >> node_id;
//                 if (node_id >= d_maxnum_node || node_id < 1)
//                 {
//                     std::cerr << "错误:节点ID超出范围:" << node_id << std::endl;
//                     exit(0);
//                     continue;
//                 }

//                 double coordinate;
//                 vector<double> coordinates_of_one_node;
//                 while (iss >> coordinate)
//                 {
//                     // mesh_node[node_id - 1].push_back(coordinate);
//                     coordinates_of_one_node.push_back(coordinate);
//                 }                
//                 if (!coordinates_of_one_node.empty())
//                 {
//                     d_nodes_coordinate.push_back(coordinates_of_one_node);
//                     ++d_actual_node_count;
//                     d_node_external_ids.push_back(node_id);
//                     d_node_internal_ids[node_id] = d_actual_node_count - 1;
//                 }  
//             }
//         }
//         else if (line_lower.substr(0, 5) == "*nset")
//         {
//             int node_set_id;
            
//             // 读取下一行
//             if (!std::getline(inputFile, line))
//             {
//                 throw std::runtime_error("Error: Expected an integer on the next line, but no more lines are available.");
//             }
//             // 去除前后的空白字符
//             line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//             line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//             // 检查是否为空行
//             if (line.empty())
//             {
//                 throw std::runtime_error("Error: Expected an integer, but the line is empty.");
//             }
//             try
//             {
//                 node_set_id = std::stoi(line);
//             }
//             catch (const std::invalid_argument &)
//             {
//                 throw std::runtime_error("Error: Expected an integer, but found a non-integer value.");
//             }
//             catch (const std::out_of_range &)
//             {
//                 throw std::runtime_error("Error: Integer value out of range.");
//             }

//             if (d_node_sets.find(node_set_id) != d_node_sets.end())
//             {
//                 throw std::runtime_error("Error: Key already exists in the map.");
//             }

//             if (line.find_first_not_of("0123456789+-") != std::string::npos)
//             {
//                 std::cout << line << std::endl;
//                 throw std::runtime_error("Error: Expected a single integer, but found additional characters.");
//             }
//             while (true)
//             {
//                 pos = inputFile.tellg();
//                 if(!std::getline(inputFile, line))break;
//                 // 去除前后的空白字符
//                 line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//                 line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//                 if (line.empty())
//                     continue;

//                 if (line[0] == '*')
//                 {
//                     inputFile.clear();
//                     inputFile.seekg(pos);
//                     break;
//                 }
//                 // 移除所有逗号
//                 line.erase(std::remove(line.begin(), line.end(), ','), line.end());
//                 std::istringstream iss(line);
//                 int node_id;
//                 while (iss >> node_id)
//                 {
//                     d_node_sets[node_set_id].push_back(node_id);
//                 }
//             }
//         }
//         else if (line_lower.substr(0, 6) == "*elset" )
//         {
//             int element_set_id;
            
//             // 读取下一行
//             if (!std::getline(inputFile, line))
//             {
//                 throw std::runtime_error("Error: Expected an integer on the next line, but no more lines are available.");
//             }
//             // 去除前后的空白字符
//             line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//             line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//             // 检查是否为空行
//             if (line.empty())
//             {
//                 throw std::runtime_error("Error: Expected an integer, but the line is empty.");
//             }
//             try
//             {
//                 element_set_id = std::stoi(line);
//             }
//             catch (const std::invalid_argument &)
//             {
//                 throw std::runtime_error("Error: Expected an integer, but found a non-integer value.");
//             }
//             catch (const std::out_of_range &)
//             {
//                 throw std::runtime_error("Error: Integer value out of range.");
//             }

//             if (d_element_sets.find(element_set_id) != d_element_sets.end())
//             {
//                 throw std::runtime_error("Error: Key already exists in the ElementSet map.");
//             }

//             if (line.find_first_not_of("0123456789+-") != std::string::npos)
//             {
//                 std::cout << line << std::endl;
//                 throw std::runtime_error("Error: Expected a single integer, but found additional characters.");
//             }
//             while (true)
//             {
//                 pos = inputFile.tellg();
//                 if(!std::getline(inputFile, line))break;
//                 // 去除前后的空白字符
//                 line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//                 line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//                 if (line.empty())
//                     continue;

//                 if (line[0] == '*')
//                 {
//                     inputFile.clear();
//                     inputFile.seekg(pos);
//                     break;
//                 }
//                 // 移除所有逗号
//                 line.erase(std::remove(line.begin(), line.end(), ','), line.end());
//                 std::istringstream iss(line);
//                 int element_id;
//                 while (iss >> element_id)
//                 {
//                     d_element_sets[element_set_id].push_back(element_id);
//                 }
//             }
//         }
//         else if (line_lower.substr(0, 12) == "*segment_set")
//         {
//             int segment_set_id;
            
//             // 读取下一行
//             if (!std::getline(inputFile, line))
//             {
//                 throw std::runtime_error("Error: Expected an integer on the next line, but no more lines are available.");
//             }
//             // 去除前后的空白字符
//             line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//             line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//             // 检查是否为空行
//             if (line.empty())
//             {
//                 throw std::runtime_error("Error: Expected an integer, but the line is empty.");
//             }
//             try
//             {
//                 segment_set_id = std::stoi(line);
//             }
//             catch (const std::invalid_argument &)
//             {
//                 throw std::runtime_error("Error: Expected an integer, but found a non-integer value.");
//             }
//             catch (const std::out_of_range &)
//             {
//                 throw std::runtime_error("Error: Integer value out of range.");
//             }

//             if (d_segment_sets.find(segment_set_id) != d_segment_sets.end())
//             {
//                 throw std::runtime_error("Error: Key already exists in the map.");
//             }

//             if (line.find_first_not_of("0123456789+-") != std::string::npos)
//             {
//                 std::cout << line << std::endl;
//                 throw std::runtime_error("Error: Expected a single integer, but found additional characters.");
//             }
//             while (true)
//             {
//                 pos = inputFile.tellg();
//                 if(!std::getline(inputFile, line))break;
//                 // 去除前后的空白字符
//                 line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
//                 line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
//                 if (line.empty())
//                     continue;

//                 if (line[0] == '*')
//                 {
//                     inputFile.clear();
//                     inputFile.seekg(pos);
//                     break;
//                 }
//                 // 移除所有逗号
//                 line.erase(std::remove(line.begin(), line.end(), ','), line.end());
//                 std::istringstream iss(line);
//                 int node_id;
//                 std::vector<int> asegment;
//                 while (iss >> node_id)
//                 {
//                     // mesh_element[element_id - 1].push_back(node_id);
//                     asegment.push_back(node_id);
//                 }                
//                 if (!asegment.empty())
//                 {         
//                     d_segment_sets[segment_set_id].push_back(asegment);
//                 }
//             }
//         }
//     }
//     inputFile.close();
//     buildElementsOfNodes();
//     std::cout << "the current model containts: " << std::endl;
//     std::cout << "    elements:" << d_actual_element_count << std::endl;
//     std::cout << "    nodes:" << d_actual_node_count << std::endl;
// }


