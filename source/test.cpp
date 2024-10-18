#include <ctime>
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include "mesh.h"
#include "input.h"
#include "elementassembler.h"
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <map>
#include <Eigen/Dense>


void getFlexibilitMatrix(double xa[3], double xb[3], double ZSS[36], double DIA, double slen)
{
  for (int i = 0; i < 36; i++) ZSS[i] = 0;
  double r[3] = {0.0};
  r[0] = xb[0] - xa[0];
  r[1] = xb[1] - xa[1];
  r[2] = xb[2] - xa[2];
  slen = std::sqrt(r[0] * r[0] + r[1] * r[1] + r[2] * r[2]);
  ZSS[21] = slen;
  ZSS[28] = slen;
  ZSS[35] = slen;
  double fact = slen / 12.0 * ((3.0 * DIA * DIA) / (2.0 * slen * slen) - 1.0);
  double l312 = slen * slen * slen / 12.0;
  ZSS[0] = l312 + fact * r[0] * r[0];
  ZSS[1] = fact * r[0] * r[1];
  ZSS[2] = fact * r[0] * r[2];
  ZSS[6] = ZSS[1];
  ZSS[7] = l312 + fact * r[1] * r[1];
  ZSS[8] = fact * r[1] * r[2];
  ZSS[12] = ZSS[2];
  ZSS[13] = ZSS[8];
  ZSS[14] = l312 + fact * r[2] * r[2];
};



void test()
{

    int tff;
    std::string filename = "/home/ma/work/M3D-C/file/a.dat";
    std::ifstream infile(filename);
    if (!infile)
    {
        std::cerr << "无法打开文件: " << filename << std::endl;
    }
    infile >> tff;

    std::cout << tff << std::endl;

    Eigen::SparseMatrix<double> mat(tff, tff);
    std::vector<Eigen::Triplet<double>> tripletList;

    // 读取文件中的每一行
    int row, col;
    double value;

    std::clock_t c_start_readstart = std::clock();
    while (infile >> row >> col >> value)
    {
        tripletList.push_back(Eigen::Triplet<double>(row, col, value));
    }
    mat.setFromTriplets(tripletList.begin(), tripletList.end());
    Eigen::VectorXd b(tff);
    std::string filename2 = "/home/ma/work/M3D-C/file/b.dat";
    std::ifstream infile2(filename2);
    if (!infile2)
    {
        std::cerr << "无法打开文件: " << filename2 << std::endl;
    }

    for (int i = 0; i < tff; i++)
        infile2 >> b(i);

    std::clock_t c_start = std::clock();
    std::cout << "time of read input" << 1000.0 * (c_start - c_start_readstart) / CLOCKS_PER_SEC << std::endl;
    // Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Upper> solver;
    // Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    // Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
    // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;
    // Eigen::PardisoLLT<Eigen::SparseMatrix<double>> solver;
    //  Eigen::PardisoLDLT<Eigen::SparseMatrix<double>> solver;
    // solver.setTolerance(1e+5);   // 设置收敛误差
    // solver.setMaxIterations(10); // 设置最大迭代次数
    solver.compute(mat);
    if (solver.info() != Eigen::Success)
    {
        // 分解失败
        std::cerr << "分解失败" << std::endl;
        exit(0);
    }
    Eigen::VectorXd x = solver.solve(b);
    if (solver.info() != Eigen::Success)
    {
        // 求解失败
        std::cerr << "求解失败" << std::endl;
        exit(0);
    }
    std::clock_t c_end = std::clock();
    double cpu_time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
    std::cout << "CPU 时间(毫秒): " << cpu_time << " ms" << std::endl;
    // 输出解 x
    // std::cout << "解 x:\n" << x << std::endl;
}