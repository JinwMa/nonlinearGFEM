#include "BaseElement.h"
#include <vector>
#include <iostream>
#include <string>
#include "toolbox.h"




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


