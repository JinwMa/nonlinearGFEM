#include "post.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <iomanip>
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

void Post::ShowDisplacement(Mesh *pmesh, Dof_Map *pdofmap, vector<double> & displacement)
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

void Post::BuildPostNodes(Mesh *pmesh, Dof_Map *pdofmap, vector<double> & displacement)
{
    PostNodes.resize(pmesh->actual_node_count);
    for (int i = 0; i < pmesh->actual_node_count; i++)
    {
        int nodeid = pmesh->NodeIdList[i];
        int nodeorder = pmesh->NodeOrderInList[nodeid];
        PostNodes[i].X = pmesh->NodesCoordinate[nodeorder - 1][0];
        PostNodes[i].Y = pmesh->NodesCoordinate[nodeorder - 1][1];
        PostNodes[i].Z = pmesh->NodesCoordinate[nodeorder - 1][2];

        // int index = pdofmap->NodesIndex[nodeorder - 1];
        PostNodes[i].ux = displacement[pdofmap->getDofIndex(nodeid, "ux")];
        PostNodes[i].uy = displacement[pdofmap->getDofIndex(nodeid, "uy")];
        PostNodes[i].uz = displacement[pdofmap->getDofIndex(nodeid, "uz")];

        
        PostNodes[i].x = pmesh->NodesCoordinate[nodeorder - 1][0] + PostNodes[i].ux;
        PostNodes[i].y = pmesh->NodesCoordinate[nodeorder - 1][1] + PostNodes[i].uy;
        PostNodes[i].z = pmesh->NodesCoordinate[nodeorder - 1][2] + PostNodes[i].uz;
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
        uxext[i] += tx[0] + tx[1] * PostNodes[i].X + tx[2] * PostNodes[i].Y + tx[3] * PostNodes[i].Z;
        uyext[i] += ty[0] + ty[1] * PostNodes[i].X + ty[2] * PostNodes[i].Y + ty[3] * PostNodes[i].Z;
        uzext[i] += tz[0] + tz[1] * PostNodes[i].X + tz[2] * PostNodes[i].Y + tz[3] * PostNodes[i].Z;

        uxnum[i] = PostNodes[i].ux;
        uynum[i] = PostNodes[i].uy;
        uznum[i] = PostNodes[i].uz;
    }

    std::cout << " the error in ux:" << std::endl;
    toolbox::checkvector(uxext, uxnum);
    std::cout << " the error in uy:" << std::endl;
    toolbox::checkvector(uyext, uynum);
    std::cout << " the error in uz:" << std::endl;
    toolbox::checkvector(uzext, uznum);
}

void Post::onlymesh(Input *pinput, Mesh *pmesh)
{
    string filename = outputFilename + "_mesh.dat";
    std::ofstream outputFile(filename, std::ios::app); // 打开文件
    if (!outputFile)
    {
        std::cerr << "无法打开文件:" << filename << std::endl;
        exit(0);
    }
    outputFile << std::fixed << std::setprecision(7);

    std::vector<std::string> elementList = pinput->getVectorString("element_list");
    for (auto elementname : elementList)
    {
        std::vector<int> element_ids;
        std::string element_set_type = pinput->getString(elementname + "_set_type");
        if (element_set_type == "range")
        {
            std::vector<int> range = pinput->getVectorInt(elementname + "_range");
            if (range.size() != 2)
                toolbox::error("the range of " + elementname + "_range" + "is given wrong");
            int start = range[0];
            int end = range[1];
            int size = end - start + 1;
            element_ids.resize(size);
            for (int i = 0; i < size; i++)
                element_ids[i] = start + i;
        }
        else
        {
            toolbox::error("not support the type of " + element_set_type + " for element ids");
        }
        if (pinput->getString(elementname + "_type") == "LinearHex8" || pinput->getString(elementname + "_type") == "NonLinearHex8")
        {
            outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
            outputFile << "VARIABLES = \"X\", \"Y\", \"Z\"" << std::endl;
            outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << element_ids.size() << ", F=FEPOINT, ET=brick" << std::endl;
            // 输出节点
            for (int i = 0; i < pmesh->actual_node_count; i++)
            {
                int nodeid = pmesh->NodeIdList[i];
                int nodeorder = pmesh->NodeOrderInList[nodeid];
                outputFile << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][0] << "   " << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][1] << "   " << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][2] << std::endl;
            }
            for (int i = 0; i < element_ids.size(); i++)
            {
                int elementid = element_ids[i];
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
        }
        else if (pinput->getString(elementname + "_type") == "LinearTet4")
        {
            outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
            outputFile << "VARIABLES = \"X\", \"Y\", \"Z\"" << std::endl;
            outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << pmesh->actual_element_count << ", F=FEPOINT, ET=TETRAHEDRON" << std::endl;
            //输出节点
            for (int i = 0; i < pmesh->actual_node_count; i++)
            {
                int nodeid = pmesh->NodeIdList[i];
                int nodeorder = pmesh->NodeOrderInList[nodeid];
                outputFile << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][0] << "   " << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][1] << "   " << std::setw(20) << pmesh->NodesCoordinate[nodeorder - 1][2] << std::endl;
            }
            //输出单元
            for (int i = 0; i < element_ids.size(); i++)
            {
                int elementid = element_ids[i];
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
        }
    }

    // 输出节点

    // 输出单元

    outputFile.close();
}



void Post::ShowDisplacement(Input * pinput, Mesh *pmesh, Dof_Map *pdofmap, vector<double> & displacement)
{
    string filename = outputFilename + "_displacement.dat";
    std::ofstream outputFile(filename, std::ios::app); // 打开文件
    if (!outputFile)
    {
        std::cerr << "无法打开文件:" << filename << std::endl;
        exit(0);
    }
    outputFile << std::fixed << std::setprecision(7);
    BuildPostNodes(pmesh, pdofmap, displacement);

    std::vector<std::string> elementList = pinput->getVectorString("element_list");
    for (auto elementname : elementList)
    {
        std::vector<int> element_ids;
        std::string element_set_type = pinput->getString(elementname + "_set_type");
        if (element_set_type == "range")
        {
            std::vector<int> range = pinput->getVectorInt(elementname + "_range");
            if (range.size() != 2)
                toolbox::error("the range of " + elementname + "_range" + "is given wrong");
            int start = range[0];
            int end = range[1];
            int size = end - start + 1;
            element_ids.resize(size);
            for (int i = 0; i < size; i++)
                element_ids[i] = start + i;
        }
        else
        {
            toolbox::error("not support the type of " + element_set_type + " for element ids");
        }
        if (pinput->getString(elementname + "_type") == "LinearHex8" || pinput->getString(elementname + "_type") == "NonLinearHex8")
        {
            outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
            outputFile << "VARIABLES = \"X\", \"Y\", \"Z\",  \"ux\",  \"uy\",  \"uz\"" << std::endl;
            outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << element_ids.size() << ", F=FEPOINT, ET=brick" << std::endl;
             // 输出节点
            for (size_t i = 0; (int)i < PostNodes.size(); i++)
            {
                outputFile << std::setw(20) << PostNodes[i].x << "   " << std::setw(20) << PostNodes[i].y << "   " << std::setw(20) << PostNodes[i].z
                           << std::setw(20) << PostNodes[i].ux << "   " << std::setw(20) << PostNodes[i].uy << "   " << std::setw(20) << PostNodes[i].uz << std::endl;
            }
            
            //输出单元
            for (int i = 0; i < element_ids.size(); i++)
            {
                int elementid = element_ids[i];
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
        }
        else if (pinput->getString(elementname + "_type") == "LinearTet4")
        {
            outputFile << "TITLE = \"Example: 3D Finite-Element Data\"" << std::endl;
            outputFile << "VARIABLES = \"X\", \"Y\", \"Z\",  \"ux\",  \"uy\",  \"uz\"" << std::endl;
            outputFile << "Zone N=  " << pmesh->actual_node_count << ", E=  " << pmesh->actual_element_count << ", F=FEPOINT, ET=TETRAHEDRON" << std::endl;
            // 输出节点
            for (size_t i = 0; (int)i < PostNodes.size(); i++)
            {
                outputFile << std::setw(20) << PostNodes[i].x << "   " << std::setw(20) << PostNodes[i].y << "   " << std::setw(20) << PostNodes[i].z
                           << std::setw(20) << PostNodes[i].ux << "   " << std::setw(20) << PostNodes[i].uy << "   " << std::setw(20) << PostNodes[i].uz << std::endl;
            }
            //输出单元
            for (int i = 0; i < element_ids.size(); i++)
            {
                int elementid = element_ids[i];
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
        }
    }
    outputFile.close();
}