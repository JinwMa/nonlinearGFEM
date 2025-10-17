

#include "FieldManager.h"

template<typename T>
void FieldManager::createNodeField(std::string name,
                                   int group,
                                   T defaultValue)
{
    std::shared_ptr<NodeFieldData<T>> node_field = make_shared<NodeFieldData<T>>(name, group);
}