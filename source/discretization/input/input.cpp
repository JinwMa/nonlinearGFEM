#include "input.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
// #include <regex>

int Input::check_line(const string &line)
{
    std::string line_temp = line;
    auto size = line.size();                                // line 长度
    size_t pos = 0;                                         // 第一个非空字符的位置
    while (pos < line.size() && std::isspace(line[pos])) {
        pos++;
    }
    size_t equal_pos = line.find('=');                      // 等号的位置
    // 规则 1: 空行
    // if (std::regex_match(line, std::regex("^\\s*")))
    // {
    //     return 1;
    // }

    if (line.empty()) return 1;

    // 规则 2: 注释行（以 // 开头）
    // if (std::regex_match(line, std::regex("^\\s*[\\/]{2}")))
    // {
    //     return 2;
    // }

    if (line.substr(0,2) == "//") return 2;

    // 规则 3: 单独一个 {
    // if (std::regex_match(line, std::regex("^\\s*\\{{1}\\s*")))
    // {
    //     return 3;
    // }

    

    if (line[0] == '{' && line.size() ==1) return 3;

    // 规则 4: 单独一个 }
    // if (std::regex_match(line, std::regex("^\\s*}{1}\\s*$")))
    // {
    //     return 4;
    // }

    if (line[0] == '}' && line.size() ==1) return 4;


    // if (line[pos] == '}' && size == (pos + 1)) return 4;

    // 规则 5: 一串字符，中间没有空格
    // if (std::regex_match(line, std::regex("^\\s*\\w+\\s*$")))
    // {
    //     return 5;
    // }
    int if5 = 0;
    for (size_t i = pos; i < size; i++)
    {
        char ch = line[i];
        if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '_') 
        {
            if5++;
            break;
        }
        
        if (ch == '=') 
        {
            if5++;
            break;
        }
    }
    if(if5 == 0) return 5;


    // key = value1, value2, value3
    if (equal_pos != std::string::npos)
    {
        std::string key = line.substr(0, equal_pos);
        std::string values = line.substr(equal_pos + 1);
        deleteSpace(key);
        deleteSpace(values);
        if (key.size() > 0 && values.size() > 0) return 6;
    }

    // // 规则 6: key=string1, string2, string3 形式的数据记录
    // if (std::regex_match(line, std::regex("^\\s*\\w+\\s*=\\s*[a-zA-Z]+\\w*(\\s*,\\s*[a-zA-Z]+\\w*)*\\s*$")))
    // {
    //     return 6;
    // }

    // // 规则 7: key=double1/int1, double2/int2, double3/int3 形式的数据记录
    // if (std::regex_match(line, std::regex("^\\s*\\w+\\s*=\\s*-?\\d+\\.?\\d*(\\s*,\\s*-?\\d+\\.?\\d*)*\\s*$")))
    // {
    //     return 7;
    // }

    // // 规则 8: key=路径 形式的数据记录
    // if (std::regex_match(line, std::regex("^\\s*\\w+\\s*=\\s*[\\/\\.\\w]+.*[\\/\\.\\w]+\\s*$")))
    // {
    //     return 8;
    // }

    // 如果以上规则都不匹配，则返回 false
    return 0;
}

void Input::checkInput(const string & filename)
{
    std::ifstream inputFile(filename); // 打开文件 
    if (!inputFile)
    {
        std::cerr << "Cannot open file: " << filename << std::endl;
        exit(0);
    }   
    std::string line;
    std::string line_last, line_now, line_next;
    //当前行行号
    int line_num = 0;  
    line_last = line;  
    deleteSpace(line_last);
    int num_common = 0;    
    while (std::getline(inputFile, line)) //读取当前行
    {
        line_num++;  //获取当前行号
        //仅取注释之前的输入
        std::size_t commentPos = line.find("//");
        if (commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos); // 保留注释符号前的部分
        }
        //对当前行进行检查
        //首先去除line前后的空格
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        if(!check_line(line))
        {
            std::cout << "Input file error - line number: " << line_num << " input does not conform to rules" << std::endl;
            std::cout << line << std::endl;
            exit(0);
        }
        if (line == "{")
        {
            num_common++;
            // if (!std::regex_match(line_last, std::regex("^\\s*\\w+\\s*$")))
            if (check_line(line_last) != 5)
            {
                std::cout << "Input file error - line number: " << line_num << " previous line should be a data block name" << std::endl;
                std::cout << line << std::endl;
                exit(0);
            }
        }
        if (line == "}")
        {
            num_common--;
        }
        if (check_line(line) == 5)
        {
            std::streampos pos = inputFile.tellg();
            std::getline(inputFile, line_next);
            deleteSpace(line_next);
            if (check_line(line_next) != 3)
            {
                std::cout << "Input file error - line number: " << line_num << " next line should be a data block wrapped in curly braces" << std::endl;
                exit(0);
            }
            inputFile.clear();
            inputFile.seekg(pos);
        }
        line_last = line;
    }    
    if (!num_common==0)
    {
        std::cout << "Input file error: data block not closed" << std::endl;
        std::cout << line << std::endl;
        exit(0);
    }
}

void Input::read(const string & filename)
{
    // 根数据块
    d_root_db = std::make_shared<DataBase>();
    // 根数据块没有父节点
    auto current_db = d_root_db;    
    current_db->d_name = "root data base";

    int layer = 0;
    current_db->d_layer = layer;
    current_db->d_root_db = d_root_db;
    std::ifstream inputFile(filename); // 打开文件
    if (!inputFile)
    {
        std::cerr << "Cannot open file: " << filename << std::endl;
        exit(0);
    }   
    std::string line;    
    std::string line_last;
    int line_num = 0;
    while(std::getline(inputFile, line)) //
    {
        line_num++;
        //仅取注释之前的输入
        std::size_t commentPos = line.find("//");
        if (commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos); // 保留注释符号前的部分
        }
        deleteSpace(line);                     // 删除字符前后的空格
        int line_type = check_line(line);
        if(line_type == 1 || line_type == 2 || line_type == 3)
        {
            // 无需处理
        }
        else if(line_type == 4)
        {
            // }: 数据块的指针指向上一层
            current_db = current_db->d_father_db;
            layer--;
        }
        else if(line_type == 5)
        {
            layer++;
            // 进入新的数据模块
            std::string common_name = line;
            // 去除common_name前后的空白字符
            common_name.erase(common_name.find_last_not_of(" \t\n\r\f\v") + 1);
            common_name.erase(0, common_name.find_first_not_of(" \t\n\r\f\v")); 
            // new一个智能指针指向子数据块
            auto son_db = make_shared<DataBase>();
            // 新的db块的父指针要指向当前数据块
            son_db->d_father_db = current_db;
            // 新db块的名字
            son_db->d_name = common_name;
            // 把新产生的数据块塞到当前数据库的子数据块中
            current_db->d_son_dbs[common_name] = son_db;
            // 把新数据块的名字统计进当前数据块的名单中
            current_db->d_all_keys.insert(common_name);
            // 当前数据块替换成子数据块
            current_db = son_db;
            current_db->d_layer = layer;
            current_db->d_root_db = d_root_db;
        }
        else if (line_type == 6 )
        {
            std::size_t equalPos = line.find('=');
            if (line.find('=') == std::string::npos)
            {
                std::cerr << "Error: equal sign not found in line: " << line << std::endl;
            }
            std::string key = line.substr(0, equalPos);
            std::string values = line.substr(equalPos + 1);
            // 去除key前后的空白字符
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
            if (key.empty() || key.find(' ') != std::string::npos)
            {
                std::cerr << "Error: key must be unique and cannot contain spaces: " << line << std::endl;
                exit(0);
            }
            if (current_db->d_all_keys.find(key) != current_db->d_all_keys.end())
            {
                std::cerr << "Data block: " << current_db->d_name << " in " << "Error: key must be unique, duplicate key found: " << key << std::endl;
                exit(0);
            }
            current_db->d_all_keys.insert(key);
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
                        std::cerr << "Error: value cannot contain spaces: " << subValue << std::endl;
                        invalidValue = true;
                        break;
                    }
                    valueList.push_back(subValue);
                }
            }
            current_db->d_key_values[key] = valueList;
        }
        else
        {
            std::cout << line_type << std::endl;
            std::cout << "Unsupported input type: " << line_num << " line" << std::endl;
            exit(0);
        }
    }
}


int DataBase::getInt(std::string name)
{
    auto it = d_key_values.find(name);
    if (it == d_key_values.end())
    {
        std::cout << "not found " << name << " in input" << std::endl;
        exit(1);
    }
    if (it->second.size() != 1)
    {
        std::cout << "the size of value of " << it->first << " is more than one" << std::endl;
    }
    int num = std::atoi(it->second[0].c_str());
    return num;
}

double DataBase::getDouble(std::string name)
{
    auto it = d_key_values.find(name);
    if (it == d_key_values.end())
    {
        std::cout << "not found " << name << " in input" << std::endl;
        exit(1);
    }
    //判断value是不是只有一个值
    if (it->second.size() != 1)
    {
        std::cout << "the size of value of " << it->first << " is more than one" << std::endl;
    }
    double value = std::stod(it->second[0].c_str());
    return value;    
}

std::vector<int> DataBase::getVectorInt(std::string name)
{
    std::vector<int> nums;
    auto it = d_key_values.find(name);
    if (it == d_key_values.end())
    {
        std::cout << "not found " << name << " in input" << std::endl;
        exit(1);
    }
    if (it->second.size() <= 0)
    {
        std::cout << "the size of value of " << it->first << " is less than one" << std::endl;
        exit(1);
    }
    int count = it->second.size();
    for (int i = 0; i < count; i++)
    {
        std::string temp = it->second[i];
        int num = std::atoi(temp.c_str());
        nums.push_back(num);
    }
    return nums;    
}

std::vector<double> DataBase::getVectorDouble(std::string name)
{
    std::vector<double> values;
    auto it = d_key_values.find(name);
    if (it == d_key_values.end())
    {
        std::cout << "not found " << name << " in input" << std::endl;
        exit(1);
    }
    if (it->second.size() <= 0)
    {
        std::cout << "the size of value of " << it->first << " is less than one" << std::endl;
        exit(1);
    }
    int count = it->second.size();
    for (int i = 0; i < count; i++)
    {
        std::string temp = it->second[i];
        double value = std::stod(temp.c_str());
        values.push_back(value);
    }
    return values;    
}

std::string DataBase::getString(string name)
{
    auto it = d_key_values.find(name);
    if (it == d_key_values.end())
    {
        std::cout << "not found " << name << " in input" << std::endl;
        exit(1);
    }
    //判断value是不是只有一个值
    if (it->second.size() != 1)
    {
        std::cout << "the size of value of " << it->first << " is more than one" << std::endl;
    }
    string str = it->second[0];
    return str;    
}

std::vector<std::string> DataBase::getVectorString(std::string name)
{
    std::vector<std::string> strings;
    auto it = d_key_values.find(name);
    if (it == d_key_values.end())
    {
        std::cout << "not found " << name << " in input" << std::endl;
        exit(1);
    }
    if (it->second.size() <= 0)
    {
        std::cout << "the size of value of " << it->first << " is less than one" << std::endl;
        exit(1);
    }
    int count = it->second.size();
    for (int i = 0; i < count; i++)
    {
        std::string temp = it->second[i];
        strings.push_back(temp);
    }
    return strings;    
}

std::shared_ptr<DataBase> DataBase::getDataBase(std::string name)
{
    auto it = d_son_dbs.find(name);
    if (it == d_son_dbs.end())
    {
        std::cout << "not found " << name << " in block: " << d_name << std::endl;
        exit(1);
    }
    return it->second;
}

bool DataBase::ifExist(std::string name)
{
    auto it = d_all_keys.find(name);
    if (it != d_all_keys.end()) return true;
    else return false;    
}



void Input::deleteSpace(std::string & str)
{
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
}

