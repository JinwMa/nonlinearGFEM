#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <chrono>

#include "mesh.h"
#include "input.h"
#include "dof_map.h"
#include "post.h"
#include "SolverInterface.h"
#include "ConstraintManager.h"
#include "element.h"

// void solve(Input &input, Mesh &mesh);
// void test();
// Eigen::MatrixXd computeNullSpace(const Eigen::SparseMatrix<double>& C);
void solve_equation(Eigen::SparseMatrix<double>& K, 
                    Eigen::SparseMatrix<double>& C,
                    Eigen::VectorXd & b,
                    Eigen::VectorXd & g,
                    Eigen::VectorXd & a);
Eigen::MatrixXd computeNullSpace2(const Eigen::SparseMatrix<double>& C);


int main(int argc, char *argv[])
{
  // test();

  Input input(argv[1]); // 读入和解析input文件


  std::cout << "The input parameters for the current problem:" << std::endl;
  for (auto & it : input.db)
  {
    std::cout << "##########-- a piece of db --##########" <<std::endl;
    std::cout << it.first << " \n";// << it.second;
    for (size_t i = 0; i < it.second.size(); i++)
      std::cout << it.second[i] << std::endl;    
  }

  // auto solver = new LinearStaticSolver;
  // solver->solve();  

  std::clock_t c_start_mesh = std::clock();
  Mesh mesh(input.db["mesh_file_name"][0]);

  Dof_Map DofMap(mesh);
  DofMap.BuildDofMap(mesh);

  auto constraint_manager = new ConstraintManager();

  constraint_manager->takeDB(input, mesh);
  Eigen::SparseMatrix<double> C = constraint_manager->buildConstrintMatrix(mesh);

  

  
  return 0;
}

void solve_equation(Eigen::SparseMatrix<double>& K, 
                    Eigen::SparseMatrix<double>& C,
                    Eigen::VectorXd & b,
                    Eigen::VectorXd & g,
                    Eigen::VectorXd & a)
{
  int rowsK = K.rows();
  int colsK = K.cols();
  int rowsC = K.rows();
  int colsC = K.cols();

  // 创建一个大的稀疏矩阵
  Eigen::SparseMatrix<double> bigMatrix(rowsK + rowsC, colsK + colsC);

  bigMatrix.setZero();

  // 填充左上角为 k
  bigMatrix.topLeftCorner(rowsK, colsK) = K;

  // // 填充右上角为 c
  // bigMatrix.topRightCorner(rowsK, colsC) = C;

  // Eigen::SparseMatrix<double> temp = C.transpose(); // 先存储转置结果
  // bigMatrix.bottomLeftCorner(rowsC, colsK) = temp;  // 再进行赋值
};
