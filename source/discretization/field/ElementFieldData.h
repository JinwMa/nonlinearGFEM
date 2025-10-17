#ifndef ELEMENTFIELDDATA_H
#define ELEMENTFIELDDATA_H

#include "BaseFieldData.h"

template<typename T>
class ElementFieldData : public BaseFieldData<T>
{

    public:
        ElementFieldData(std::string name,
                      int group = 1) : BaseFieldData<T>(name, group)
        {           
        }
        virtual ~ElementFieldData(){};

};




#endif