#include "NonLinearStaticSolver.h"


void NonLinearStaticSolver::initialize_solver(Input * pinput, Mesh * pmesh)
{
    int elements_num = pmesh->actual_element_count;
    d_elements_data.resize(elements_num);
    for (int i = 0; i < elements_num; i++)
    {
        d_elements_data[i].if_element_is_initialized = false;
    }
}


void NonLinearStaticSolver::solve(Input * pinput, Mesh * pmesh)
{
    Eigen::SparseMatrix<double> K;
    // 定义和初始化自由度映射列表
    Dof_Map DofMap(pmesh);
    Dof_Map * pdofmap = &DofMap;  
    
    std::cout << "building DofMap" << std::endl;    
    pdofmap->BuildDofMap(pmesh);
    std::cout << "complete DofMap" << std::endl;


    initialize_solver(pinput, pmesh);
    auto * pelementassembler = new ElementAssembler;
    pelementassembler->takeDB(pinput, pmesh, pdofmap); //读单元列表
    // 构造刚度矩阵,构造右端项
    std::cout << "building stiffness" << std::endl;
    pelementassembler->assembleNonLinearElementStiffness(pinput, pmesh, pdofmap, d_elements_data, K);
    std::cout << "complete the stiffness " << std::endl;   
    //释放组装器指针
    delete pelementassembler;
}