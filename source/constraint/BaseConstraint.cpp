#include "BaseConstraint.h"
#include "toolbox.h"

bool BaseConstraint::if_dof_in_mset(const int node_dof_index)
{
    auto it = m_set.find(node_dof_index);

    if (it != m_set.end())
        return true;
    else
        return false;
}

bool BaseConstraint::if_dof_in_mset(const int node_id, const string dof)
{
    int node_dof_index;
    if (dof == "ux")
        node_dof_index = (node_id - 1) * 6 + 0;
    else if (dof == "uy")
        node_dof_index = (node_id - 1) * 6 + 1;
    else if (dof == "uz")
        node_dof_index = (node_id - 1) * 6 + 2;
    else if (dof == "rx")
        node_dof_index = (node_id - 1) * 6 + 3;
    else if (dof == "ry")
        node_dof_index = (node_id - 1) * 6 + 4;
    else if (dof == "rz")
        node_dof_index = (node_id - 1) * 6 + 5;
    else
        toolbox::error("dof is inputed wrong");

    auto it = m_set.find(node_dof_index);
    if (it != m_set.end())
        return true;
    else
        return false;
}

void BaseConstraint::addToMset(const int node_dof_index)
{
    m_set.insert(node_dof_index);
}

void BaseConstraint::addToMset(const int node_id, const string dof)
{
    int node_dof_index;
    if (dof == "ux")
        node_dof_index = (node_id - 1) * 6 + 0;
    else if (dof == "uy")
        node_dof_index = (node_id - 1) * 6 + 1;
    else if (dof == "uz")
        node_dof_index = (node_id - 1) * 6 + 2;
    else if (dof == "rx")
        node_dof_index = (node_id - 1) * 6 + 3;
    else if (dof == "ry")
        node_dof_index = (node_id - 1) * 6 + 4;
    else if (dof == "rz")
        node_dof_index = (node_id - 1) * 6 + 5;
    else
        toolbox::error("dof is inputed wrong");
    m_set.insert(node_dof_index);
}

