#ifndef INPUT1_H
#define INPUT1_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;
class Input
{
    public:
    map<string, vector<string> > db;
    std::unordered_set<std::string> keys;
    Input(const string filename)
    {
        read_input_file(filename);
    }
    vector<int> getVectorInt(string);
    vector<double> getVectorDouble(string);
    int getInt(string);
    double getDouble(string);
    string getString(string);
    vector<string> getVectorString(string);
    bool ifExist(string);
    private:
    void read_input_file(const string filename);
};

#endif // INPUT1_H