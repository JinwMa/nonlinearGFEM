#ifndef POST1_H
#define POST1_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;
class Posts
{
    public:
    map<string, vector<string> > db;
    std::unordered_set<std::string> keys;
    Posts(const string filename)
    {
        read_input_file(filename);
    }
    private:
    void read_input_file(const string filename);
};

#endif // POST1_H