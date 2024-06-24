#include "post.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <iomanip>


using namespace std;

void Post::onlymesh(Mesh & mesh)
{
    
    // string filename = outputFilename + "_mesh.dat";
    // std::ofstream outputFile(filename); // 打开文件
    // if (!outputFile)
    // {
    //     std::cerr << "无法打开文件:" << filename << std::endl;
    //     exit(0);
    // }
    // outputFile << std::fixed << std::setprecision(7);
    // // 临时生效:检查网格中单元的类型
    // if (mesh.mesh_element[0].size() == 4)//按照四面体单元输出
    // {
    //     outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
    //     outputFile << "VARIABLES = \"X\", \"Y\", \"Z\"" << std::endl;
    //     outputFile << "Zone N=  " << mesh.actual_node_count << ", E=  " << mesh.actual_element_count << ", F=FEPOINT, ET=TETRAHEDRON" << std::endl;
    //     for (int i = 0; i < mesh.actual_node_count; i++)
    //     {
    //         int nodeid = mesh.node_ids[i];
    //         outputFile << std::setw(20) << mesh.mesh_node[nodeid - 1][0] << "   " << std::setw(20) << mesh.mesh_node[nodeid - 1][1] << "   " << std::setw(20) << mesh.mesh_node[nodeid - 1][2] << std::endl;
    //     }
    //     for (int i = 0; i < mesh.actual_element_count; i++)
    //     {
    //         outputFile << std::setw(20) << mesh.mesh_element[i][0] << " " << std::setw(20) << mesh.mesh_element[i][1] << " " << std::setw(20) << mesh.mesh_element[i][2] << " " << std::setw(20) << mesh.mesh_element[i][3] << std::endl;
    //     }
    // }
    // else if (mesh.mesh_element[0].size() == 8)//按照六面体单元输出
    //  {
    //     outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
    //     outputFile << "VARIABLES = \"X\", \"Y\", \"Z\"" << std::endl;
    //     outputFile << "Zone N=  " << mesh.actual_node_count << ", E=  " << mesh.actual_element_count << ", F=FEPOINT, ET=brick" << std::endl;
    //     for (int i = 0; i < mesh.actual_node_count; i++)
    //     {
    //         int nodeid = mesh.node_ids[i];
    //         outputFile << std::setw(20) << mesh.mesh_node[nodeid - 1][0] << "   " << std::setw(20) << mesh.mesh_node[nodeid - 1][1] << "   " << std::setw(20) << mesh.mesh_node[nodeid - 1][2] << std::endl;
    //     }
    //     for (int i = 0; i < mesh.actual_element_count; i++)
    //     {
    //         for (int j = 0; j < 8; j++)
    //         outputFile << std::setw(20) << mesh.mesh_element[i][j] << " ";
    //         outputFile << std::endl;
    //     }
    // }


    // outputFile.close();
}