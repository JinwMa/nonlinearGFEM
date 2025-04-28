#include "StructuralAtomSolver.h"



void StructuralAtomSolver::takeDB()
{

}


void StructuralAtomSolver::initializeSolver()
{
    // 迭代控制器
    d_control_param = make_shared<ObjectControlParam>();
    // 自由度映射表
    d_dof_map = make_shared<DofMap>(d_mesh);
    // 约束管理器
    d_constraint_manger = make_shared<ConstraintManager>(d_solve_db, d_mesh, d_dof_map);
    // 组装代理
    d_elementAssembler = make_shared<ElementAssembler>(d_solve_db, d_mesh, d_dof_map);    
    // 初始化ElementData
    d_elementAssembler->allocateElementData(d_elementData);

    // 预处理DofMap
    // std::cout << "befor" << std::endl;
    // d_dof_map->printNodesDofs();
    d_elementAssembler->preBuildNodesDofs();
    d_constraint_manger->buildNodeDofs();
    d_dof_map->buildNodeIndex();
    // std::cout << "after" << std::endl;
    // d_dof_map->printNodesDofs();
}

void StructuralAtomSolver::initializeTimeStep(const double current_time)
{

}

void StructuralAtomSolver::initializeIterationStep()
{

}

void StructuralAtomSolver::solveOneTimeStep(const double current_time,
                                            const double predict_dt,
                                            double &actual_dt)
{

}


void StructuralAtomSolver::afterSolve()
{

}














