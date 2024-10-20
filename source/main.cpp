#include "mesh.h"
#include "input.h"
#include "SolverInterface.h"
#include <iostream>
#include <ctime>

int main(int argc, char *argv[])
{
    // 读入和解析input文件    
    auto start = std::chrono::high_resolution_clock::now();
    Input input(argv[1]);
    auto end = std::chrono::high_resolution_clock::now();    
    std::chrono::duration<double, std::milli> duration = end - start;    
    std::cout << "time of read input data: " << duration.count() << " ms" << std::endl;

    Mesh mesh(input.db["mesh_file_name"][0]);
    start = std::chrono::high_resolution_clock::now();  
    duration = start - end;
    std::cout << "time of read mesh data: " << duration.count() << " ms" << std::endl;

   

    // 构造输入和网格的指针
    Input *p_input = &input;
    Mesh *p_mesh = &mesh;

    // std::cout << p_mesh->actual_element_count << " " << p_mesh->actual_node_count << std::endl;
    // exit(0);

    BaseSolver *structsolver;
    structsolver = new LinearStaticSolver;
    structsolver->solve(p_input, p_mesh);

    delete structsolver;

    return 0;
}
