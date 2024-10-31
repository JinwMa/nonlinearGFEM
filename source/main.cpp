#include <iostream>
#include <ctime>
#include <chrono>

#include "mesh.h"
#include "input.h"


void solve(Input * pinput, Mesh * pmesh);
int main(int argc, char *argv[])
{
    // 读入和解析input文件    
    auto start = std::chrono::high_resolution_clock::now();
    Input input(argv[1]);
    Input *p_input = &input;
    auto end = std::chrono::high_resolution_clock::now();    
    std::chrono::duration<double, std::milli> duration = end - start;    
    std::cout << "time of read input data: " << duration.count() << " ms" << std::endl;

    Mesh mesh(p_input, p_input->db["mesh_file_name"][0]);
    Mesh *p_mesh = &mesh;
    start = std::chrono::high_resolution_clock::now();  
    duration = start - end;
    std::cout << "time of read mesh data: " << duration.count() << " ms" << std::endl;

    solve(p_input, p_mesh);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;    
    std::cout << "time for all solve: " << duration.count() << " ms" << std::endl;

    
    return 0;
}
