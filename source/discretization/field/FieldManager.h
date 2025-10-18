#ifndef FIELDMANAGER_H
#define FIELDMANAGER_H

#include "BaseFieldData.h"
#include "NodeFieldData.h"
#include "ElementFieldData.h"


class FieldManager
{
    public:
    FieldManager(std::shared_ptr<MeshDataAgent> mda) : d_mda(mda)
    {
    }
    virtual ~FieldManager() {};

    template<typename T>
    void createNodeField(std::string name,
                         int group,
                         T defaultValue);

    template<typename T>
    void printNodeFieldData(std::string name);


    template<typename T>
    void createElementField(std::string name,
                         int group,
                         T defaultValue);

    template<typename T>
    void printElementFieldData(std::string name);

    private:
    std::map<std::string, std::shared_ptr<BaseField>> d_node_fields;
    std::map<std::string, std::shared_ptr<BaseField>> d_element_fields;
    std::shared_ptr<MeshDataAgent> d_mda;

};



#endif