#ifndef BASEFIELDDATA_H
#define BASEFIELDDATA_H

#include<string>
#include<map>
template<typename T>
class BaseFieldData
{
    public:
    BaseFieldData(){};
    virtual ~BaseFieldData(){};

    std::string name;
    int d_group;

}






#endif