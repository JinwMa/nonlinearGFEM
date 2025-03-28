#ifndef BASECONSTRAINT_H
#define BASECONSTRAINT_H

#include<iostream>
#include<string>
#include<vector>
#include<unordered_set>
#include<Eigen/Dense>
#include<Eigen/Sparse>
#include"input.h"
#include"mesh.h"
#include"toolbox.h"


using namespace std;

struct ConstraintEquation
{
    vector<int> node_local_ids;
    vector<string> dof_list;
    vector<double> factors;
    double rhs = 0.0;
};

class BaseConstraint
{
    public:
    BaseConstraint(){};
    virtual ~BaseConstraint(){};
    virtual void takeDB(shared_ptr<DataBase> db) = 0;
    virtual void buildDofMap() = 0;
    virtual void buildConstraintEquations(shared_ptr<Mesh> mesh, vector<ConstraintEquation> & CEs) = 0;
    virtual void buildConstraintMatrix(vector<Eigen::Triplet<double>> & ijValue);


    bool if_dof_in_mset(const int node_id, const string dof);
    bool if_dof_in_mset(const int node_dof_index);
    void addToMset(const int node_id, const string dof);
    void addToMset(const int node_dof_index);

    public:
    unordered_set<int> m_set;

};


#endif