#ifndef POST1_H
#define POST1_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>
#include "input.h"
#include "mesh.h"
#include "dof_map.h"
#include "node.h"
#include "toolbox.h"
using namespace std;
class Post
{
    public:
    Post(const std::string &filename): outputFilename(filename)
    {
    }
    void onlymesh(Mesh * pmesh);
    void ShowDisplacement(Mesh * pmesh, Dof_Map * pdofmap, vector<double> dis);
    void BuildPostNodes(Mesh * pmesh, Dof_Map * pdofmap, vector<double> dis);
    void check_error(Input * pinput, Mesh * pmesh, Dof_Map * pdofmap);
    private:
    string outputFilename;
    vector<Node> PostNodes;

};

#endif // POST1_H