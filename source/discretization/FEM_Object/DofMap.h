#ifndef GFEM_DOF_MAP_H
#define GFEM_DOF_MAP_H

#include "Entity.h"

#include <string>
namespace DofTypes {
    enum class Dof_Tag
    {
        Ux,
        Uy,
        Uz,
        Rx,
        Ry,
        Rz
    };
    
    // 转换为字符串
    inline const char* to_string(Dof_Tag dof) {
        switch(dof) {
            case Dof_Tag::Ux: return "Ux";
            case Dof_Tag::Uy: return "Uy";
            case Dof_Tag::Uz: return "Uz";
            case Dof_Tag::Rx: return "Rx";
            case Dof_Tag::Ry: return "Ry";
            case Dof_Tag::Rz: return "Rz";
            default: return "Unknown";
        }
    }
    
    // 判断类型
    inline bool is_translational(Dof_Tag dof) {
        return dof == Dof_Tag::Ux || dof == Dof_Tag::Uy || dof == Dof_Tag::Uz;
    }
    
    inline bool is_rotational(Dof_Tag dof) {
        return dof == Dof_Tag::Rx || dof == Dof_Tag::Ry || dof == Dof_Tag::Rz;
    }

    inline int to_int(Dof_Tag dof)
    {
         return static_cast<int>(dof);
    }
}


struct Dof
{
    Entity::EntityType entity_type;
    int node_id;
    DofTypes::Dof_Tag dofTag;
};

class DofMap
{
    public:
    DofMap(){};
    virtual ~DofMap(){};



    private:

};












#endif