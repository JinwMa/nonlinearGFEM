#include "post.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <iomanip>


using namespace std;

void Post::onlymesh(Mesh & mesh)
{
    
    string filename = outputFilename + "_mesh.dat";
    std::ofstream outputFile(filename); // 打开文件
    if (!outputFile)
    {
        std::cerr << "无法打开文件:" << filename << std::endl;
        exit(0);
    }
    outputFile << std::fixed << std::setprecision(7);

    outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
    outputFile << "VARIABLES = \"X\", \"Y\", \"Z\"" << std::endl;
    outputFile << "Zone N=  " << mesh.actual_node_count << ", E=  " << mesh.actual_element_count << ", F=FEPOINT, ET=brick" << std::endl;

    // 输出节点
    for (int i = 0; i < mesh.actual_node_count; i++)
    {
        int nodeid = mesh.NodeIdList[i];
        int nodeorder = mesh.NodeOrderInList[nodeid];
        outputFile << std::setw(20) << mesh.NodesCoordinate[nodeorder - 1][0] << "   " << std::setw(20) << mesh.NodesCoordinate[nodeorder - 1][1] << "   " << std::setw(20) << mesh.NodesCoordinate[nodeorder - 1][2] << std::endl;
    }

    // 输出单元
    for (int i = 0; i < mesh.actual_element_count; i++)
    {
        int elementid = mesh.ElementIdList[i];
        int elementorder = mesh.ElementOrderInList[elementid];
        vector<int> element_connect = mesh.NodesOnElements[elementorder -1];

        if (element_connect.size() == 4)//四面体单元
        {
            for (int ii = 0; ii < 4; ii++)
            {
                 outputFile << std::setw(20) << element_connect[ii];
            }
            for (int ii = 0; ii < 4; ii++)
            {
                outputFile << std::setw(20) << element_connect[3];
            }
            outputFile << std::endl;
        }
        else if (element_connect.size() == 8) //六面体单元
        {
            for (int ii = 0; ii < 8; ii++)
            {
                outputFile << std::setw(20) << element_connect[ii];
            }
            outputFile << std::endl;            
        }

    }

    outputFile.close();
}