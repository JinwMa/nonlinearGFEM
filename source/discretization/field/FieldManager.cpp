

#include "FieldManager.h"


 template<typename T>
 void FieldManager::createElementField(std::string name,
                         int group,
                         T defaultValue)
{
    if (d_all_fields.find(name) != d_all_fields.end() )
    {
        toolbox::error(name + " has already built");
    }
    std::shared_ptr<ElementFieldData<T>> element_field = make_shared<ElementFieldData<T>>(name, group);
    d_all_fields[name] = element_field;
    auto & element_field_data = element_field->getData();
    // 初始化一个全局单元场
    int elementNum = d_mda->getElementNum();
    int length = elementNum * group;
    element_field_data.resize(length);
    for (int i = 0; i < length; i++)
    {
        element_field_data[i] = defaultValue;
    }
}

template<typename T>
void FieldManager::createNodeField(std::string name,
                                   int group,
                                   T defaultValue)
{
    if (d_all_fields.find(name) != d_all_fields.end() )
    {
        toolbox::error(name + " has already built");
    }
    std::shared_ptr<NodeFieldData<T>> node_field = make_shared<NodeFieldData<T>>(name, group);
    d_all_fields[name] = node_field;
    auto & node_field_data = node_field->getData();
    // 初始化一个全局节点场
    int nodeNum = d_mda->getNodeNum();
    int length = nodeNum * group;
    node_field_data.resize(length);
    for (int i = 0; i < length; i++)
    {
        node_field_data[i] = defaultValue;
    }

}

template<typename T>
void FieldManager::printNodeFieldData(std::string name)
{
    // 检查字段是否存在
    auto it = d_all_fields.find(name);
    if (it == d_all_fields.end()) {
        toolbox::error("Field " + name + " is not built");
        return;
    }
    
    // 安全的类型转换
    auto field = std::static_pointer_cast<NodeFieldData<T>>(it->second);
    if (!field) {
        toolbox::error("Field " + name + " has incorrect type for the requested template parameter");
        return;
    }
    
    // 打印数据
    const auto& field_data = field->getData();
    std::cout << "Field: " << name << " (size: " << field_data.size() << ")" << std::endl;
    for (const auto& value : field_data) {
        std::cout << value << std::endl;
    }
}

template<typename T>
void FieldManager::printElementFieldData(std::string name)
{
    // 检查字段是否存在
    auto it = d_all_fields.find(name);
    if (it == d_all_fields.end()) {
        toolbox::error("Field " + name + " is not built");
        return;
    }
    
    // 安全的类型转换
    auto field = std::static_pointer_cast<ElementFieldData<T>>(it->second);
    if (!field) {
        toolbox::error("Field " + name + " has incorrect type for the requested template parameter");
        return;
    }
    
    // 打印数据
    const auto& field_data = field->getData();
    std::cout << "Field: " << name << " (size: " << field_data.size() << ")" << std::endl;
    for (const auto& value : field_data) {
        std::cout << value << std::endl;
    }
}










template void FieldManager::printNodeFieldData<bool>   (std::string name);
template void FieldManager::printNodeFieldData<double> (std::string name);
template void FieldManager::printNodeFieldData<int>    (std::string name);
template void FieldManager::printNodeFieldData<float>  (std::string name);


template void FieldManager::createNodeField<bool>  (std::string, int, bool);      
template void FieldManager::createNodeField<double>(std::string, int, double);
template void FieldManager::createNodeField<int>   (std::string, int, int);
template void FieldManager::createNodeField<float> (std::string, int, float);


template void FieldManager::printElementFieldData<bool>   (std::string name);
template void FieldManager::printElementFieldData<double> (std::string name);
template void FieldManager::printElementFieldData<int>    (std::string name);
template void FieldManager::printElementFieldData<float>  (std::string name);


template void FieldManager::createElementField<bool>  (std::string, int, bool);      
template void FieldManager::createElementField<double>(std::string, int, double);
template void FieldManager::createElementField<int>   (std::string, int, int);
template void FieldManager::createElementField<float> (std::string, int, float);
