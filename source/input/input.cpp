#include "input.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>

void Input::read_input_file(const string filename)
{
    std::ifstream inputFile(filename); // 打开文件
    if (!inputFile)
    {
        std::cerr << "无法打开文件:" << filename << std::endl;
        exit(0);
    }
    std::string line;
    while (std::getline(inputFile, line))
    { // 按行读取文件内容
        // 查找注释符号的位置
        std::size_t commentPos = line.find("//");
        if (commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos); // 保留注释符号前的部分
        }
        if (!line.empty())
        { // 如果不是空行,则检查
            std::size_t equalPos = line.find('=');
            if (line.find('=') == std::string::npos)
            {
                std::cerr << "错误:行中没有找到等号:" << line << std::endl;
            }

            std::string key = line.substr(0, equalPos);
            std::string values = line.substr(equalPos + 1);

            // 去除key前后的空白字符
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));

            if (key.empty() || key.find(' ') != std::string::npos)
            {
                std::cerr << "错误:键必须是唯一且不能包含空格:" << line << std::endl;
                continue;
            }

            if (keys.find(key) != keys.end())
            {
                std::cerr << "错误:键必须是唯一的,发现重复键:" << key << std::endl;
                continue;
            }
            keys.insert(key);

            // 去除values前后的空白字符
            values.erase(values.find_last_not_of(" \t\n\r\f\v") + 1);
            values.erase(0, values.find_first_not_of(" \t\n\r\f\v"));

            std::vector<std::string> valueList;
            std::stringstream ss(values);
            std::string value;
            bool invalidValue = false;
            while (std::getline(ss, value, ','))
            {
                // 分割每个用逗号分隔的部分
                std::stringstream valueStream(value);
                std::string subValue;
                while (std::getline(valueStream, subValue, ';'))
                {
                    // 去除subValue前后的空白字符
                    subValue.erase(subValue.find_last_not_of(" \t\n\r\f\v") + 1);
                    subValue.erase(0, subValue.find_first_not_of(" \t\n\r\f\v"));
                    if (subValue.find(' ') != std::string::npos)
                    {
                        std::cerr << "错误:值中不能包含空格:" << subValue << std::endl;
                        invalidValue = true;
                        break;
                    }
                    valueList.push_back(subValue);
                }
            }
            db[key] = valueList;
        }
    }

    inputFile.close(); // 关闭文件
}