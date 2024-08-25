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
int main(int argc, char *argv[])
{
  // test();

  Input input(argv[1]); // 读入和解析input文件

  for (auto & it : input.db)
  {
    std::cout << it.first << " ";// << it.second;
  }
  exit(0);

  // 读入网格文件
  std::clock_t c_start_mesh = std::clock();
  Mesh mesh(input.db["mesh_file_name"][0]); 
  std::clock_t c_end_mesh = std::clock();
  std::cout << "time of read mesh " << 1000.0 * (c_end_mesh - c_start_mesh) / CLOCKS_PER_SEC << std::endl;

  solve(input, mesh);
  std::clock_t c_end_solve = std::clock();
  std::cout << "time of solve " << 1000.0 * (c_end_solve - c_end_mesh) / CLOCKS_PER_SEC << std::endl;
  
  
  return 0;
}
