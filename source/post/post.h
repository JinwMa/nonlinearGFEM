#ifndef POST1_H
#define POST1_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>
#include "mesh.h"
#include "dof_map.h"
#include "node.h"
using namespace std;
class Post
{
    public:
    Post(const std::string &filename): outputFilename(filename)
    {
    }
    void onlymesh(Mesh * pmesh);
    void ShowDisplacement(Mesh * pmesh, Dof_Map& DofMap, vector<double> dis);
    vector<Node> BuildPostNodes(Mesh * pmesh, Dof_Map& DofMap, vector<double> dis);
    private:
    string outputFilename;

};

#endif // POST1_H