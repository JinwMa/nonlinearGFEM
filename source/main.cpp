#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include "mesh.h"
#include "input.h"
#include "dof_map.h"
#include "post.h"
#include <omp.h>
#include <chrono>

void solve(Input &input, Mesh &mesh);
int main(int argc, char *argv[])
{
  std::cout.precision(20);

  Input input(argv[1]); // 读入和解析input文件

  // 读入网格文件
  std::clock_t c_start_mesh = std::clock();
  Mesh mesh(input.db["mesh_file_name"][0]); 
  std::clock_t c_end_mesh = std::clock();
  std::cout << "time of read mesh " << 1000.0 * (c_end_mesh - c_start_mesh) / CLOCKS_PER_SEC << std::endl;

  solve(input, mesh);
  std::clock_t c_end_solve = std::clock();
  std::cout << "time of solve " << 1000.0 * (c_end_solve - c_end_mesh) / CLOCKS_PER_SEC << std::endl;
  
  
  exit(0);

  //后面用于测试线性方程组的求解速度,勿删除

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
  Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Upper> solver;
  // Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
  // Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
  // Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
  // Eigen::PardisoLU<Eigen::SparseMatrix<double>> solver;
  solver.setTolerance(1e+5);   // 设置收敛误差
  solver.setMaxIterations(10); // 设置最大迭代次数
  solver.compute(mat);
  if (solver.info() != Eigen::Success)
  {
    // 分解失败
    std::cerr << "分解失败" << std::endl;
    return -1;
  }
  Eigen::VectorXd x = solver.solve(b);
  if (solver.info() != Eigen::Success)
  {
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
