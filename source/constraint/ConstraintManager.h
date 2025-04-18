#ifndef CONSTRAINTMANAGER_H
#define CONSTRAINTMANAGER_H



#include<string>
#include<iostream>
#include<vector>

#include"input.h"
#include"mesh.h"
#include"BaseConstraint.h"
#include"DofMap.h"
#include<Eigen/Sparse>
using namespace std;



class ConstraintManager
{
    public:    
    ConstraintManager(){};
    ~ConstraintManager(){};
    void takeDB(shared_ptr<DataBase> db);
    vector<ConstraintEquation> buildConstrintEquation();

    unordered_set<int> all_m_set;
    // unordered_set<int> d_m_set_dofs;
};


#endif