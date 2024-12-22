#include <iostream>
#include <ctime>
#include <chrono>
#include "mesh.h"
#include "input.h"
#include "pre.h"
#include "post.h"


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

    // 读入mesh文件
    Mesh mesh(p_input, p_input->db["mesh_file_name"][0]);
    Mesh *p_mesh = &mesh;
    start = std::chrono::high_resolution_clock::now();  
    duration = start - end;
    std::cout << "time of read mesh data: " << duration.count() << " ms" << std::endl;


    // 先调用前处理
    if (std::getenv("ONLYPRE")||std::getenv("NEEDPRE"))
    {
        Pre pre;
        pre.preprocess(p_input, p_mesh);
        Post post("pretest");
        post.onlymesh(p_input, p_mesh);
        if(std::getenv("ONLYPRE")) exit(0);
    }   


    // 求解：求解器选择
    solve(p_input, p_mesh);
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;    
    std::cout << "time for all solve: " << duration.count() << " ms" << std::endl;

    
    return 0;
}
