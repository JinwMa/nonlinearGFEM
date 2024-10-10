#include"SPC.h"
#include"toolbox.h"

void SPC::takeDB(Input * input, Mesh * mesh)
{
    // 读节点
    std::string nodes_type = input->getString(name + "_nodes_type");
    if (nodes_type == "geometry")
    {
        d_geometry_expression = input->getVectorDouble(name + "_geometry_expression");
        getNodesFromGeometry(mesh);
    }
    else
    {
        toolbox::error("not found " + name + "nodes_type");
    }


    // 读自由度
    d_dof = input->getString(name + "_dof");


    // 读值
    if (input->ifExist(name + "_value"))
    {
        d_is_value = true;
        d_value = input->getDouble(name + "_value");
    }
    else if (input->ifExist(name + "_value_expression"))
    {
        d_is_value_expression = true;
        d_value_expression = input->getVectorDouble(name + "_value_expression");
    }
    else
    {
        toolbox::error("the value of SPC" + name + " is wrong");
    }
    
}

void SPC::getNodesFromGeometry(Mesh * mesh)
{
    // d_node_ids
    const double eps = 1.E-10;
    int count = d_geometry_expression.size();
    if (count == 0) toolbox::error("the size of d_geometry_expression is wrong 0" );
    if (count > 10) toolbox::error("the size of d_geometry_expression is wrong 10");
    
    //重新定义几何表达式
    double expression[10] = {0.0};
    for (int i = 0; i < count; i++)expression[i] = d_geometry_expression[i];

    int node_num = mesh->actual_node_count;
    for (int i = 0; i < node_num; i++)
    {
        int node_id = mesh->NodeIdList[i];
        int node_order = mesh->NodeOrderInList[node_id] - 1;        
        double x = mesh->NodesCoordinate[node_order][0]; 
        double y = mesh->NodesCoordinate[node_order][1]; 
        double z = 0.0;
        if (NDIM == 3) z = mesh->NodesCoordinate[node_order][2];
        double value = 1E10;
        if (NDIM != 3) toolbox::error("2D or 1D cases are not supported");
        else
        {
            value = expression[0] + expression[1] * x + expression[2] * y + expression[3] * z +
                    expression[4] * x * x + expression[5] * y * y + expression[6] * z * z + 
                    expression[7] * x * y + expression[8] * y * z + expression[9] * x * z;
        }
        if (std::fabs(value) < eps) d_node_ids.push_back(node_id);
    }
    // std::cout << d_node_ids.size() << std::endl;
}

std::vector<ConstraintEquation> SPC::buildEquations(Mesh * mesh)
{
    std::vector<ConstraintEquation> equations;
    for (size_t i = 0; i < d_node_ids.size(); i++)
    {
        ConstraintEquation equation;
        int node_id = d_node_ids[i];
        equation.equationterms.resize(1);
        equation.equationterms[0].node_id = node_id;
        equation.equationterms[0].node_dof = d_dof;
        equation.equationterms[0].factor = 1.0;
        if (d_is_value)
        {            
            equation.rhs = d_value;
        }
        else if ( d_is_value_expression )
        {
            int count = d_value_expression.size();
            if (count == 0)
                toolbox::error("the size of d_value_expression is wrong 0");
            if (count > 10)
                toolbox::error("the size of d_value_expression is wrong 10");

            // 重新定义几何表达式
            double expression[10] = {0.0};
            for (int ii = 0; ii < count; ii++)
                expression[ii] = d_value_expression[ii];

            int node_order = mesh->NodeOrderInList[node_id] - 1;
            double x = mesh->NodesCoordinate[node_order][0];
            double y = mesh->NodesCoordinate[node_order][1];
            double z = 0.0;
            if (NDIM == 3)
                z = mesh->NodesCoordinate[node_order][2];
            double value = 1E10;
            if (NDIM != 3)
                toolbox::error("2D or 1D cases are not supported");

            value = expression[0] + expression[1] * x + expression[2] * y + expression[3] * z +
                    expression[4] * x * x + expression[5] * y * y + expression[6] * z * z + 
                    expression[7] * x * y + expression[8] * y * z + expression[9] * x * z;
            equation.rhs = value;
        }
        equations.push_back(equation);
    }

    // for (size_t i = 0; i < equations.size(); i++)
    // {
    //     std::cout << equations[i].equationterms[0].node_id << " " << equations[i].equationterms[0].factor << std::endl;
    // }

    return equations;

}