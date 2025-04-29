#include "BaseConstraint.h"
#include "toolbox.h"

void BaseConstraint::addDofToDofSet(const int nodeId,
                                       const string dof,
                                       shared_ptr<set<int>> allSlaveDofs)
{
    if (nodeId < 0 || nodeId > d_mesh->d_actual_node_count)
    {
        toolbox::error("node id is node in the range of mesh");
    }
    if (d_dof_map->d_Tag2Int.find(dof) == d_dof_map->d_Tag2Int.end())
    {
        toolbox::error("dof lab " + dof + " is not right");
    }
    int dof_index = d_dof_map->getDofIndex(nodeId, dof);
    allSlaveDofs->insert(dof_index);    
}

bool BaseConstraint::checkIfdofTouchedMasterOrSlave(const int nodeId,
                                                    const string dof,
                                                    shared_ptr<set<int>> DofSet)
{
    if (nodeId < 0 || nodeId > d_mesh->d_actual_node_count)
    {
        toolbox::error("node id is node in the range of mesh");
    }
    if (d_dof_map->d_Tag2Int.find(dof) == d_dof_map->d_Tag2Int.end())
    {
        toolbox::error("dof lab " + dof + " is not right");
    }
    int dof_index = d_dof_map->getDofIndex(nodeId, dof);
    if (DofSet->find(dof_index) != DofSet->end())
    {
        return true;
    }    
    else
    {
        return false;
    }
}
