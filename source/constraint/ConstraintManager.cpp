#include"ConstraintManager.h"
#include"toolbox.h"


using namespace std;

void ConstraintManager::takeDB(Input * pinput, Mesh * pmesh)
{
    //首先检查是否存在boundary_conditions
    d_boundary_conditions = pinput->getVectorString("boundary_conditions");    
    // 若存在,则一一读取
    for (size_t i = 0; i < d_boundary_conditions.size(); i++)
    {
        vector<ConstraintEquation> equations;
        auto constraint_name = d_boundary_conditions[i];
        auto constraint_type = pinput->getString(constraint_name + "_type");

        BaseConstraint *constrain;
        if (constraint_type == "SPC")
        {
            constrain = new SPC(constraint_name);            
        }
        else
        {
            toolbox::error("not support this type : " + constraint_type);
        }
        // 用all_m_set初始化约束子类中的m_set;
        constrain->m_set = all_m_set;
        constrain->takeDB(pinput, pmesh);
        constrain->buildDofMap();
        equations = constrain->buildEquations(pmesh);
        d_equations_num = d_equations_num + equations.size();

        // 把所有的约束自由度再塞回到总约束自由度集合中
        all_m_set.insert(constrain->m_set.begin(), constrain->m_set.end());
        delete constrain;
        FinalConstraintEquations.push_back(equations);
    }
}


Eigen::SparseMatrix<double> ConstraintManager::buildConstrintMatrix(Mesh * pmesh)
{
    //TODO:: 临时写死--第一个分支:实体单元
    int numdofs = pmesh->actual_node_count * NDIM;
    Eigen::SparseMatrix<double> C(d_equations_num, numdofs);
    C.setZero();
    std::vector<Eigen::Triplet<double>> tripletList;
    map<string, int> dof_map = { 
        {"ux", 1},
        {"uy", 2},
        {"uz", 3}
        };

    int equation_id = 0;

    for (size_t i = 0; i < FinalConstraintEquations.size(); i++)
    {
        auto equations = FinalConstraintEquations[i];
        for (size_t j = 0; j < equations.size(); j++)
        {
            auto equation = equations[j];
            int mid = equation.master_node_id;
            string mdof = equation.mater_node_dof;
            auto terms = equation.equationterms;
            for (size_t k = 0; k < terms.size(); k++)
            {
                int sid = terms[k].node_id;
                string sdof = terms[k].node_dof;
                double factor = terms[k].factor;
                int row, col;
                row = (sid - 1) * NDIM + dof_map[sdof] - 1;
                tripletList.push_back(Eigen::Triplet<double>(equation_id, row, factor));
            }
            equation_id++;
        }
    }
    if (equation_id != d_equations_num) toolbox::error("equation_id is wrong");
    for (const auto &triplet : tripletList)
    {
        C.coeffRef(triplet.row(), triplet.col()) += triplet.value();
    }
    // std::cout << C.transpose();
    return C;
}

Eigen::VectorXd ConstraintManager::buildConstrintForce(Mesh * pmesh)
{
    Eigen::VectorXd G(d_equations_num);
    G.setZero();

    int equation_id = 0;

    for (size_t i = 0; i < FinalConstraintEquations.size(); i++)
    {
        auto equations = FinalConstraintEquations[i];
        for (size_t j = 0; j < equations.size(); j++)
        {            
            auto equation = equations[j];
            G(equation_id) = equation.rhs;
            equation_id++;
        }
    }
    if (equation_id != d_equations_num) toolbox::error("equation_id is wrong");
    
    return G;
}