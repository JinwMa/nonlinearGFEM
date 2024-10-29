#include "LinearTet4.h"
#include <vector>
#include <iostream>
#include <Eigen/Dense>

using namespace std;

void LinearTet4::SetElement()
{
    // 设置维度
    dim = 3;
    // 设置节点自由度数
    numNodeDof = 3;
    // 设置节点数目
    numNodes = 4;
    // 设置自由度标签
    dofs.resize(dim);
    dofs[0] = "ux";
    dofs[1] = "uy";
    dofs[2] = "uz";
    // 设置高斯积分点
    const int integrationorder = integration_order;

    if (integrationorder < 1)
    {
        std::cout << "积分阶次输入错误" << std::endl;
        exit(0);
    }
    const int number_Gauss_point = integrationorder;
    num_intergration_point = integrationorder;
    d_GaussPoints.resize(number_Gauss_point);
    for (int i = 0; i < number_Gauss_point; i++)
    {
        d_GaussPoints[i].resize(5);
    }

    double xs[integrationorder][5];

    if (integrationorder == 1)
    {
        xs[0][0] = 0.25;
        xs[0][1] = 0.25;
        xs[0][2] = 0.25;
        xs[0][3] = 0.25;
        xs[0][4] = 1.0;
    }
    else if (integrationorder == 4)
    {
        xs[0][0] = 0.585410196624969;
        xs[0][1] = 0.138196601125011;
        xs[0][2] = 0.138196601125011;
        xs[0][3] = 0.138196601125011;
        xs[0][4] = 0.25;

        xs[1][0] = 0.138196601125011;
        xs[1][1] = 0.585410196624969;
        xs[1][2] = 0.138196601125011;
        xs[1][3] = 0.138196601125011;
        xs[1][4] = 0.25;

        xs[2][0] = 0.138196601125011;
        xs[2][1] = 0.138196601125011;
        xs[2][2] = 0.585410196624969;
        xs[2][3] = 0.138196601125011;
        xs[2][4] = 0.25;

        xs[3][0] = 0.138196601125011;
        xs[3][1] = 0.138196601125011;
        xs[3][2] = 0.138196601125011;
        xs[3][3] = 0.585410196624969;
        xs[3][4] = 0.25;
    }
    else if (integrationorder == 5)
    {
        xs[0][0] = 0.25;
        xs[0][1] = 0.25;
        xs[0][2] = 0.25;
        xs[0][3] = 0.25;
        xs[0][4] = -0.8;

        xs[1][0] = 0.5;
        xs[1][1] = 0.166666666666667;
        xs[1][2] = 0.166666666666667;
        xs[1][3] = 0.166666666666667;
        xs[1][4] = 0.45;

        xs[2][0] = 0.166666666666667;
        xs[2][1] = 0.5;
        xs[2][2] = 0.166666666666667;
        xs[2][3] = 0.166666666666667;
        xs[2][4] = 0.45;

        xs[3][0] = 0.166666666666667;
        xs[3][1] = 0.166666666666667;
        xs[3][2] = 0.5;
        xs[3][3] = 0.166666666666667;
        xs[3][4] = 0.45;

        xs[4][0] = 0.166666666666667;
        xs[4][1] = 0.166666666666667;
        xs[4][2] = 0.166666666666667;
        xs[4][3] = 0.5;
        xs[4][4] = 0.45;
    }
    else if (integrationorder == 11)
    {
        xs[0][0] = 0.25;
        xs[0][1] = 0.25;
        xs[0][2] = 0.25;
        xs[0][3] = 0.25;
        xs[0][4] = -0.013155555555556 * 6.0;

        xs[1][0] = 0.785714285714286;
        xs[1][1] = 0.071428571428571;
        xs[1][2] = 0.071428571428571;
        xs[1][3] = 0.071428571428571;
        xs[1][4] = 0.007622222222222 * 6.0;

        xs[2][0] = 0.071428571428571;
        xs[2][1] = 0.785714285714286;
        xs[2][2] = 0.071428571428571;
        xs[2][3] = 0.071428571428571;
        xs[2][4] = 0.007622222222222 * 6.0;

        xs[3][0] = 0.071428571428571;
        xs[3][1] = 0.071428571428571;
        xs[3][2] = 0.785714285714286;
        xs[3][3] = 0.071428571428571;
        xs[3][4] = 0.007622222222222 * 6.0;

        xs[4][0] = 0.071428571428571;
        xs[4][1] = 0.071428571428571;
        xs[4][2] = 0.071428571428571;
        xs[4][3] = 0.785714285714286;
        xs[4][4] = 0.007622222222222 * 6.0;

        xs[5][0] = 0.399403576166799;
        xs[5][1] = 0.399403576166799;
        xs[5][2] = 0.100596423833201;
        xs[5][3] = 0.100596423833201;
        xs[5][4] = 0.024888888888889 * 6.0;

        xs[6][0] = 0.100596423833201;
        xs[6][1] = 0.399403576166799;
        xs[6][2] = 0.399403576166799;
        xs[6][3] = 0.100596423833201;
        xs[6][4] = 0.024888888888889 * 6.0;

        xs[7][0] = 0.100596423833201;
        xs[7][1] = 0.100596423833201;
        xs[7][2] = 0.399403576166799;
        xs[7][3] = 0.399403576166799;
        xs[7][4] = 0.024888888888889 * 6.0;

        xs[8][0] = 0.399403576166799;
        xs[8][1] = 0.100596423833201;
        xs[8][2] = 0.100596423833201;
        xs[8][3] = 0.399403576166799;
        xs[8][4] = 0.024888888888889 * 6.0;

        xs[9][0] = 0.399403576166799;
        xs[9][1] = 0.100596423833201;
        xs[9][2] = 0.399403576166799;
        xs[9][3] = 0.100596423833201;
        xs[9][4] = 0.024888888888889 * 6.0;

        xs[10][0] = 0.100596423833201;
        xs[10][1] = 0.399403576166799;
        xs[10][2] = 0.100596423833201;
        xs[10][3] = 0.399403576166799;
        xs[10][4] = 0.024888888888889 * 6.0;
    }
    else
    {
        std::cout << "不支持的积分阶次" << std::endl;
        exit(0);
    }
    for (int i = 0; i < integrationorder; i++)
    {
        for (int j = 0; j < 5; j++)
            d_GaussPoints[i][j] = xs[i][j];
    }
}

void LinearTet4::getShapeFunction(double nodes_coordinate[20][3],
                                  std::vector<std::vector<double>> &GaussPoints,
                                  vector<vector<double>> &ShapeFunction,
                                  vector<vector<vector<double>>> &ShapeFunction_dxy,
                                  vector<double> &value_jkb,
                                  const int num_GP)
{
    std::cout.precision(8);
    if (!num_GP == GaussPoints.size())
    {
        std::cout << "数组长度不匹配" << std::endl;
        exit(0);
    }
    // 为形函数开辟空间
    ShapeFunction.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
        ShapeFunction[i].resize(numNodes);
    // 为形函数导数开辟空间
    ShapeFunction_dxy.resize(num_GP);
    for (int i = 0; i < num_GP; i++)
        ShapeFunction_dxy[i].resize(numNodes);
    for (int i = 0; i < num_GP; i++)
        for (int j = 0; j < numNodes; j++)
            ShapeFunction_dxy[i][j].resize(dim);
    // 为积分点上的雅可比行列式值开辟空间
    value_jkb.resize(num_GP);

    double AA[numNodes];
    double BB[numNodes];
    double CC[numNodes];
    double DD[numNodes];

    // AA数组计算
    AA[0] = nodes_coordinate[1][0] * (nodes_coordinate[2][1] * nodes_coordinate[3][2] - nodes_coordinate[2][2] * nodes_coordinate[3][1]) -
            nodes_coordinate[1][1] * (nodes_coordinate[2][0] * nodes_coordinate[3][2] - nodes_coordinate[2][2] * nodes_coordinate[3][0]) +
            nodes_coordinate[1][2] * (nodes_coordinate[2][0] * nodes_coordinate[3][1] - nodes_coordinate[2][1] * nodes_coordinate[3][0]);

    // BB数组计算
    BB[0] = 1.0 * (nodes_coordinate[2][1] * nodes_coordinate[3][2] - nodes_coordinate[2][2] * nodes_coordinate[3][1]) -
            nodes_coordinate[1][1] * (nodes_coordinate[3][2] - nodes_coordinate[2][2]) +
            nodes_coordinate[1][2] * (nodes_coordinate[3][1] - nodes_coordinate[2][1]);

    BB[1] = 1.0 * (nodes_coordinate[2][1] * nodes_coordinate[3][2] - nodes_coordinate[2][2] * nodes_coordinate[3][1]) -
            nodes_coordinate[0][1] * (nodes_coordinate[3][2] - nodes_coordinate[2][2]) +
            nodes_coordinate[0][2] * (nodes_coordinate[3][1] - nodes_coordinate[2][1]);

    BB[2] = 1.0 * (nodes_coordinate[1][1] * nodes_coordinate[3][2] - nodes_coordinate[1][2] * nodes_coordinate[3][1]) -
            nodes_coordinate[0][1] * (nodes_coordinate[3][2] - nodes_coordinate[1][2]) +
            nodes_coordinate[0][2] * (nodes_coordinate[3][1] - nodes_coordinate[1][1]);

    BB[3] = 1.0 * (nodes_coordinate[1][1] * nodes_coordinate[2][2] - nodes_coordinate[1][2] * nodes_coordinate[2][1]) -
            nodes_coordinate[0][1] * (nodes_coordinate[2][2] - nodes_coordinate[1][2]) +
            nodes_coordinate[0][2] * (nodes_coordinate[2][1] - nodes_coordinate[1][1]);

    BB[0] = -BB[0];
    BB[2] = -BB[2];

    // CC数组计算
    CC[0] = 1.0 * (nodes_coordinate[2][0] * nodes_coordinate[3][2] - nodes_coordinate[2][2] * nodes_coordinate[3][0]) -
            nodes_coordinate[1][0] * (nodes_coordinate[3][2] - nodes_coordinate[2][2]) +
            nodes_coordinate[1][2] * (nodes_coordinate[3][0] - nodes_coordinate[2][0]);

    CC[1] = 1.0 * (nodes_coordinate[2][0] * nodes_coordinate[3][2] - nodes_coordinate[2][2] * nodes_coordinate[3][0]) -
            nodes_coordinate[0][0] * (nodes_coordinate[3][2] - nodes_coordinate[2][2]) +
            nodes_coordinate[0][2] * (nodes_coordinate[3][0] - nodes_coordinate[2][0]);

    CC[2] = 1.0 * (nodes_coordinate[1][0] * nodes_coordinate[3][2] - nodes_coordinate[1][2] * nodes_coordinate[3][0]) -
            nodes_coordinate[0][0] * (nodes_coordinate[3][2] - nodes_coordinate[1][2]) +
            nodes_coordinate[0][2] * (nodes_coordinate[3][0] - nodes_coordinate[1][0]);

    CC[3] = 1.0 * (nodes_coordinate[1][0] * nodes_coordinate[2][2] - nodes_coordinate[1][2] * nodes_coordinate[2][0]) -
            nodes_coordinate[0][0] * (nodes_coordinate[2][2] - nodes_coordinate[1][2]) +
            nodes_coordinate[0][2] * (nodes_coordinate[2][0] - nodes_coordinate[1][0]);

    CC[1] = -CC[1];
    CC[3] = -CC[3];

    // DD数组计算
    DD[0] = 1.0 * (nodes_coordinate[2][0] * nodes_coordinate[3][1] - nodes_coordinate[2][1] * nodes_coordinate[3][0]) -
            nodes_coordinate[1][0] * (nodes_coordinate[3][1] - nodes_coordinate[2][1]) +
            nodes_coordinate[1][1] * (nodes_coordinate[3][0] - nodes_coordinate[2][0]);

    DD[1] = 1.0 * (nodes_coordinate[2][0] * nodes_coordinate[3][1] - nodes_coordinate[2][1] * nodes_coordinate[3][0]) -
            nodes_coordinate[0][0] * (nodes_coordinate[3][1] - nodes_coordinate[2][1]) +
            nodes_coordinate[0][1] * (nodes_coordinate[3][0] - nodes_coordinate[2][0]);

    DD[2] = 1.0 * (nodes_coordinate[1][0] * nodes_coordinate[3][1] - nodes_coordinate[1][1] * nodes_coordinate[3][0]) -
            nodes_coordinate[0][0] * (nodes_coordinate[3][1] - nodes_coordinate[1][1]) +
            nodes_coordinate[0][1] * (nodes_coordinate[3][0] - nodes_coordinate[1][0]);

    DD[3] = 1.0 * (nodes_coordinate[1][0] * nodes_coordinate[2][1] - nodes_coordinate[1][1] * nodes_coordinate[2][0]) -
            nodes_coordinate[0][0] * (nodes_coordinate[2][1] - nodes_coordinate[1][1]) +
            nodes_coordinate[0][1] * (nodes_coordinate[2][0] - nodes_coordinate[1][0]);

    DD[0] = -DD[0];
    DD[2] = -DD[2];

    double volum = (1.0 / 6.0) * (AA[0] + BB[0] * nodes_coordinate[0][0] + CC[0] * nodes_coordinate[0][1] + DD[0] * nodes_coordinate[0][2]);
    if (volum < 0) toolbox::error(" a volum of tet4 element is < 0 ");

    // 计算形函数 和 形函数导数
    for (int i = 0; i < num_GP; i++)
    {
        for (int j = 0; j < numNodes; j++)
        {
            ShapeFunction[i][j] = GaussPoints[i][j];
            ShapeFunction_dxy[i][j][0] = (1.0 / 6.0) * BB[j] / volum;
            ShapeFunction_dxy[i][j][1] = (1.0 / 6.0) * CC[j] / volum;
            ShapeFunction_dxy[i][j][2] = (1.0 / 6.0) * DD[j] / volum;

        }
    }
    // 计算雅可比值
    for (int i = 0; i < value_jkb.size(); i++)
        value_jkb[i] = volum;
}



void LinearTet4::ComputeStiffness(double nodes_coordinate[20][3],
                                  std::vector<double> & displacement,
                                  std::vector<double> & du,
                                  std::vector<double> & ddu,
                                  vector<double> & elementmat)
{
    // 先给elementmat清零
    elementmat.resize(num_edofs * num_edofs);
    // 材料参数先给一个默认值
    double matE = pmaterial->E;
    double v = pmaterial->u;
    // 形函数和形函数导数
    vector<vector<double>> SF;
    vector<vector<vector<double>>> SF_dxyz;
    vector<double> detJ;
    this->getShapeFunction(nodes_coordinate, d_GaussPoints, SF, SF_dxyz, detJ, num_intergration_point);

    double D[6][6] = {};
    double F = (matE * (1.0 - v)) / ((1.0 - 2.0 * v) * (1 + v));
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
        double w = d_GaussPoints[i][4];
        double J = detJ[i];
        for (int ii = 0; ii < numNodes; ii++)
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

            for (int jj = 0; jj < numNodes; jj++)
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

void LinearTet4::getShapeFunctionOnIntegrationPoint(std::vector<double> &p_coord,
                                                std::vector<double> &sf)
{
    if (p_coord.size() != 5)toolbox::error("the size of p_coord is wrong");
    sf.resize(4);
    sf[0] = p_coord[0];
    sf[1] = p_coord[1];
    sf[2] = p_coord[2];
    sf[3] = p_coord[3];
}