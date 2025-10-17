#ifndef BASEFIELDDATA_H
#define BASEFIELDDATA_H

#include<string>
#include<map>
#include "MeshDataAgent.h"



class BaseField
{
    public:
    BaseField() = default;
    virtual ~BaseField() = default;
};


template<typename T>
class BaseFieldData : public BaseField
{
    public:
        BaseFieldData(std::string name,
                      int group = 1) {
            d_name = name;
            d_group = group;       
        };
        virtual ~BaseFieldData() {};
        // virtual void initFieldData() = 0;

        inline std::string getFieldName()
        {return d_name;}

        inline int getGroup()
        {return d_group;}

        inline int getLength()
        {return d_group * d_field_data.size();}

        
        T* getDataPointer()
        {
            if(d_field_data.size() == 0)
            {
                std::cout << "Field: " << d_name << " does not initialized" << std::endl;
                exit(1);
            }
            return d_field_data.data();
        }

        std::vector<T> & getData()
        {
            return d_field_data;
        }

    private:    
    std::string d_name;
    int d_group;
    int d_length;
    std::vector<T> d_field_data;
    T defaultVaule;
    

};






#endif