#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <Eigen/Dense>


using namespace std;

class toolbox
{
    private:
    const double eps = 1.E-15;
public:
    static void error(string info)
    {
        cout << info << endl;
        exit(1);
    }
    static void printvector(vector<double> vec)
    {
        for (size_t i = 0; i < vec.size(); i++)
            std::cout << vec[i] << " ";
        std::cout << std::endl;
    }
    static void printvector(vector<int> vec)
    {
        for (size_t i = 0; i < vec.size(); i++)
            std::cout << vec[i] << " ";
        std::cout << std::endl;
    }

    static void checkvector(std::vector<double> &a, std::vector<double> b)
    {
        if (a.size() != b.size())
            error("Vector sizes do not match");
        double absolute_error = 0.0;
        double relative_error = 0.0;
        for (int i = 0; i < a.size(); i++)
        {
            absolute_error += (a[i] - b[i]) * (a[i] - b[i]);
            relative_error += b[i] * b[i];
        }
        absolute_error = std::sqrt(absolute_error);
        relative_error = std::sqrt(relative_error);
        relative_error = absolute_error / relative_error;

        std::cout << "the absolute error is " << absolute_error << std::endl;
        std::cout << "the relative error is " << relative_error << std::endl;
    }

    static double a3_dot_b3(const double a[3], const double b[3])
    {
        return  a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    }


    static void a3_cross_b3(const double a[3], const double b[3], double c[3])
    {
        c[0] = a[1] * b[2] - a[2] * b[1];
        c[1] = a[2] * b[0] - a[0] * b[2];
        c[2] = a[0] * b[1] - a[1] * b[0];
    }

    static void normalize_a3(double a[3])
    {
        double normal = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
        normal = std::sqrt(normal);
        if (normal < 1.e-15) error("the normal of a3 is too small for normalizing");
        a[0] = a[0] / normal;
        a[1] = a[1] / normal;
        a[2] = a[2] / normal;
    }

    static double get_normal(const double a[3])
    {
        return std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    }

    // 参数:参考点,直线方向,待求点
    static double distance_point_to_line(const double refer_point[3], const double dir[3], const double point[3])
    {
        double r[3] = {0.0};
        r[0] = point[0] - refer_point[0];
        r[1] = point[1] - refer_point[1];
        r[2] = point[2] - refer_point[2];
        double dir_temp[3] = {0.0};
        for (int i = 0; i < 3; i++) dir_temp[i] = dir[i];
        normalize_a3(dir_temp);
        double ty = a3_dot_b3(dir_temp, r);
        double normal = get_normal(r);
        return std::sqrt(normal * normal - ty * ty);
    }

    static void Tensor4dotdotTensor2(const double C[3][3][3][3],
                                     const double E[3][3],
                                     double X[3][3]){

        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                X[i][j] = 0.0;
            }
        }
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    for (int l = 0; l < 3; l++)
                    {
                        X[i][j] += C[i][j][k][l] * E[k][l];
                    }
                }
            }
        }
    }

    static void AmnXBpq(const double *A, const int m, const int n,
                              const double *B, const int p, const int q,
                              double *C)
    {
        if (p != n)
        {
            std::cout << "Matrix multiplication input error" << std::endl;
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




    static void PrintVector(std::vector<double> & vector)
    {
        std::cout.precision(20);
        for (size_t i = 0; i < vector.size(); i++)
        {
            std::cout << std::setw(30) << vector[i] << std::endl;
        }
    }

    static void PrintVector(std::vector<std::vector<double>> & vector)
    {
        std::cout.precision(20);
        for (size_t i = 0; i < vector.size(); i++)
        {
            for (size_t j = 0; j < vector[i].size(); j++)
            {
                std::cout << std::setw(30) << vector[i][j];
            }
            std::cout << std::endl;
        }
    }


    static double getEigenVectorNormal(Eigen::VectorXd & a)
    {
        double normal = 0.0;
        int size = a.size();
        for (int i = 0; i < size; i++)
        {
            normal += a[i] * a[i];
        }
        return std::sqrt(normal);
    }


    static void invertMatrix(const double input[3][3], double inverse[3][3]) {
    // 计算行列式
    double det = input[0][0] * (input[1][1] * input[2][2] - input[2][1] * input[1][2]) -
                 input[0][1] * (input[1][0] * input[2][2] - input[2][0] * input[1][2]) +
                 input[0][2] * (input[1][0] * input[2][1] - input[2][0] * input[1][1]);

    if (det == 0) {
        error("Matrix is not invertible");
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

}

static void invertMatrix(const std::vector<std::vector<double>> & input, double inverse[3][3]) {
    // 计算行列式
    double det = input[0][0] * (input[1][1] * input[2][2] - input[2][1] * input[1][2]) -
                 input[0][1] * (input[1][0] * input[2][2] - input[2][0] * input[1][2]) +
                 input[0][2] * (input[1][0] * input[2][1] - input[2][0] * input[1][1]);

    if (det == 0) {
        error("Matrix is not invertible");
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

}

static void invertMatrix(const std::vector<std::vector<double>> & input, std::vector<std::vector<double>> & inverse) {
    // 计算行列式
    double det = input[0][0] * (input[1][1] * input[2][2] - input[2][1] * input[1][2]) -
                 input[0][1] * (input[1][0] * input[2][2] - input[2][0] * input[1][2]) +
                 input[0][2] * (input[1][0] * input[2][1] - input[2][0] * input[1][1]);

    if (det == 0) {
        error("Matrix is not invertible");
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

}

static void getTensorNormal(const double A[3][3], double & normal)
{
    normal = 0.0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            normal += A[i][j] * A[i][j];
        }
    }
    normal = std::sqrt(normal);
}


};

#endif