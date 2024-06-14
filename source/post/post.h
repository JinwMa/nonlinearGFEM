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
using namespace std;
class Post
{
    public:
    Post(const std::string &filename): outputFilename(filename)
    {
    }
    void onlymesh(Mesh & mesh);
    private:
    string outputFilename;
};

#endif // POST1_H