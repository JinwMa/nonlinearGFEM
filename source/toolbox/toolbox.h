#ifndef TOOLBOX_H
#define TOOLBOX_H

#include<string>
#include<iostream>
#include<vector>
using namespace std;

class toolbox
{
    public:
    static void error(string info)
    {
        cout << info << endl;
        exit(1);
    }
    static void printvector(vector<double> vec)
    {
        for (size_t i = 0; i < vec.size(); i++)
        std::cout << vec[i] << " ";
        std::cout << std::endl;
    }
    static void printvector(vector<int> vec)
    {
        for (size_t i = 0; i < vec.size(); i++)
        std::cout << vec[i] << " ";
        std::cout << std::endl;
    }

};

#endif