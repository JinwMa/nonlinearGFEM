#ifndef NODEFIELDDATA_H
#define NODEFIELDDATA_H

#include "BaseFieldData.h"

template<typename T>
class NodeFieldData : public BaseFieldData<T>
{

    public:
        NodeFieldData(std::string name,
                      int group = 1) : BaseFieldData<T>(name, group)
        {           
        }
        virtual ~NodeFieldData(){};


};




#endif