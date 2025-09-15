#include"ConstraintManager.h"

void ConstraintManager::takeDB()
{
    std::vector<string> constraint_list = d_db->getVectorString("refer_constraint_list");
    for (int i = 0; i < constraint_list.size(); i++)
    {
        std::string iconstraint = constraint_list[i];
        auto db = d_db->d_root_db->getDataBase("modal_market")->getDataBase(iconstraint);
        std::string constraint_type = db->getString("type");
        auto constraint = newConstraint(constraint_type, db, d_mesh, d_dof_map);
        d_constraints.push_back(constraint);
    } 
    // 循环调用每个约束的takeDB
    for (auto constraint : d_constraints)
    {
        constraint->takeDB();
    }   
}

void ConstraintManager::buildNodeDofs()
{
    if (d_constraints.size() == 0) toolbox::error("no constraint in this input");
    for (auto constraint : d_constraints)
    {
        constraint->buildNodeDofs();
    }
}


void ConstraintManager::buildConstrintEquation()
{
    d_SlaveSetPointer = make_shared<set<int>>();
    d_MasterSetPointer = make_shared<set<int>>();
    for (auto constraint : d_constraints)
    {
        std::vector<ConstraintEquation> Ces;
        constraint->buildConstraintEquations(Ces, d_SlaveSetPointer, d_MasterSetPointer);
        d_CEs_vec.insert(d_CEs_vec.end(), Ces.begin(), Ces.end());
    }   
}

void ConstraintManager::printConstraintEquations()
{
    std::cout << "number of CEs: " << d_CEs_vec.size() << std::endl;
    for (auto CE : d_CEs_vec)
    {
        // ce id 
        std::cout << " CE id =  " << CE.equation_id << std::endl;
        // equation:
        std::cout << CE.constant << " = ";
        std::cout << CE.slave_factor << " * " << d_mesh->getNodeExternalId(CE.slave_node_id) << CE.slave_dof;
        int numMaster = CE.master_dofs.size();
        for (int i = 0; i < numMaster; i++)
        {
            std::cout << " + " << CE.master_factors[i] << " * " << CE.master_node_ids[i] << CE.master_dofs[i];
        } 
        std::cout << std::endl;
        std::cout << "#######################################" << std::endl;
    }
}

void ConstraintManager::buildDofMap()
{
    std::cout << "ConstraintManager->buildDofMap ..." << std::endl;

    // 自由度标签数组开空间
    d_dof_map->d_DofType.resize(d_dof_map->d_numAllDofs);
    for (int i = 0; i < d_dof_map->d_numAllDofs; i++) d_dof_map->d_DofType[i] = 0;
    // 
    d_dof_map->d_All2Solver.resize(d_dof_map->d_numAllDofs);
    d_dof_map->d_All2MasterSlave.resize(d_dof_map->d_numAllDofs);


    // 1 tag all dofs
    // 循环所有的约束方程(1) 给约束方程编号 (2) 给自由度打标签
    
    int CEId = 0;
    for (auto & it : d_CEs_vec)
    {
        it.equation_id = CEId;
        CEId++;
        int dofIndex = d_dof_map->getDofIndex(it.slave_node_id, it.slave_dof);
        d_dof_map->d_DofType[dofIndex] = -1;
    }

    // 2 build dofMap
    // 循环所有自由度 0 +1; -1 -1
    int dof_Master_index = 0;
    int dof_Slave_index = 0;
    for (int i = 0; i < d_dof_map->d_numAllDofs; i++)
    {
        if(d_dof_map->d_DofType[i] >= 0) // master
        {
            d_dof_map->d_All2MasterSlave[i] = dof_Master_index;
            dof_Master_index++;            
        }
        else                             // slave
        {
            dof_Slave_index--;
            d_dof_map->d_All2MasterSlave[i] = dof_Slave_index;
        }
    }
    d_dof_map->d_numMasterDofs = dof_Master_index;
    d_dof_map->d_numSlaveDofs = -dof_Slave_index;

    for (int i = 0; i < d_dof_map->d_numAllDofs; i++)
    {
        if(d_dof_map->d_All2MasterSlave[i] < 0) d_dof_map->d_All2Solver[i] = -d_dof_map->d_All2MasterSlave[i] + d_dof_map->d_numMasterDofs;
        else
        {
            d_dof_map->d_All2Solver[i] = d_dof_map->d_All2MasterSlave[i];
        } 

        // std::cout << " " << d_dof_map->d_All2MasterSlave[i] << "  " << d_dof_map->d_All2Solver[i] << std::endl;
    }

}