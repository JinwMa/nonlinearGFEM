#include "post.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <iomanip>
#include "node.h"

using namespace std;

void Post::onlymesh(Mesh *pmesh)
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
    // outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << pmesh->actual_element_count << ", F=FEPOINT, ET=brick" << std::endl;
    outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << pmesh->actual_element_count << ", F=FEPOINT, ET=TETRAHEDRON" << std::endl;


    // 输出节点
    for (int i = 0; i < pmesh->actual_node_count; i++)
    {
        int nodeid = pmesh->NodeIdList[i];
        int nodeorder = pmesh->NodeOrderInList[nodeid];
        outputFile << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][0] << "   " << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][1] << "   " << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][2] << std::endl;
    }

    // 输出单元
    for (int i = 0; i < pmesh->actual_element_count; i++)
    {
        int elementid = pmesh->ElementIdList[i];
        int elementorder = pmesh->ElementOrderInList[elementid];
        vector<int> element_connect = pmesh->NodesOnElements[elementorder - 1];

        if (element_connect.size() == 4) // 四面体单元
        {
            for (int ii = 0; ii < 4; ii++)
            {
                outputFile << std::setw(20) << pmesh->NodeOrderInList[element_connect[ii]];
            }
            outputFile << std::endl;
        }
        else if (element_connect.size() == 8) // 六面体单元
        {
            for (int ii = 0; ii < 8; ii++)
            {
                outputFile << std::setw(20) << pmesh->NodeOrderInList[element_connect[ii]];
            }
            outputFile << std::endl;
        }
    }

    outputFile.close();
}

void Post::ShowDisplacement(Mesh *pmesh, Dof_Map *pdofmap, vector<double> displacement)
{
    string filename = outputFilename + "_displacement.dat";
    std::ofstream outputFile(filename); // 打开文件
    if (!outputFile)
    {
        std::cerr << "无法打开文件:" << filename << std::endl;
        exit(0);
    }
    outputFile << std::fixed << std::setprecision(7);
    BuildPostNodes(pmesh, pdofmap, displacement);

    outputFile << std::fixed << std::setprecision(7);

    outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
    outputFile << "VARIABLES = \"X\", \"Y\", \"Z\",  \"ux\",  \"uy\",  \"uz\"" << std::endl;
    // outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << pmesh->actual_element_count << ", F=FEPOINT, ET=brick" << std::endl;
    outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << pmesh->actual_element_count << ", F=FEPOINT, ET=TETRAHEDRON" << std::endl;


    // 输出节点
    for (size_t i = 0; (int)i < PostNodes.size(); i++)
    {
        outputFile << std::setw(20) << PostNodes[i].x << "   " << std::setw(20) << PostNodes[i].y << "   " << std::setw(20) << PostNodes[i].z
                   << std::setw(20) << PostNodes[i].ux << "   " << std::setw(20) << PostNodes[i].uy << "   " << std::setw(20) << PostNodes[i].uz << std::endl;
    }

    // 输出单元
    for (int i = 0; i < pmesh->actual_element_count; i++)
    {
        int elementid = pmesh->ElementIdList[i];
        int elementorder = pmesh->ElementOrderInList[elementid];
        vector<int> element_connect = pmesh->NodesOnElements[elementorder - 1];

        if (element_connect.size() == 4) // 四面体单元
        {
            for (int ii = 0; ii < 4; ii++)
            {
                outputFile << std::setw(20) << pmesh->NodeOrderInList[element_connect[ii]];
            }
            outputFile << std::endl;
        }
        else if (element_connect.size() == 8) // 六面体单元
        {
            for (int ii = 0; ii < 8; ii++)
            {
                outputFile << std::setw(20) << pmesh->NodeOrderInList[element_connect[ii]];
            }
            outputFile << std::endl;
        }
    }

    outputFile.close();
}

void Post::BuildPostNodes(Mesh *pmesh, Dof_Map *pdofmap, vector<double> displacement)
{
    PostNodes.resize(pmesh->actual_node_count);
    for (int i = 0; i < pmesh->actual_node_count; i++)
    {
        int nodeid = pmesh->NodeIdList[i];
        int nodeorder = pmesh->NodeOrderInList[nodeid];
        PostNodes[i].x = pmesh->NodesCoordinate[nodeorder - 1][0];
        PostNodes[i].y = pmesh->NodesCoordinate[nodeorder - 1][1];
        PostNodes[i].z = pmesh->NodesCoordinate[nodeorder - 1][2];

        // int index = pdofmap->NodesIndex[nodeorder - 1];
        PostNodes[i].ux = displacement[pdofmap->getDofIndex(nodeid, "ux")];
        PostNodes[i].uy = displacement[pdofmap->getDofIndex(nodeid, "uy")];
        PostNodes[i].uz = displacement[pdofmap->getDofIndex(nodeid, "uz")];
    }
}

void Post::check_error(Input *pinput, Mesh *pmesh, Dof_Map *pdofmap)
{
    std::vector<double> tx, ty, tz;
    if (pinput->ifExist("theoretical_solution_ux"))
    {
        tx = pinput->getVectorDouble("theoretical_solution_ux");
    }
    if (pinput->ifExist("theoretical_solution_uy"))
    {
        ty = pinput->getVectorDouble("theoretical_solution_uy");
    }
    if (pinput->ifExist("theoretical_solution_uz"))
    {
        tz = pinput->getVectorDouble("theoretical_solution_uz");
    }


    vector<double> uxext, uyext, uzext;
    vector<double> uxnum, uynum, uznum;
    uxext.resize(PostNodes.size());
    uyext.resize(PostNodes.size());
    uzext.resize(PostNodes.size());

    uxnum.resize(PostNodes.size());
    uynum.resize(PostNodes.size());
    uznum.resize(PostNodes.size());

    for (int i = 0; i < PostNodes.size(); i++)
    {
        uxext[i] += tx[0] + tx[1] * PostNodes[i].x +  tx[2] * PostNodes[i].y + tx[3] * PostNodes[i].z;
        uyext[i] += ty[0] + ty[1] * PostNodes[i].x +  ty[2] * PostNodes[i].y + ty[3] * PostNodes[i].z;
        uzext[i] += tz[0] + tz[1] * PostNodes[i].x +  tz[2] * PostNodes[i].y + tz[3] * PostNodes[i].z;

        uxnum[i] = PostNodes[i].ux;
        uynum[i] = PostNodes[i].uy;
        uznum[i] = PostNodes[i].uz;
    }

    toolbox::checkvector(uxext, uxnum);
    toolbox::checkvector(uyext, uynum);
    toolbox::checkvector(uzext, uznum);
}