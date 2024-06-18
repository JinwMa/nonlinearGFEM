#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
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
int main(int argc, char *argv[])
{
//    omp_set_num_threads(4);

//     const int size = 500000000;
//     std::vector<int> a(size, 1);
//     std::vector<int> bb(size, 2);
//     std::vector<int> c(size, 0);

//     // 记录非并行for循环的开始时间
//     auto start = std::chrono::high_resolution_clock::now();

//     // 非并行for循环
//     for (int i = 0; i < size; ++i) {
//         c[i] = a[i] + bb[i];
//     }

//     // 记录非并行for循环的结束时间
//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double, std::milli> non_parallel_time = end - start;

//     // 打印非并行for循环的时间
//     std::cout << "Non-parallel for loop time: " << non_parallel_time.count() << " ms" << std::endl;

//     // 清空c向量
//     std::fill(c.begin(), c.end(), 0);

//     // 记录并行for循环的开始时间
//     start = std::chrono::high_resolution_clock::now();

//     // 并行化for循环
//     #pragma omp parallel for
//     for (int i = 0; i < size; ++i) {
//         c[i] = a[i] + bb[i];
//     }

//     // 记录并行for循环的结束时间
//     end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double, std::milli> parallel_time = end - start;

//     // 打印并行for循环的时间
//     std::cout << "Parallel for loop time: " << parallel_time.count() << " ms" << std::endl;

//  exit(0);
  std::cout.precision(20);
  Input input(argv[1]); // 读入和解析input文件
  Mesh mesh(input.db["mesh_file_name"][0]); // 读入网格文件
  Dof_Map dof_map(mesh.node_ids, mesh.actual_node_count);

  Post post("aaa");
  post.onlymesh(mesh);

  // for (int i = 0; i < mesh.actual_element_count; i++)
  // std::cout << mesh.element_ids[i] << std::endl;


  // exit(0);
  std::cout.precision(20);
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
  solver.setTolerance(1e+5);     // 设置收敛误差
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