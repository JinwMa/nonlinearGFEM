#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <chrono>
#include <mkl_pardiso.h>

void test();
void a();
void b();
int main(int argc, char *argv[])
{
  a();  
  b();
  return 0;
}
