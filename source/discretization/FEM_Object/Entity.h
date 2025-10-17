#ifndef GFEM_ENTITY_H
#define GFEM_ENTITY_H

#include <string>

namespace Entity
{
    enum class EntityType
    {
        NODE,
        ELEMENT,
        FACE,
        EDGE
    };
    inline int to_int(EntityType type)
    {
         return static_cast<int>(type);
    }
}




#endif