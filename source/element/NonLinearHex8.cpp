#include "NonLinearHex8.h"

using namespace std;
void NonLinearHex8::ComputeStiffness(double nodes_coordinates[20][3],
                                     std::vector<double> &displacement,
                                     std::vector<double> &du,
                                     std::vector<double> &ddu,
                                     ObjectElement &elementdata,
                                     std::vector<double> &elementmat)
{
    std::cout.precision(10);
    elementmat.resize(d_num_edofs * d_num_edofs);
    // 如果单元没有被初始化
    if (!elementdata.if_element_is_initialized)
    {
        // 对齐输入
        vector<vector<double>> a;
        vector<vector<vector<double>>> b;
        vector<double> c;
        initialize_element(nodes_coordinates, d_GaussPoints, a, b, c, d_num_intergration_point, elementdata);
    }
    // 根据位移更新变形梯度，和变形梯度的逆
    updateF_Finv(displacement, du, ddu, elementdata);




    // 为了openmp并行，在单元内部维护材料参数
    double C_e_tensor[3][3][3][3] = {0.0}; //弹性模量
    pmaterial->getC_e_tensor(C_e_tensor);
    double Ct[3][3][3][3] = {0.0};         //切线模量


    // TODO: 
    // updateStressAndC();
    // 循环所有积分点
    int num_GP = elementdata.num_Gauss_points;
    for (int i = 0; i < num_GP; i++) // 积分点循环
    {
        double D[6][6] = {0.0};      // 6X6的材料矩阵        
        double BT[3][6] = {0.0};     // BT
        double B[6][3] = {0.0};      // B
        double Bg[3][1] = {0.0};     // 几何矩阵
        double BgT[1][3] = {0.0};    // 
        double weight = elementdata.weights[i];
        double stress[3][3] = {0.0};
        double stressn[3][3] = {0.0};
        double stressn1[3][3] = {0.0};
        double JKB = elementdata.JKB[i]; // 母单元映射雅可比
        double jkb = elementdata.jkb[i]; // 构型变化之雅可比
        // pmaterial->getDt(elementdata.F_on_Gauss_points[i], elementdata.jkb[i], D);

        double F[3][3] = {0.0};
        double Finv[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                F[ii][jj] = elementdata.F_on_Gauss_points[i][ii][jj];
                Finv[ii][jj] = elementdata.Finv_on_Gauss_points[i][ii][jj];                
            }
        }
        pmaterial->getCt(C_e_tensor, F, jkb, Ct);
        pmaterial->transeCtoD(Ct, D);
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                stressn[ii][jj] = elementdata.stress_tensor_on_Gauss_points[i][ii][jj];
            }
        }
        pmaterial->getStress(C_e_tensor, F, jkb, stress);
        for (int j = 0; j < d_num_nodes; j++) // 节点循环
        {

            double sfdx = elementdata.sfdxyz_on_Gauss_points[i][j][0];
            double sfdy = elementdata.sfdxyz_on_Gauss_points[i][j][1];
            double sfdz = elementdata.sfdxyz_on_Gauss_points[i][j][2];
            double sf_dxnow = elementdata.Finv_on_Gauss_points[i][0][0] * sfdx +
                              elementdata.Finv_on_Gauss_points[i][1][0] * sfdy +
                              elementdata.Finv_on_Gauss_points[i][2][0] * sfdz;

            double sf_dynow = elementdata.Finv_on_Gauss_points[i][0][1] * sfdx +
                              elementdata.Finv_on_Gauss_points[i][1][1] * sfdy +
                              elementdata.Finv_on_Gauss_points[i][2][1] * sfdz;

            double sf_dznow = elementdata.Finv_on_Gauss_points[i][0][2] * sfdx +
                              elementdata.Finv_on_Gauss_points[i][1][2] * sfdy +
                              elementdata.Finv_on_Gauss_points[i][2][2] * sfdz;
            BT[0][0] = sf_dxnow;
            BT[0][3] = sf_dynow;
            BT[0][5] = sf_dznow;

            BT[1][1] = sf_dynow;
            BT[1][3] = sf_dxnow;
            BT[1][4] = sf_dznow;

            BT[2][2] = sf_dznow;
            BT[2][4] = sf_dynow;
            BT[2][5] = sf_dxnow;

            BgT[0][0] = sf_dxnow;
            BgT[0][1] = sf_dynow;
            BgT[0][2] = sf_dznow;

            for (int k = 0; k < d_num_nodes; k++)
            {
                sfdx = elementdata.sfdxyz_on_Gauss_points[i][k][0];
                sfdy = elementdata.sfdxyz_on_Gauss_points[i][k][1];
                sfdz = elementdata.sfdxyz_on_Gauss_points[i][k][2];
                sf_dxnow = elementdata.Finv_on_Gauss_points[i][0][0] * sfdx +
                           elementdata.Finv_on_Gauss_points[i][1][0] * sfdy +
                           elementdata.Finv_on_Gauss_points[i][2][0] * sfdz;

                sf_dynow = elementdata.Finv_on_Gauss_points[i][0][1] * sfdx +
                           elementdata.Finv_on_Gauss_points[i][1][1] * sfdy +
                           elementdata.Finv_on_Gauss_points[i][2][1] * sfdz;

                sf_dznow = elementdata.Finv_on_Gauss_points[i][0][2] * sfdx +
                           elementdata.Finv_on_Gauss_points[i][1][2] * sfdy +
                           elementdata.Finv_on_Gauss_points[i][2][2] * sfdz;

                B[0][0] = sf_dxnow;
                B[1][1] = sf_dynow;
                B[2][2] = sf_dznow;

                B[3][0] = sf_dynow;
                B[3][1] = sf_dxnow;
                B[4][1] = sf_dznow;

                B[4][2] = sf_dynow;
                B[5][0] = sf_dznow;
                B[5][2] = sf_dxnow;

                Bg[0][0] = sf_dxnow;
                Bg[1][0] = sf_dynow;
                Bg[2][0] = sf_dznow;

                double BTD[3][6] = {0.0};
                AmnXBpq(&BT[0][0], 3, 6, &D[0][0], 6, 6, &BTD[0][0]);
               
                double BTDB[3][3] = {0.0};
                AmnXBpq(&BTD[0][0], 3, 6, &B[0][0], 6, 3, &BTDB[0][0]);
                
                double BgTS[1][3] = {0.0};
                AmnXBpq(&BgT[0][0], 1, 3, &stress[0][0], 3, 3, &BgTS[0][0]);
                double BgTSBg[1][1] = {0.0};
                AmnXBpq(&BgTS[0][0], 1, 3, &Bg[0][0], 3, 1, &BgTSBg[0][0]);
                double TC1 = BgTSBg[0][0];
                

                double EK_IJ[3][3] = {};
                for (int iii = 0; iii < 3; iii++)
                {
                    for(int jjj = 0; jjj < 3; jjj++)
                    {
                        double delta = 0.0;
                        if (iii == jjj) delta = 1.0;
                        EK_IJ[iii][jjj] = (BTDB[iii][jjj] + TC1 * delta) * weight * JKB * jkb;
                    }
                }
                // for (int iii = 0; iii < 3; iii++)
                // {
                //     for(int jjj = 0; jjj < 3; jjj++)
                //     {
                //         std::cout << EK_IJ[iii][jjj] << std::endl;
                //     }
                // }
                // std::cout << "@@@@@@@@@@@@@@" << std::endl;
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

void NonLinearHex8::getShapeFunction(double nodes_coordinate[20][3],
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

void NonLinearHex8::SetElement()
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

void NonLinearHex8::initialize_element(double nodes_coordinate[20][3],
                                       std::vector<std::vector<double>> &GaussPoints,
                                       vector<vector<double>> &ShapeFunction,
                                       vector<vector<vector<double>>> &ShapeFunction_dxy,
                                       vector<double> &value_jkb,
                                       const int num_GP,
                                       ObjectElement &elementdata)
{
    getShapeFunction(nodes_coordinate, GaussPoints, ShapeFunction, ShapeFunction_dxy, value_jkb, num_GP);
    // 初始化积分点数目
    elementdata.num_Gauss_points = GaussPoints.size();
    // 积分点上的形函数
    elementdata.sf_on_Gauss_points = ShapeFunction;
    // 积分点上的形函数导数
    elementdata.sfdxyz_on_Gauss_points = ShapeFunction_dxy;
    // 积分点上的雅可比：母单元映射物理单元
    elementdata.JKB = value_jkb;
    // 积分点上的雅可比: 构型变化之间的映射
    elementdata.jkb.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
        elementdata.jkb[i] = 1.0;
    // 积分点权重
    elementdata.weights.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
    {
        elementdata.weights[i] = GaussPoints[i][3];
    }

    // 等效塑性应变
    elementdata.effecitve_plastic_strain_on_Gauss_points.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
        elementdata.effecitve_plastic_strain_on_Gauss_points[i] = 0.0;
    // 变形梯度 应力 变形梯度逆
    elementdata.F_on_Gauss_points.resize(num_GP);
    elementdata.Finv_on_Gauss_points.resize(num_GP);
    elementdata.stress_tensor_on_Gauss_points.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
    {
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
            {
                elementdata.stress_tensor_on_Gauss_points[i][j][k] = 0.0;
                elementdata.F_on_Gauss_points[i][j][k] = 0.0;
                elementdata.Finv_on_Gauss_points[i][j][k] = 0.0;

                if (j == k)
                {
                    elementdata.F_on_Gauss_points[i][j][k] = 1.0;
                    elementdata.Finv_on_Gauss_points[i][j][k] = 1.0;
                }
            }
    }
    elementdata.if_element_is_initialized = true;
}

void NonLinearHex8::updateF_Finv(std::vector<double> &displacement,
                                 std::vector<double> &du,
                                 std::vector<double> &ddu,
                                 ObjectElement &elementdata)
{
    double Fn1[3][3] = {0.0};
    double Finv_n1[3][3] = {0.0};
    for (int i = 0; i < 3; i++)
    {
        Fn1[i][i] = 1.0;
    }  

    double ux = 0.0;
    double uy = 0.0;
    double uz = 0.0;  
    double sfdx = 0.0;
    double sfdy = 0.0;
    double sfdz = 0.0;

    for (int i = 0; i < elementdata.num_Gauss_points; i++)
    {
        auto & shapefunction = elementdata.sfdxyz_on_Gauss_points[i];
        auto & Fn = elementdata.F_on_Gauss_points[i];
        auto & Finv_n = elementdata.Finv_on_Gauss_points[i];
        auto & jkbn = elementdata.jkb[i];
        for (int inode = 0; inode < d_num_nodes; inode++)
        {
            ux = displacement[inode * d_num_nodes * 3 + 0];
            uy = displacement[inode * d_num_nodes * 3 + 1];
            uz = displacement[inode * d_num_nodes * 3 + 2];
            sfdx = shapefunction[inode][0];
            sfdy = shapefunction[inode][1];
            sfdz = shapefunction[inode][2];

            Fn1[0][0] = Fn1[0][0] + ux * sfdx;
            Fn1[0][1] = Fn1[0][1] + ux * sfdy;
            Fn1[0][2] = Fn1[0][2] + ux * sfdz;

            Fn1[1][0] = Fn1[1][0] + uy * sfdx;
            Fn1[1][1] = Fn1[1][1] + uy * sfdy;
            Fn1[1][2] = Fn1[1][2] + uy * sfdz;

            Fn1[2][0] = Fn1[2][0] + uz * sfdx;
            Fn1[2][1] = Fn1[2][1] + uz * sfdy;
            Fn1[2][2] = Fn1[2][2] + uz * sfdz;
        }
        double jkb_n1 = invertMatrix(Fn1, Finv_n1);
        jkbn = jkb_n1; // 更新雅可比
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                Fn[ii][jj] = Fn1[ii][jj];
                Finv_n[ii][jj] = Finv_n1[ii][jj];
            }
        }

    }
}