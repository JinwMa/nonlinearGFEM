
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include "mesh.h"

int main() {
  std::cout.precision(20);
  int tff;
  std::string filename = "/home/ma/work/M3D-C/file/a.dat";

  Mesh mesh1;
  mesh1.test();

  std::ifstream infile(filename);
  if (!infile) {
    std::cerr << "无法打开文件: " << filename << std::endl;
  }
  infile >> tff;

  std::cout << tff << std::endl;

  Eigen::SparseMatrix<double> mat(tff, tff);
  std::vector<Eigen::Triplet<double>> tripletList;
  // ... 添加更多的元素

  // 读取文件中的每一行
  int row, col;
  double value;
  while (infile >> row >> col >> value) {
    tripletList.push_back(Eigen::Triplet<double>(row, col, value));
  }

  mat.setFromTriplets(tripletList.begin(), tripletList.end());

  Eigen::VectorXd b(tff);

  std::string filename2 = "/home/ma/work/M3D-C/file/b.dat";

  std::ifstream infile2(filename2);
  if (!infile2) {
    std::cerr << "无法打开文件: " << filename2 << std::endl;
  }

  for (int i = 0; i < tff; i++)
    infile2 >> b(i);

  std::clock_t c_start = std::clock();
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Upper> solver;

  // Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;

  // Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;

  // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;

  solver.compute(mat);
  if (solver.info() != Eigen::Success) {
    // 分解失败
    std::cerr << "分解失败" << std::endl;
    return -1;
  }

  Eigen::VectorXd x = solver.solve(b);
  if (solver.info() != Eigen::Success) {
    // 求解失败
    std::cerr << "求解失败" << std::endl;
    return -1;
  }
  std::clock_t c_end = std::clock();
  double cpu_time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
  std::cout << "CPU 时间(毫秒): " << cpu_time << " ms" << std::endl;
  // 输出解 x
  // std::cout << "解 x:\n" << x << std::endl;

  return 0;
}