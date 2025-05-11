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
    d_elementAssembler->preBuildNodesDofs();
    d_constraint_manger->buildNodeDofs();
    d_dof_map->buildNodeIndex();

    // 建立约束方程 
    d_constraint_manger->buildConstrintEquation();

    // 建立自由度映射表
    d_constraint_manger->buildDofMap();

    if(std::getenv("PRINTCONSTRAINTEQUATIONS"))  d_constraint_manger->printConstraintEquations();


    // allocate matrix and vector
    d_Matrix["K"] = make_shared<SparseMatrix>("K");
    d_Vectors["u"] = make_shared<SparseVector>("u");
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














