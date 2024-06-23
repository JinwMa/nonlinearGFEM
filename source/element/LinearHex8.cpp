#include "LinearHex8.h"
#include <vector>
#include <iostream>
#include <Eigen/Dense>

void LinearHex8::ComputeStiffness(double nodes_coordinate[8][3],
                                  std::vector<std::vector<double>> &GaussPoints,
                                  double elementmat[num_edofs][num_edofs])
{
    // 先给elementmat清零
    for (int i = 0; i < num_edofs; i++)
        for (int j = 0; j < num_edofs; j++)
            elementmat[i][j] = 0.0;
    // 材料参数先给一个默认值
    double E = 1.E7;
    double v = 0.3;
    // 形函数和形函数导数
    double *SF[8];
    double *SF_dxyz[8][3];
    double detJ[num_intergration_point];
    this->getShapeFunction(nodes_coordinate, GaussPoints, SF, SF_dxyz, detJ, num_intergration_point);

    Eigen::Matrix<double, 6, 6> D;
    D.setZero();
    double F = (E * (1.0 - v)) / ((1.0 - 2.0 * v) * (1 + v));
    D(0, 0) = F;
    D(1, 1) = F;
    D(2, 2) = F;
    D(3, 3) = F * (1.0 - 2.0 * v) / (2.0 * (1.0 - v));
    D(4, 4) = D(3, 3);
    D(5, 5) = D(3, 3);
    D(0, 1) = F * v / (1.0 - v);
    D(0, 2) = D(0, 1);
    D(1, 0) = D(0, 1);
    D(1, 2) = D(0, 1);
    D(2, 0) = D(0, 1);
    D(2, 1) = D(0, 1);
    // 循环积分点
    Eigen::Matrix<double, 6, 3> B;
    Eigen::Matrix<double, 3, 6> BT;
    for (int i = 0; i < num_intergration_point; i++)
    {
        double w = GaussPoints[i][3];
        double J = detJ[i];  
        for (int ii = 0; ii < 8; ii++)
        {
            BT.setZero();
            double sf_dxnow = SF_dxyz[i][ii][0];
            double sf_dynow = SF_dxyz[i][ii][1];
            double sf_dznow = SF_dxyz[i][ii][2];
            BT(0, 0) = sf_dxnow;
            BT(0, 3) = sf_dynow;
            BT(0, 5) = sf_dznow;
            BT(1, 1) = sf_dynow;
            BT(1, 3) = sf_dxnow;
            BT(1, 4) = sf_dznow;
            BT(2, 2) = sf_dznow;
            BT(2, 4) = sf_dynow;
            BT(2, 5) = sf_dxnow;

            for (int jj = 0; jj < 8; jj++)
            {
                B.setZero();
                double sf_dxnow = SF_dxyz[i][jj][0];
                double sf_dynow = SF_dxyz[i][jj][1];
                double sf_dznow = SF_dxyz[i][jj][2];
                B(0, 0) = sf_dxnow;
                B(1, 1) = sf_dynow;
                B(2, 2) = sf_dznow;
                B(3, 0) = sf_dynow;
                B(3, 1) = sf_dxnow;
                B(4, 1) = sf_dznow;
                B(4, 2) = sf_dynow;
                B(5, 0) = sf_dznow;
                B(5, 2) = sf_dxnow;
                Eigen::Matrix<double, 3, 3> BTDB = BT * D * B;
                Eigen::Matrix<double, 3, 3> EK_IJ = BTDB * w * J;

                for (int iii = 0; iii < 3; iii++)
                    for (int jjj = 0; jjj < 3; jjj++)
                    {
                        int col = ii * 3 + iii;
                        int row = jj * 3 + jjj;
                        elementmat[col][row] += EK_IJ(iii, jjj);
                    }
            }
        }
    }
}

void LinearHex8::getShapeFunction(double nodes_coordinate[8][3],
                                  std::vector<std::vector<double>> &GaussPoints,
                                  double *ShapeFunction[8],
                                  double *ShapeFunction_dxy[8][3],
                                  double *value_jkb,
                                  const int num_GP)
{
    if (!num_GP == GaussPoints.size())
    {
        std::cout << "数组长度不匹配" << std::endl;
        exit(0);
    }
    std::cout<< "pass 0" << std::endl;
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
    std::cout << "pass 1" << std::endl;
    // 计算形函数导数和雅可比行列式值
    for (int i = 0; i < num_GP; i++)
    {
        Eigen::Matrix<double, 3, 8> jl;
        double x1[3] = {0.0};
        x1[0] = GaussPoints[i][0];
        x1[1] = GaussPoints[i][1];
        x1[2] = GaussPoints[i][2];
        jl(0, 0) = -(1.0 - x1[1]) * (1.0 - x1[2]) / 8.0;
        jl(0, 1) = -jl(0, 0);
        jl(0, 2) = (1.0 + x1[1]) * (1.0 - x1[2]) / 8.0;
        jl(0, 3) = -jl(0, 2);
        jl(0, 4) = -(1.0 - x1[1]) * (1.0 + x1[2]) / 8.0;
        jl(0, 5) = -jl(0, 4);
        jl(0, 6) = (1.0 + x1[1]) * (1.0 + x1[2]) / 8.0;
        jl(0, 7) = -jl(0, 6);

        jl(1, 0) = -(1.0 - x1[0]) * (1.0 - x1[2]) / 8.0;
        jl(1, 1) = -(1.0 + x1[0]) * (1.0 - x1[2]) / 8.0;
        jl(1, 2) = (1.0 + x1[0]) * (1.0 - x1[2]) / 8.0;
        jl(1, 3) = (1.0 - x1[0]) * (1.0 - x1[2]) / 8.0;
        jl(1, 4) = -(1.0 - x1[0]) * (1.0 + x1[2]) / 8.0;
        jl(1, 5) = -(1.0 + x1[0]) * (1.0 + x1[2]) / 8.0;
        jl(1, 6) = (1.0 + x1[0]) * (1.0 + x1[2]) / 8.0;
        jl(1, 7) = (1.0 - x1[0]) * (1.0 + x1[2]) / 8.0;

        // 第三行
        jl(2, 0) = -(1.0 - x1[0]) * (1.0 - x1[1]) / 8.0;
        jl(2, 1) = -(1.0 + x1[0]) * (1.0 - x1[1]) / 8.0;
        jl(2, 2) = -(1.0 + x1[0]) * (1.0 + x1[1]) / 8.0;
        jl(2, 3) = -(1.0 - x1[0]) * (1.0 + x1[1]) / 8.0;
        jl(2, 4) = -jl(2, 0);
        jl(2, 5) = -jl(2, 1);
        jl(2, 6) = -jl(2, 2);
        jl(2, 7) = -jl(2, 3);

        Eigen::Matrix<double, 8, 3> nodes_coordinate_Eigen;
        for (int row = 0; row < 8; row++)
            for (int col = 0; col < 3; col++)
            {
                nodes_coordinate_Eigen(row, col) = nodes_coordinate[row][col];
            }

        Eigen::Matrix<double, 3, 3> jkb = jl * nodes_coordinate_Eigen;
        Eigen::Matrix<double, 3, 3> jkb_inv = jkb.inverse();
        Eigen::Matrix<double, 3, 8> DSF = jkb_inv * jl;
        value_jkb[i] = jkb.determinant();

        for (int ii = 0; ii < 8; ii++)
        {
            ShapeFunction_dxy[i][ii][0] = DSF(0, ii);
            ShapeFunction_dxy[i][ii][1] = DSF(1, ii);
            ShapeFunction_dxy[i][ii][2] = DSF(2, ii);
        }
    }
}

void LinearHex8::SetGaussIntegration(const int intergrationorder, std::vector<std::vector<double>>& GaussPoints)
{
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
            }
        }
    }
}