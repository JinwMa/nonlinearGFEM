#ifndef BASECONSTRAINT_H
#define BASECONSTRAINT_H

#include<iostream>
#include<string>
#include<vector>

using namespace std;



struct EquationTerm
{
    double factor = 0.0;
    int node_id = 0;
    string node_dof;
};

struct ConstraintEquation
{
    int master_node_id = 0;
    string mater_node_dof;
    vector<EquationTerm> equationterms;
};


#endif