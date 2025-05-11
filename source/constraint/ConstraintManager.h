#ifndef CONSTRAINTMANAGER_H
#define CONSTRAINTMANAGER_H



#include<string>
#include<iostream>
#include<vector>

#include"input.h"
#include"mesh.h"
#include "ConstraintInterface.h"

#include"BaseConstraint.h"
#include"DofMap.h"
#include<Eigen/Sparse>
using namespace std;



class ConstraintManager
{
    public:    
    ConstraintManager(shared_ptr<DataBase> db, shared_ptr<Mesh> mesh, shared_ptr<DofMap> dofmap){
        d_mesh = mesh;
        d_db = db;
        d_dof_map = dofmap;
        init();
    };
    ~ConstraintManager(){};
    // 不要把takeDB接口直接暴露给外部
    void init()
    {
        takeDB();
    }
    void takeDB();
    void buildNodeDofs();
    void buildConstrintEquation();

    void buildDofMap();



    void assembleStiffness(){};
    void assembleInternalFoce(){};
    void assembleGMatrix(){};
    void assembleGVector(){};

    //
    vector<ConstraintEquation> d_CEs_vec;
    vector<shared_ptr<BaseConstraint>> d_constraints;
    shared_ptr<set<int>> d_MasterSetPointer;
    shared_ptr<set<int>> d_SlaveSetPointer;

    //
    shared_ptr<Mesh> d_mesh;
    shared_ptr<DataBase> d_db; 
    shared_ptr<DofMap> d_dof_map;

    void printConstraintEquations();
};


#endif