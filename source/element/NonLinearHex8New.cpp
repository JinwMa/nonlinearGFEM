#include "NonLinearHex8New.h"

using namespace std;


void NonLinearHex8New::initializeElement(ObjectElementData & element_data)
{
    if (element_data.is_initialized) return;

    double nodes_coordinate[20][3] = {0.0};
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            nodes_coordinate[i][j] = element_data.coordinates.at(i).at(j);
        }
    }
    // 对齐输入
    vector<vector<double>> a;
    vector<vector<vector<double>>> b;
    vector<double> c;
    initialize_element(nodes_coordinate, d_GaussPoints, a, b, c, d_num_intergration_point, element_data);
}

void NonLinearHex8New::ComputeStiffness(ObjectElementData &element_data,
                                     std::vector<double> &elementmat,
                                     ObjectContralParam * contral_param)
{
    if(contral_param == nullptr) toolbox::error("contral_param is null");
    std::cout.precision(20);
    elementmat.resize(d_num_edofs * d_num_edofs);

    
    std::vector<std::vector<double>> dS_du;
    // pmaterial->getDSDu(element_data, dS_du);
    pmaterial->updateStressAndDSDu(element_data, dS_du);
    if (!element_data.is_updated_interation) 
    {
        // 根据位移更新变形梯度，和变形梯度的逆
        updateF_Finv(element_data);
        // pmaterial->updateStress(element_data);
        element_data.is_updated_interation = true;
    }
    
    // updateInternalVariable(element_data);   
    // 循环所有积分点
    int num_GP = element_data.num_integration_points;    

    double dNJ_dx[3];
    double dNK_dx[3];
    double delt[3][3] = {0.0};   
    for (int i = 0; i < 3; i++)
    {
        delt[i][i] = 1.0;
    }

    for (int i = 0; i < num_GP; i++) // 积分点循环
    {
        double weight = element_data.weights[i];
        double JKB = element_data.JKB[i]; // 母单元映射雅可比
        double jkb = element_data.jkb_n1[i]; // 构型变化之雅可比

        const auto & stress = element_data.stress_n1[i];

        for (int j = 0; j < d_num_nodes; j++) // 节点循环
        {

            double sfdx = element_data.sfdxyz[i][j][0];
            double sfdy = element_data.sfdxyz[i][j][1];
            double sfdz = element_data.sfdxyz[i][j][2];
            double sf_dxnow = element_data.Finv_n1[i][0][0] * sfdx +
                              element_data.Finv_n1[i][1][0] * sfdy +
                              element_data.Finv_n1[i][2][0] * sfdz;

            double sf_dynow = element_data.Finv_n1[i][0][1] * sfdx +
                              element_data.Finv_n1[i][1][1] * sfdy +
                              element_data.Finv_n1[i][2][1] * sfdz;

            double sf_dznow = element_data.Finv_n1[i][0][2] * sfdx +
                              element_data.Finv_n1[i][1][2] * sfdy +
                              element_data.Finv_n1[i][2][2] * sfdz;

            dNJ_dx[0] = sf_dxnow;
            dNJ_dx[1] = sf_dynow;
            dNJ_dx[2] = sf_dznow;

            for (int k = 0; k < d_num_nodes; k++)
            {
                sfdx = element_data.sfdxyz[i][k][0];
                sfdy = element_data.sfdxyz[i][k][1];
                sfdz = element_data.sfdxyz[i][k][2];
                sf_dxnow = element_data.Finv_n1[i][0][0] * sfdx +
                           element_data.Finv_n1[i][1][0] * sfdy +
                           element_data.Finv_n1[i][2][0] * sfdz;

                sf_dynow = element_data.Finv_n1[i][0][1] * sfdx +
                           element_data.Finv_n1[i][1][1] * sfdy +
                           element_data.Finv_n1[i][2][1] * sfdz;

                sf_dznow = element_data.Finv_n1[i][0][2] * sfdx +
                           element_data.Finv_n1[i][1][2] * sfdy +
                           element_data.Finv_n1[i][2][2] * sfdz;

                dNK_dx[0] = sf_dxnow;
                dNK_dx[1] = sf_dynow;
                dNK_dx[2] = sf_dznow;


                // EK = EK_1 - EK_2 + EK_3
                double EK_1[3][3] = {0.0};
                double EK_2[3][3] = {0.0};
                double EK_3[3][3] = {0.0};
                double EK_4[3][3] = {0.0};

                const auto & dS_duK = dS_du[i * d_num_nodes + k];
                for (int ii = 0; ii < 3; ii++)
                {
                    for (int jj = 0; jj <3; jj++)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            EK_4[ii][p] += dNJ_dx[jj] * dS_duK[ii * 9 + jj * 3 + p];
                        }
                    }
                }
                for (int ii = 0; ii < 3; ii++)
                {
                    for (int jj = 0; jj < 3; jj++)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            EK_1[ii][p] += stress[ii][jj] * dNJ_dx[jj] * dNK_dx[p];
                        }
                    }
                }
                for (int ii = 0; ii < 3; ii++)
                {
                    for (int jj = 0; jj < 3; jj++)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            EK_2[ii][p] += stress[ii][jj] * dNK_dx[jj] * dNJ_dx[p];
                        }
                    }
                }
                double EK_IJ[3][3] = {};
                for (int iii = 0; iii < 3; iii++)
                {
                    for(int jjj = 0; jjj < 3; jjj++)
                    {
                        double delta = 0.0;
                        if (iii == jjj) delta = 1.0;
                        EK_IJ[iii][jjj] = (EK_1[iii][jjj] - EK_2[iii][jjj] + EK_4[iii][jjj]) * weight * JKB * jkb;
                    }
                }
                for (int iii = 0; iii < 3; iii++)
                {
                    for (int jjj = 0; jjj < 3; jjj++)
                    {
                        int col = j * 3 + iii;
                        int row = k * 3 + jjj;
                        elementmat[col * d_num_edofs + row] += EK_IJ[iii][jjj];
                    }
                }
            }

        }
    }
}

void NonLinearHex8New::getShapeFunction(double nodes_coordinate[20][3],
                                     std::vector<std::vector<double>> &GaussPoints,
                                     vector<vector<double>> &ShapeFunction,
                                     vector<vector<vector<double>>> &ShapeFunction_dxy,
                                     vector<double> &value_jkb,
                                     const int num_GP)
{
    if (!num_GP == GaussPoints.size())
    {
        std::cout << "数组长度不匹配" << std::endl;
        exit(0);
    }
    // 为形函数开辟空间
    ShapeFunction.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
        ShapeFunction[i].resize(8);
    // 为形函数导数开辟空间
    ShapeFunction_dxy.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
        ShapeFunction_dxy[i].resize(8);
    for (int i = 0; i < num_GP; i++)
        for (int j = 0; j < 8; j++)
            ShapeFunction_dxy[i][j].resize(3);
    // 为积分点上的雅可比行列式值开辟空间
    value_jkb.resize(num_GP);

    // 计算形函数
    for (int i = 0; i < num_GP; i++)
    {
        ShapeFunction[i][0] = ((1.0 - GaussPoints[i][0]) * (1.0 - GaussPoints[i][1]) * (1.0 - GaussPoints[i][2])) / 8.0;
        ShapeFunction[i][1] = ((1.0 + GaussPoints[i][0]) * (1.0 - GaussPoints[i][1]) * (1.0 - GaussPoints[i][2])) / 8.0;
        ShapeFunction[i][2] = ((1.0 + GaussPoints[i][0]) * (1.0 + GaussPoints[i][1]) * (1.0 - GaussPoints[i][2])) / 8.0;
        ShapeFunction[i][3] = ((1.0 - GaussPoints[i][0]) * (1.0 + GaussPoints[i][1]) * (1.0 - GaussPoints[i][2])) / 8.0;
        ShapeFunction[i][4] = ((1.0 - GaussPoints[i][0]) * (1.0 - GaussPoints[i][1]) * (1.0 + GaussPoints[i][2])) / 8.0;
        ShapeFunction[i][5] = ((1.0 + GaussPoints[i][0]) * (1.0 - GaussPoints[i][1]) * (1.0 + GaussPoints[i][2])) / 8.0;
        ShapeFunction[i][6] = ((1.0 + GaussPoints[i][0]) * (1.0 + GaussPoints[i][1]) * (1.0 + GaussPoints[i][2])) / 8.0;
        ShapeFunction[i][7] = ((1.0 - GaussPoints[i][0]) * (1.0 + GaussPoints[i][1]) * (1.0 + GaussPoints[i][2])) / 8.0;
    }
    // 计算形函数导数和雅可比行列式值
    for (int i = 0; i < num_GP; i++)
    {
        double jl[3][8] = {};
        double x1[3] = {0.0};
        x1[0] = GaussPoints[i][0];
        x1[1] = GaussPoints[i][1];
        x1[2] = GaussPoints[i][2];

        jl[0][0] = -(1.0 - x1[1]) * (1.0 - x1[2]) / 8.0;
        jl[0][1] = -jl[0][0];
        jl[0][2] = (1.0 + x1[1]) * (1.0 - x1[2]) / 8.0;
        jl[0][3] = -jl[0][2];
        jl[0][4] = -(1.0 - x1[1]) * (1.0 + x1[2]) / 8.0;
        jl[0][5] = -jl[0][4];
        jl[0][6] = (1.0 + x1[1]) * (1.0 + x1[2]) / 8.0;
        jl[0][7] = -jl[0][6];

        jl[1][0] = -(1.0 - x1[0]) * (1.0 - x1[2]) / 8.0;
        jl[1][1] = -(1.0 + x1[0]) * (1.0 - x1[2]) / 8.0;
        jl[1][2] = (1.0 + x1[0]) * (1.0 - x1[2]) / 8.0;
        jl[1][3] = (1.0 - x1[0]) * (1.0 - x1[2]) / 8.0;
        jl[1][4] = -(1.0 - x1[0]) * (1.0 + x1[2]) / 8.0;
        jl[1][5] = -(1.0 + x1[0]) * (1.0 + x1[2]) / 8.0;
        jl[1][6] = (1.0 + x1[0]) * (1.0 + x1[2]) / 8.0;
        jl[1][7] = (1.0 - x1[0]) * (1.0 + x1[2]) / 8.0;

        // 第三行
        jl[2][0] = -(1.0 - x1[0]) * (1.0 - x1[1]) / 8.0;
        jl[2][1] = -(1.0 + x1[0]) * (1.0 - x1[1]) / 8.0;
        jl[2][2] = -(1.0 + x1[0]) * (1.0 + x1[1]) / 8.0;
        jl[2][3] = -(1.0 - x1[0]) * (1.0 + x1[1]) / 8.0;
        jl[2][4] = -jl[2][0];
        jl[2][5] = -jl[2][1];
        jl[2][6] = -jl[2][2];
        jl[2][7] = -jl[2][3];
        double jkb[3][3] = {};
        AmnXBpq(&jl[0][0], 3, 8, &nodes_coordinate[0][0], 8, 3, &jkb[0][0]);
        double jkb_inv[3][3] = {};
        value_jkb[i] = invertMatrix(jkb, jkb_inv);
        double DSF[3][8] = {};
        // AmnXBpq(&jkb_inv[0][0], 3, 3, &jl[0][0], 3, 8, &DSF[0][0]);
        AXB3338(jkb_inv, jl, DSF);

        for (int ii = 0; ii < 8; ii++)
        {
            ShapeFunction_dxy[i][ii][0] = DSF[0][ii];
            ShapeFunction_dxy[i][ii][1] = DSF[1][ii];
            ShapeFunction_dxy[i][ii][2] = DSF[2][ii];
        }
    }
}

void NonLinearHex8New::SetElement()
{
    // 设置维度
    dim = 3;
    // 设置节点自由度数
    numNodeDof = 3;
    // 设置节点数目
    numNodes = 8;
    // 设置自由度标签
    dofs.resize(dim);
    dofs[0] = "ux";
    dofs[1] = "uy";
    dofs[2] = "uz";
    // 设置高斯积分点
    const int intergrationorder = integration_order;
    if (intergrationorder < 1)
    {
        std::cout << "积分阶次输入错误" << std::endl;
        exit(0);
    }
    const int number_Gauss_point = intergrationorder * intergrationorder * intergrationorder;
    d_GaussPoints.resize(number_Gauss_point);
    for (int i = 0; i < number_Gauss_point; i++)
    {
        d_GaussPoints[i].resize(4);
    }

    double xs[intergrationorder][2];

    if (intergrationorder == 1)
    {
        xs[0][0] = 0.0;
        xs[0][1] = 2.0;
    }
    else if (intergrationorder == 2)
    {
        xs[0][0] = -0.577350269189626;
        xs[0][1] = 1.0;

        xs[1][0] = 0.577350269189626;
        xs[1][1] = 1.0;
    }
    else if (intergrationorder == 3)
    {
        xs[0][0] = -0.774596669241483;
        xs[0][1] = 0.555555555555556;
        xs[1][0] = 0.0;
        xs[1][1] = 0.888888888888889;
        xs[2][0] = 0.774596669241483;
        xs[2][1] = 0.555555555555556;
    }
    else if (intergrationorder == 4)
    {
        xs[0][0] = -0.861136311594053;
        xs[0][1] = 0.347854845137454;
        xs[1][0] = -0.339981043584856;
        xs[1][1] = 0.652145154862546;
        xs[2][0] = 0.339981043584856;
        xs[2][1] = 0.652145154862546;
        xs[3][0] = 0.861136311594053;
        xs[3][1] = 0.347854845137454;
    }
    else if (intergrationorder == 6)
    {
        xs[0][0] = -0.932469514203152;
        xs[1][0] = -0.661209386466265;
        xs[2][0] = -0.238619186083197;
        xs[3][0] = 0.238619186083197;
        xs[4][0] = 0.661209386466265;
        xs[5][0] = 0.932469514203152;

        xs[0][1] = 0.171324492379170;
        xs[1][1] = 0.360761573048139;
        xs[2][1] = 0.467913934572691;
        xs[3][1] = 0.467913934572691;
        xs[4][1] = 0.360761573048139;
        xs[5][1] = 0.171324492379170;
    }
    else
    {
        std::cout << "不支持的积分阶次" << std::endl;
        exit(0);
    }
    int index = 0;
    for (int i = 0; i < intergrationorder; i++)
    {
        for (int j = 0; j < intergrationorder; j++)
        {
            for (int k = 0; k < intergrationorder; k++)
            {
                d_GaussPoints[index][0] = xs[i][0];
                d_GaussPoints[index][1] = xs[j][0];
                d_GaussPoints[index][2] = xs[k][0];
                d_GaussPoints[index][3] = xs[i][1] * xs[j][1] * xs[k][1];
                index++;
            }
        }
    }
}

void NonLinearHex8New::initialize_element(double nodes_coordinate[20][3],
                                       std::vector<std::vector<double>> &GaussPoints,
                                       vector<vector<double>> &ShapeFunction,
                                       vector<vector<vector<double>>> &ShapeFunction_dxy,
                                       vector<double> &value_jkb,
                                       const int num_GP,
                                       ObjectElementData &elementdata)
{
    // 初始化element patch
    elementdata.element_patch = elementdata.node_ids;
    getShapeFunction(nodes_coordinate, GaussPoints, ShapeFunction, ShapeFunction_dxy, value_jkb, num_GP);
    // 初始化积分点数目
    elementdata.num_integration_points = GaussPoints.size();
    // 积分点上的形函数
    elementdata.sf = ShapeFunction;
    // 积分点上的形函数导数
    elementdata.sfdxyz = ShapeFunction_dxy;
    // 积分点上的雅可比：母单元映射物理单元
    elementdata.JKB = value_jkb;
    // 积分点权重
    elementdata.weights.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
    {
        elementdata.weights[i] = GaussPoints[i][3];
    }
    // 积分点上的雅可比: 构型变化之间的映射
    elementdata.jkb_n.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
        elementdata.jkb_n[i] = 1.0;
    
    // 等效塑性应变
    elementdata.eff_p_strain_n.resize(num_GP);
    elementdata.kappa_n.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
    {
        elementdata.eff_p_strain_n[i] = 0.0;
        elementdata.kappa_n[i] = pmaterial->d_init_kappa;
    }

    // 变形梯度 应力 变形梯度逆
    elementdata.F_n.resize(num_GP);
    elementdata.Finv_n.resize(num_GP);
    elementdata.stress_n.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
    {
        elementdata.F_n[i].resize(d_num_node_dof);
        elementdata.Finv_n[i].resize(d_num_node_dof);
        elementdata.stress_n[i].resize(d_num_node_dof);
        for (int j = 0; j < d_num_node_dof; j++)
        {
            elementdata.F_n[i][j].resize(d_num_node_dof);
            elementdata.Finv_n[i][j].resize(d_num_node_dof);
            elementdata.stress_n[i][j].resize(d_num_node_dof);
        }
    }
    for (int i = 0; i < num_GP; i++)
    {
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
            {
                elementdata.stress_n[i][j][k] = 0.0;
                elementdata.F_n[i][j][k] = 0.0;
                elementdata.Finv_n[i][j][k] = 0.0;

                if (j == k)
                {
                    elementdata.F_n[i][j][k] = 1.0;
                    elementdata.Finv_n[i][j][k] = 1.0;
                }
            }
    }

    elementdata.jkb_n1 = elementdata.jkb_n;
    elementdata.eff_p_strain_n1 = elementdata.eff_p_strain_n;
    elementdata.kappa_n1 = elementdata.kappa_n;

    elementdata.F_n1 = elementdata.F_n;
    elementdata.Finv_n1 = elementdata.Finv_n;
    elementdata.stress_n1 = elementdata.stress_n;

    //初始化位移场可能已经在单元外部执行了初始化
    if (elementdata.u.size() == 0) elementdata.u.resize(d_num_nodes);
    if (elementdata.du.size() == 0) elementdata.du.resize(d_num_nodes);
    for (int i = 0; i < d_num_nodes; i++)
    {
        if (elementdata.u[i].size() == 0)
            elementdata.u[i].resize(d_num_node_dof);
        if (elementdata.du[i].size() == 0)
            elementdata.du[i].resize(d_num_node_dof);
    }

    elementdata.is_initialized = true;    
}



void NonLinearHex8New::updateF_Finv(ObjectElementData &elementdata)
{
    double ux = 0.0;
    double uy = 0.0;
    double uz = 0.0;  
    double sfdx = 0.0;
    double sfdy = 0.0;
    double sfdz = 0.0;

    for (int i = 0; i < elementdata.num_integration_points; i++)
    {
        //更新变形梯度
        auto & F_n1 = elementdata.F_n1[i];
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                F_n1[ii][jj] = 0.0;
                if (ii == jj) F_n1[ii][jj] = 1.0;
            }
        }
        auto & shapefunction = elementdata.sfdxyz[i];
        for (int inode = 0; inode < d_num_nodes; inode++)
        {

            ux = elementdata.u[inode][0] + elementdata.du[inode][0];
            uy = elementdata.u[inode][1] + elementdata.du[inode][1];
            uz = elementdata.u[inode][2] + elementdata.du[inode][2];
            sfdx = shapefunction[inode][0];
            sfdy = shapefunction[inode][1];
            sfdz = shapefunction[inode][2];

            F_n1[0][0] += ux * sfdx;
            F_n1[0][1] += ux * sfdy;
            F_n1[0][2] += ux * sfdz;

            F_n1[1][0] += uy * sfdx;
            F_n1[1][1] += uy * sfdy;
            F_n1[1][2] += uy * sfdz;

            F_n1[2][0] += uz * sfdx;
            F_n1[2][1] += uz * sfdy;
            F_n1[2][2] += uz * sfdz;
        }
        // 更新变形梯度的逆和雅可比
        auto & Finv_n1 = elementdata.Finv_n1[i];
        auto & jkb_n1 = elementdata.jkb_n1[i];
        jkb_n1 = invertMatrix(F_n1, Finv_n1);
    }
}




void NonLinearHex8New::ComputeInternalForce(ObjectElementData &element_data,
                                  std::vector<double> &elementvector,
                                  ObjectContralParam *contral_param)
{
    if (contral_param == nullptr)
        toolbox::error("contral_param is null");
    if (!element_data.is_initialized)
        toolbox::error("element is not initialized");

    updateInternalVariable(element_data);

    std::cout.precision(20);
    elementvector.resize(d_num_edofs);   

    int num_GP = element_data.num_integration_points;    
    
    for (int i = 0; i < num_GP; i++)
    {
        auto & stress = element_data.stress_n1[i];
        double BT[3][6] = {0.0};     // BT
        double F[3][3] = {0.0};
        double Finv[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                F[ii][jj] = element_data.F_n1[i][ii][jj];
                Finv[ii][jj] = element_data.Finv_n1[i][ii][jj];                
            }
        }

        for (int j = 0; j < d_num_nodes; j++) // 节点循环
        {
            // auto dNI_dX = element_data.sfdxyz[i][j];
            // double dNI_dx[3] = {0.0};
            // for (int ii = 0; ii < 3; ii++)
            // {
            //     for (int jj = 0; jj < 3; jj++)
            //     {
            //         dNI_dx[jj] += dNI_dX[ii] * Finv[ii][jj];
            //     }
            // }

            // for (int ii = 0; ii < 3; ii++)
            // {
            //     for (int jj = 0; jj < 3; jj++)
            //     {
            //         elementvector[3 * j + ii] += (stress[ii][jj] * dNI_dx[jj]) * element_data.weights[i] * element_data.jkb_n1[i] * element_data.JKB[i];
            //     }
            // }

            double sfdx = element_data.sfdxyz[i][j][0];
            double sfdy = element_data.sfdxyz[i][j][1];
            double sfdz = element_data.sfdxyz[i][j][2];

            double sf_dxnow = element_data.Finv_n1[i][0][0] * sfdx +
                              element_data.Finv_n1[i][1][0] * sfdy +
                              element_data.Finv_n1[i][2][0] * sfdz;

            double sf_dynow = element_data.Finv_n1[i][0][1] * sfdx +
                              element_data.Finv_n1[i][1][1] * sfdy +
                              element_data.Finv_n1[i][2][1] * sfdz;

            double sf_dznow = element_data.Finv_n1[i][0][2] * sfdx +
                              element_data.Finv_n1[i][1][2] * sfdy +
                              element_data.Finv_n1[i][2][2] * sfdz;
            BT[0][0] = sf_dxnow;
            BT[0][3] = sf_dynow;
            BT[0][5] = sf_dznow;

            BT[1][1] = sf_dynow;
            BT[1][3] = sf_dxnow;
            BT[1][4] = sf_dznow;

            BT[2][2] = sf_dznow;
            BT[2][4] = sf_dynow;
            BT[2][5] = sf_dxnow;

            elementvector[3 * j + 0] += (sf_dxnow * stress[0][0] + sf_dynow * stress[1][0] + sf_dznow * stress[2][0]) * element_data.weights[i] * element_data.jkb_n1[i] * element_data.JKB[i];
            elementvector[3 * j + 1] += (sf_dxnow * stress[0][1] + sf_dynow * stress[1][1] + sf_dznow * stress[2][1]) * element_data.weights[i] * element_data.jkb_n1[i] * element_data.JKB[i];
            elementvector[3 * j + 2] += (sf_dxnow * stress[0][2] + sf_dynow * stress[1][2] + sf_dznow * stress[2][2]) * element_data.weights[i] * element_data.jkb_n1[i] * element_data.JKB[i];         

        }        
    }

}

void NonLinearHex8New::updateInternalVariable(ObjectElementData & elementdata)
{
    if (elementdata.is_updated_interation) return;
    // 根据位移更新变形梯度，和变形梯度的逆
    updateF_Finv(elementdata);
    pmaterial->updateStress(elementdata);
    elementdata.is_updated_interation = true;
}