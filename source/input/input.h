#ifndef INPUT1_H
#define INPUT1_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <memory>
using namespace std;


class DataBase
{
    public:
    DataBase(){};
    ~DataBase(){};

    vector<int> getVectorInt(string);
    vector<double> getVectorDouble(string);
    int getInt(string);
    double getDouble(string);
    string getString(string);
    vector<string> getVectorString(string);
    std::shared_ptr<DataBase> getDataBase(string);
    bool ifExist(string);

    std::string d_name;
    std::set<std::string> d_all_keys;
    map<string, vector<string>> d_key_values;
    map<string, shared_ptr<DataBase>> d_son_dbs;
    shared_ptr<DataBase> d_father_db;
};


class Input
{
    public:
    Input(const string filename)
    {
        checkInput(filename);
        read(filename);
    }       
    shared_ptr<DataBase> d_root_db;
    private:
    void checkInput(const string & filename);
    int check_line(const string & line);
    void read(const string & filename);
};

#endif // INPUT1_H