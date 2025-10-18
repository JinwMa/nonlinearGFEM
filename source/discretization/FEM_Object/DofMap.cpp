# include "DofMap.h"

void DofMap::buildDofMap()
{
    d_all_dof_ids.clear();
    for (auto dof : d_all_dofs){
        d_all_dof_ids.insert(getDofIdByDof(dof));
    }
    int count = 0;
    for (auto dofId : d_all_dof_ids)
    {
        d_dof_list[dofId] = count;
        count++;
    }
}