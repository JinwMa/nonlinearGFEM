#ifndef GFEM_DOF_MAP_H
#define GFEM_DOF_MAP_H

#include "Entity.h"
#include "MeshDataAgent.h"

#include <string>
namespace DofTypes {
    enum class Dof_Tag
    {
        Ux,
        Uy,
        Uz,
        Rx,
        Ry,
        Rz,
        MAX
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
    int entity_id;
    DofTypes::Dof_Tag dofTag;

    // 重载小于号 用于去重
    bool operator<(const Dof &other) const
    {
        if (entity_type != other.entity_type)
            return entity_type < other.entity_type;
        if (entity_id != other.entity_id)
            return entity_id < other.entity_id;
        return dofTag < other.dofTag;
    }
};

class DofMap
{
    public:
    DofMap(std::shared_ptr<MeshDataAgent> mda) : d_mda(mda) {}
    virtual ~DofMap(){};

    inline int getDofIndex(int entity_id, DofTypes::Dof_Tag dofTag, Entity::EntityType et = Entity::EntityType::NODE)
    {        
        Dof dof{et, entity_id, dofTag};
        return getDofIdByDof(dof);
    }
    inline int getDofIndexByDof(Dof dof)
    {
        int dof_id = getDofIdByDof(dof);
        return d_dof_list[dof_id];
    }

    
    void addDof(Dof dof)
    {
        d_all_dofs.insert(dof);
    }
    void removeDof(Dof dof)
    {
        d_all_dofs.erase(dof);
    }
    bool isSlaveDof(Dof dof)
    {
        int dofId = getDofIdByDof(dof);
        if (d_slave_dof_ids.find(dofId) != d_slave_dof_ids.end()) return true;
        return false;
    }

    // 建立/刷新 自由度映射列表
    void buildDofMap();


    private:
    std::shared_ptr<MeshDataAgent> d_mda;
    // 节点自由度列表  每个节点上开了哪些自由度
    std::vector<std::set<DofTypes::Dof_Tag>> d_nodes_dofs;
    // 单元自由度列表  每个单元上开了哪些自由度
    std::vector<std::set<DofTypes::Dof_Tag>> d_elements_dofs;

    // 自由度列表  自由度的唯一标识--自由度在整个列表中的位置    id -> order
    std::unordered_map<long, int> d_dof_list;

    // 自由度池
    std::set<Dof> d_all_dofs;          // 存储  自由度  结构体
    std::set<long> d_all_dof_ids;      // 存储  所有   自由度id
    std::set<int> d_slave_dof_ids;     // 存储  从     自由度id
    std::set<int> d_master_dof_ids;    // 存储  主     自由度id


    // 获取dof id
    inline long getDofId(int entity_id, DofTypes::Dof_Tag dofTag, Entity::EntityType et = Entity::EntityType::NODE)
    {
        if (et == Entity::EntityType::NODE)
           return entity_id * DofTypes::to_int(DofTypes::Dof_Tag::MAX) + DofTypes::to_int(dofTag);
        else
           toolbox::error("not support this type of Entity for getDofId");

        return -1;
    }

    // 通过结构体获取dof id
    inline long getDofIdByDof (Dof dof)
    {
        int entity_id = dof.entity_id;
        DofTypes::Dof_Tag dofTag = dof.dofTag;
        Entity::EntityType et = dof.entity_type;
        return getDofId(entity_id, dofTag, et);
    }


};












#endif