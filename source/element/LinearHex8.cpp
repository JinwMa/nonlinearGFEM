#include "LinearHex8.h"
#include <vector>
#include <iostream>
#include <Eigen/Dense>

using namespace std;

void LinearHex8::ComputeStiffness(vector<vector<double>> & nodes_coordinate,
                                  std::vector<std::vector<double>> & GaussPoints,
                                  vector<double> & elementmat)
{
    // 先给elementmat清零
    elementmat.resize(num_edofs * num_edofs);
    // 材料参数先给一个默认值
    double E = 1.E7;
    double v = 0.3;
    // 形函数和形函数导数
    vector<vector<double>> SF;
    vector<vector<vector<double>>> SF_dxyz;
    vector<double> detJ;
    this->getShapeFunction(nodes_coordinate, GaussPoints, SF, SF_dxyz, detJ, num_intergration_point);

    double D[6][6] = {};
    double F = (E * (1.0 - v)) / ((1.0 - 2.0 * v) * (1 + v));
    D[0][0] = F;
    D[1][1] = F;
    D[2][2] = F;
    D[3][3] = F * (1.0 - 2.0 * v) / (2.0 * (1.0 - v));
    D[4][4] = D[3][3];
    D[5][5] = D[3][3];
    D[0][1] = F * v / (1.0 - v);
    D[0][2] = D[0][1];
    D[1][0] = D[0][1];
    D[1][2] = D[0][1];
    D[2][0] = D[0][1];
    D[2][1] = D[0][1];
    // 循环积分点
    double B[6][3] = {};
    double BT[3][6] = {};
    double BTDB[3][3] = {};
    for (int i = 0; i < num_intergration_point; i++)
    {
        double w = GaussPoints[i][3];
        double J = detJ[i];
        for (int ii = 0; ii < 8; ii++)
        {
            double sf_dxnow = SF_dxyz[i][ii][0];
            double sf_dynow = SF_dxyz[i][ii][1];
            double sf_dznow = SF_dxyz[i][ii][2];

            BT[0][0] = sf_dxnow;
            BT[0][3] = sf_dynow;
            BT[0][5] = sf_dznow;
            BT[1][1] = sf_dynow;
            BT[1][3] = sf_dxnow;
            BT[1][4] = sf_dznow;
            BT[2][2] = sf_dznow;
            BT[2][4] = sf_dynow;
            BT[2][5] = sf_dxnow;

            for (int jj = 0; jj < 8; jj++)
            {                
                double sf_dxnow = SF_dxyz[i][jj][0];
                double sf_dynow = SF_dxyz[i][jj][1];
                double sf_dznow = SF_dxyz[i][jj][2];

                B[0][0] = sf_dxnow;
                B[1][1] = sf_dynow;
                B[2][2] = sf_dznow;
                B[3][0] = sf_dynow;
                B[3][1] = sf_dxnow;
                B[4][1] = sf_dznow;
                B[4][2] = sf_dynow;
                B[5][0] = sf_dznow;
                B[5][2] = sf_dxnow;
                double BTD[3][6] = {};
                double BTDB[3][3] = {};
                AmnXBpq(&BT[0][0], 3, 6, &D[0][0], 6, 6, &BTD[0][0]);
                AmnXBpq(&BTD[0][0], 3, 6, &B[0][0], 6, 3, &BTDB[0][0]);
                double EK_IJ[3][3] = {};
                for (int iii = 0; iii < 3; iii++)
                {
                    for(int jjj = 0; jjj < 3; jjj++)
                    {
                        EK_IJ[iii][jjj] = BTDB[iii][jjj] * w * J;
                    }
                }
                for (int iii = 0; iii < 3; iii++)
                    for (int jjj = 0; jjj < 3; jjj++)
                    {
                        int col = ii * 3 + iii;
                        int row = jj * 3 + jjj;
                        elementmat[col * num_edofs + row] += EK_IJ[iii][jjj];
                    }
            }
        }
    }
}

void LinearHex8::getShapeFunction(vector<vector<double>> & nodes_coordinate,
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

void LinearHex8::SetGaussIntegration( std::vector<std::vector<double>> &GaussPoints)
{
    const int intergrationorder = integration_order;
    if (intergrationorder < 1)
    {
        std::cout << "积分阶次输入错误" << std::endl;
        exit(0);
    }
    const int number_Gauss_point = intergrationorder * intergrationorder * intergrationorder;
    GaussPoints.resize(number_Gauss_point);
    for (int i = 0; i < number_Gauss_point; i++)
    {
        GaussPoints[i].resize(4);
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
                GaussPoints[index][0] = xs[i][0];
                GaussPoints[index][1] = xs[j][0];
                GaussPoints[index][2] = xs[k][0];
                GaussPoints[index][3] = xs[i][1] * xs[j][1] * xs[k][1];
                index++;
            }
        }
    }
}