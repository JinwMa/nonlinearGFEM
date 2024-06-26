#include "BaseElement.h"
#include <vector>
#include <iostream>
#include <string>

void BaseElement::AmnXBpq(const double *A, const int m, const int n,
                          const double *B, const int p, const int q,
                          double *C)
{
    if (p != n){
        std::cout << "矩阵相乘输入错误" << std::endl;
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < q; j++)
        {
            for (int k = 0; k < n; k++)
            {
                C[i * q + j] = C[i * q + j] + A[i * n + k] * B[k * q + j];
            }
        }
    }

}

void BaseElement::AXB3663(const double A[3][6], const double B[6][3], double C[3][3])
{
    // 矩阵乘法
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 6; ++k)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
void BaseElement::AXB3883(const double A[3][8], const double B[8][3], double C[3][3])
{
    // 矩阵乘法
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 8; ++k)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void BaseElement::AXB3666(const double A[3][6], const double B[6][6], double C[3][6])
{
    // 矩阵乘法
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            for (int k = 0; k < 6; ++k)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}
void BaseElement::AXB3338(const double A[3][3], const double B[3][8], double C[3][8])
{
    // 矩阵乘法
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

double BaseElement::invertMatrix(const double input[3][3], double inverse[3][3]) {
    // 计算行列式
    double det = input[0][0] * (input[1][1] * input[2][2] - input[2][1] * input[1][2]) -
                 input[0][1] * (input[1][0] * input[2][2] - input[2][0] * input[1][2]) +
                 input[0][2] * (input[1][0] * input[2][1] - input[2][0] * input[1][1]);

    if (det == 0) {
        return -1; // 矩阵不可逆
    }

    double invDet = 1.0 / det;

    // 计算伴随矩阵并除以行列式
    inverse[0][0] = (input[1][1] * input[2][2] - input[2][1] * input[1][2]) * invDet;
    inverse[0][1] = (input[0][2] * input[2][1] - input[0][1] * input[2][2]) * invDet;
    inverse[0][2] = (input[0][1] * input[1][2] - input[0][2] * input[1][1]) * invDet;
    inverse[1][0] = (input[1][2] * input[2][0] - input[1][0] * input[2][2]) * invDet;
    inverse[1][1] = (input[0][0] * input[2][2] - input[0][2] * input[2][0]) * invDet;
    inverse[1][2] = (input[1][0] * input[0][2] - input[0][0] * input[1][2]) * invDet;
    inverse[2][0] = (input[1][0] * input[2][1] - input[2][0] * input[1][1]) * invDet;
    inverse[2][1] = (input[2][0] * input[0][1] - input[0][0] * input[2][1]) * invDet;
    inverse[2][2] = (input[0][0] * input[1][1] - input[1][0] * input[0][1]) * invDet;

    return det;
}