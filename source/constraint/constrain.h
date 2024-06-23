#ifndef BASEELEMENT1_H
#define BASEELEMENT1_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;
class Constrain
{
    public:
    struct Equation
    {
        int id;        
        vector<int> node_ids;
        vector<string> dofs;
        vector<double> factors; 
        double rhs;
    };
    vector<Equation> Equations;
};

#endif