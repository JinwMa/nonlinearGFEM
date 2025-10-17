#ifndef FIELDMANAGER_H
#define FIELDMANAGER_H

#include "BaseFieldData.h"
#include "NodeFieldData.h"
#include "ElementFieldData.h"


class FieldManager
{
    public:
    FieldManager();
    virtual ~FieldManager();

    template<typename T>
    void createNodeField(std::string name,
                         int group,
                         T defaultValue);

    private:
    std::map<std::string, std::shared_ptr<BaseField>> d_all_fields;

};



#endif