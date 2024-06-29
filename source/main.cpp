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
void test();
int main(int argc, char *argv[])
{
  test();

  Input input(argv[1]); // 读入和解析input文件

  // 读入网格文件
  std::clock_t c_start_mesh = std::clock();
  Mesh mesh(input.db["mesh_file_name"][0]); 
  std::clock_t c_end_mesh = std::clock();
  std::cout << "time of read mesh " << 1000.0 * (c_end_mesh - c_start_mesh) / CLOCKS_PER_SEC << std::endl;

  // solve(input, mesh);
  std::clock_t c_end_solve = std::clock();
  std::cout << "time of solve " << 1000.0 * (c_end_solve - c_end_mesh) / CLOCKS_PER_SEC << std::endl;
  
  
  return 0;
}
