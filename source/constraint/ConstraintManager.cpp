#include"ConstraintManager.h"

void ConstraintManager::takeDB()
{
    std::vector<string> constraint_list = d_db->getVectorString("refer_constraint_list");
    for (int i = 0; i < constraint_list.size(); i++)
    {
        std::string iconstraint = constraint_list[i];
        auto db = d_db->d_root_db->getDataBase("modal_market")->getDataBase("constraint")->getDataBase(iconstraint);
        std::string constraint_type = db->getString("type");
        if (true)
        {
            auto constraint = make_shared<BaseConstraint>(db, d_mesh, d_dof_map);
            d_constraints.push_back(constraint);
        }
        else
        {
            toolbox::error("not supprot this type of constraint");
        }
    } 
    // 循环调用每个约束的takeDB
    for (auto constraint : d_constraints)
    {
        constraint->takeDB();
    }   
}

void ConstraintManager::buildDofs()
{
    if (d_constraints.size() == 0) toolbox::error("no constraint in this input");
    for (auto constraint : d_constraints)
    {
        constraint->buildDofs();
    }
}


void ConstraintManager::buildConstrintEquation()
{
    for (auto constraint : d_constraints)
    {
        std::vector<ConstraintEquation> Ces;
        constraint->buildConstraintEquations(Ces);
        d_CEs_vec.insert(d_CEs_vec.end(), Ces.begin(), Ces.end());
    }
}