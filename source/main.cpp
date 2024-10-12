

#include "mesh.h"
#include "input.h"
#include "SolverInterface.h"
#include <omp.h>
#include <iostream>

int main(int argc, char *argv[])
{

// #pragma omp parallel for
//     for (int i = 0; i < 100; i++)
// #pragma omp critical
//     {
//         std::cout << i << std::endl;
//     }
//     exit(0);
    // 读入和解析input文件
    Input input(argv[1]);
    Mesh mesh(input.db["mesh_file_name"][0]);

    // 构造输入和网格的指针
    Input *p_input = &input;
    Mesh *p_mesh = &mesh;

    BaseSolver *structsolver;
    structsolver = new LinearStaticSolver;
    structsolver->solve(p_input, p_mesh);

    delete structsolver;

    return 0;
}
