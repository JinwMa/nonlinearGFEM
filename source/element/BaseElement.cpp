#include "BaseElement.h"
#include <vector>
#include <iostream>
#include <string>
#include "toolbox.h"

void BaseElement::takeDB(Input * pinput, Mesh * pmesh, std::string & name)
{
    
    if (pinput->ifExist(name + "_integration_order"))
    {
        integration_order = pinput->getInt(name + "_integration_order");
    }
    if (integration_order <= 0) toolbox::error("the integration_order of " + name + " is wrong");
    d_num_intergration_point = integration_order * integration_order * integration_order;

    //read material
    std::string material_name = pinput->getString(name + "_material");
    std::string material_type = pinput->getString(material_name + "_type");
    if (material_type == "LinearElasticity")
    {
        pmaterial = std::make_shared<LinearElasticity>();
    }
    else if (material_type == "Kirchhoff_hyperelasticity")
    {
        pmaterial = std::make_shared<KirchhoffHyperelasticity>();
    }
    else if (material_type == "Hypoelastic")
    {
        pmaterial = std::make_shared<Hypoelastic>();
    }
    else if (material_type == "Hypoelastoplastic")
    {
        pmaterial = std::make_shared<Hypoelastoplastic>();
    }
    else
    {
        toolbox::error("not support material type: " + material_type);
    }
    pmaterial->takeDB(pinput, material_name);
}

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

double BaseElement::invertMatrix(const std::vector<std::vector<double>> & input,
                                 std::vector<std::vector<double>> & inverse) {
    
    if (input.size() != 3 || inverse.size() != 3) toolbox::error("size error in inverMatrix");
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

void BaseElement::ATensorProductB(const double *A, const int m,
                                  const double *B, const int n,
                                  double *C)
{
    if (m + n > 4) toolbox::error("m + n should not larger than 4");
    if (m == 0)
    {
        if (n == 0)
        {
            C[0] = A[0] * B[0];
        }
        else if (n == 1)
        {
            for (int i = 0; i < 3; i++)
            {
                C[i] = A[0] * B[i];
            }
        }
        else if (n == 2)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[i * 3 + j] = A[0] * B[i * 3 + j];
                }
            }
        }
        else if (n == 3)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[i * 9 + j * 3 + k] = A[0] *  B[i * 9 + j * 3 + k];
                    }
                }
            }
        }
        else if (n == 4)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 27 + j * 9 + k * 3 + l] = A[0] * B[i * 27 + j * 9 + k * 3 + l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("m + n should not larger than 4");
        }
    }    
    else if (m == 1)
    {
        if (n == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                C[i] = A[i] * B[0];
            }
        }
        else if (n == 1)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[i * 3 + j] = A[i] * B[j];
                }
            }
        }
        else if (n == 2)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[i * 9 + j * 3 + k] = A[i] * B[j * 3 + k];
                    }
                }
            }
        }
        else if (n == 3)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 27 + j * 9 + k * 3 + l] = A[i] * B[j * 9 + k * 3 + l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("m + n should not larger than 4");
        }        
    }
    else if (m == 2)
    {
        if (n == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[i * 3 + j] = A[i * 3 + j] * B[0];
                }
            }
        }
        else if (n == 1)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k =0; k < 3; k++)
                    {
                        C[i * 9 + j * 3 + k] = A[i * 3 + j] * B[k];
                    }
                }
            }
        }
        else if (n == 2)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 27 + j * 9 + k * 3 + l] = A[i * 3 + j] * B[k * 3 + l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("m + n should not larger than 4");            
        }
    }
    else if (m == 3)
    {
        if (n == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[i * 9 + j * 3 + k] = A[i * 9 + j * 3 + k] * B[0];
                    }
                }
            }
        }
        else if (n == 1)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 27 + j * 9 + k * 3 + l] = A[i * 9 + j * 3 + k] * B[l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("m + n should not larger than 4");              
        }
    }
    else if (m == 4)
    {
        if (n == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 27 + j * 9 + k * 3 + l] = A[i * 27 + j * 9 + k * 3 + l] * B[0];
                        }
                    }
                }
            }            
        }
        else
        {
            toolbox::error("m + n should not larger than 4");              
        }
    }
}

void BaseElement::ADotB(const double *A, const int m,
                        const double *B, const int n,
                        double *C)
{
    if (m == 0)
    {
        if (n == 0)
        {
            C[0] = A[0] * B[0];
        }
        else if (n == 1)
        {
            for (int i = 0; i < 3; i++)
            {
                C[i] = A[0] * B[i];
            }
        }
        else if (n == 2)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[i * 3 + j] = A[0] * B[i * 3 + j];
                }
            }
        }
        else if (n == 3)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[i * 9 + j * 3 + k] = A[0] *  B[i * 9 + j * 3 + k];
                    }
                }
            }
        }
        else if (n == 4)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 27 + j * 9 + k * 3 + l] = A[0] * B[i * 27 + j * 9 + k * 3 + l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("n should not larger than 4 for m = 0");
        }
    }
    else if (m == 1)
    {
        if (n == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                C[i] = A[i] * B[0];
            }
        }
        else if (n == 1)
        {
            C[0] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                C[0] = C[0] + A[i] * B[i];
            }
        }
        else if (n == 2)
        {
            for (int i = 0; i < 3; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[j] = C[j] + A[i] * B[i * 3 + j];
                }
            }
        }
        else if (n == 3)
        {
            for (int i = 0; i < 9; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[j * 3 + k] += A[i] * B[i * 9 + j * 3 + k];
                    }
                }
            }
        }
        else if (n == 4)
        {
            for (int i = 0; i < 27; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[j * 9 + k * 3 + l] += A[i] * B[i * 27 + j * 9 + k * 3 + l]; 
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("n should not larger than 4 for m = 1");
        }
    }  
    else if (m == 2)
    {
        if (n == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[i * 3 + j] = A[i * 3 + j] * B[0];
                }
            }
        }
        else if (n == 1)
        {
            for (int i = 0; i < 3; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[i] += A[i * 3 + j] * B[j];
                }
            }
        }
        else if (n == 2)
        {
            for (int i = 0; i < 9; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[i * 3 + k] += A[i * 3 + j] * B[j * 3 + k];
                    }
                }
            }
        }
        else if (n == 3)
        {
            for (int i = 0; i < 27; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 9 + k * 3 + l] += A[i * 3 + j] * B[j * 9 + k * 3 + l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("not support m = 2 and n > 3");
        }
    }
    else if (m == 3)
    {
        if (n == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[i * 9 + j * 3 + k] = A[i * 9 + j * 3 + k] * B[0];
                    }
                }
            }
        }
        else if (n == 1)
        {
            for (int i = 0; i < 9; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for ( int k = 0; k < 3; k++)
                    {
                        C[i * 3 + j] = A[i * 9 + j * 3 + k] * B[k];
                    }
                }
            }
        }
        else if (n == 2)
        {
            for (int i = 0; i < 27; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 9 + j * 3 + l] = A[i * 9 + j * 3 + k] * B[k * 3 + l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("not support m = 3 and n > 2");
        }
    }
    else
    {
        toolbox::error("not support m > 3");
    }  
}

void BaseElement::ADotDotB(const double *A, const int m,
                           const double *B, const int n,
                           double *C)
{
    if (m < 2 || n < 2) toolbox::error("m or n should not smaller than 2");
    if (m == 2)
    {
        if (n == 2)
        {
            C[0]= 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    C[0] += A[i * 3 + j] * B[i * 3 + j];
                }
            }
        }
        else if (n == 3)
        {
            for (int i = 0; i < 3; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[k] += A[i * 3 + j] * B[i * 9 + j * 3 + k]; 
                    }
                }
            }
        }
        else if (n == 4)
        {
            for (int i = 0; i < 9; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[k * 3 + l] = A[i * 3 + j] * B[i * 27 + j * 9 + k * 3 + l];
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("n should not larger than 4");
        }
    }
    else if (m == 3)
    {
        if (n == 2)
        {
            for (int i = 0; i < 3; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        C[i] += A[i * 9 + j * 3 + k] * B[j * 3 + k];
                    }
                }
            }
        }
        else if (n == 4)
        {
            for (int i = 0; i < 27; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            for (int m = 0; m < 3; m++)
                            {
                                C[i * 9 + l * 3 + m] += A[i * 9 + j * 3 + k] * B[j * 27 + k * 9 + l * 3 + m];
                            }
                        }
                    }
                }
            }
        }
        else
        {
            toolbox::error("only support n = 2 and n = 4 for m = 3");
        }
    }
    else if (m == 4)
    {
        if (n == 2)
        {
            for (int i = 0; i < 9; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            C[i * 3 + j] += A[i * 27 + j * 9 + k * 3 + l] * B[k * 3 + l];
                        }
                    }
                }
            }
        }
        else if (n == 3)
        {
            for (int i = 0; i < 27; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            for (int m = 0; m < 3; m++)
                            {
                                C[i * 9 + j * 3 + m] += A[i * 27 + j * 9 + k * 3 + l] * B[k* 9 + l * 3 + m];
                            }
                        }
                    }
                }
            }
        }
        else if (n == 4)
        {
            for (int i = 0; i < 81; i++) C[i] = 0.0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        for (int l = 0; l < 3; l++)
                        {
                            for (int m = 0; m < 3; m++)
                            {
                                for (int n = 0; n < 3; n++)
                                {
                                    C[i * 27 + j * 9 + m * 3 + n] += A[i * 27 + j * 9 + k * 3 + l] * B[k * 27 + l * 9 + m * 3 + n];
                                }
                            }
                        }
                    }
                }
            }
        }
        else 
        {
            toolbox::error("not support n > 4 for m = 4");
        }
    }
    else
    {
        toolbox::error("m should not larger than 4");
    }
}