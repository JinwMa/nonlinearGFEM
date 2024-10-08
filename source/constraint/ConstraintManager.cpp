#include"ConstraintManager.h"
#include"toolbox.h"
// #include"SPC.h"



using namespace std;

void ConstraintManager::takeDB(Input & input, Mesh & mesh)
{
    //首先检查是否存在boundary_conditions
    d_boundary_conditions = input.getVectorString("boundary_conditions");    
    // 若存在,则一一读取    
    for (size_t i = 0; i < d_boundary_conditions.size(); i++)
    {
        vector<ConstraintEquation> equations;
        auto constraint_name = d_boundary_conditions[i];
        auto constraint_type = input.getString(constraint_name + "_type");

        if (constraint_type == "SPC")
        {
            auto constrain = new SPC(constraint_name);
            constrain->takeDB(input, mesh);
            // constrain->buildDofMap();
            equations = constrain->buildEquations(mesh);
            d_equations_num = d_equations_num + equations.size();
            delete constrain;
            // FinalConstraintEquations.push_back(equations);
        }
        else
        {
            cout << "not support this type : " << constraint_type << endl;
            exit(1);
        }
        FinalConstraintEquations.push_back(equations);        
    }
}


Eigen::SparseMatrix<double> ConstraintManager::buildConstrintMatrix(Mesh & mesh)
{
    //TODO:: 临时写死--第一个分支:实体单元
    int numdofs = mesh.actual_node_count * NDIM;
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

Eigen::VectorXd ConstraintManager::buildConstrintForce(Mesh & mesh)
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