#ifndef SPC_H
#define SPC_H


#include<iostream>
#include<string>
#include<vector>
#include"ConstraintManager.h"
#include"BaseConstraint.h"
#include"input.h"
#include"mesh.h"
using namespace std;

class SPC
{
    string name;
    public:    
    SPC(string SPC_name)
    {
        name = SPC_name;
    };
    ~SPC(){};
    void takeDB(Input & input, Mesh & mesh);
    void buildDofMap(){};
    vector<ConstraintEquation> buildEquations(){};

    private:
    vector<int> d_node_ids;
};




#endif