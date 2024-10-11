#ifndef BASECONSTRAINT_H
#define BASECONSTRAINT_H

#include<iostream>
#include<string>
#include<vector>

#include"input.h"
#include"mesh.h"

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
    string mater_node_dof = "null";
    vector<EquationTerm> equationterms;
    double rhs = 0.0;
};

class BaseConstraint
{
    public:
    BaseConstraint(){};
    virtual ~BaseConstraint(){};
    virtual void takeDB(Input * pinput, Mesh * pmesh){};
    virtual void buildDofMap(){};
    virtual vector<ConstraintEquation> buildEquations(Mesh * pmesh) {};

};


#endif