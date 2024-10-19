#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

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

    static void checkvector(std::vector<double> &a, std::vector<double> b)
    {
        if (a.size() != b.size())
            error("the vec1 and vec2 compared is do not match in size!!!");
        double absolute_error = 0.0;
        double relative_error = 0.0;
        for (int i = 0; i < a.size(); i++)
        {
            absolute_error += (a[i] - b[i]) * (a[i] - b[i]);
            relative_error += b[i] * b[i];
        }
        absolute_error = std::sqrt(absolute_error);
        relative_error = std::sqrt(relative_error);
        relative_error = absolute_error / relative_error;

        std::cout << "the absolute error is " << absolute_error << std::endl;
        std::cout << "the relative error is " << relative_error << std::endl;
    }
};

#endif