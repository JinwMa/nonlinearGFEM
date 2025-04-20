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
    ConstraintManager(shared_ptr<DataBase> db, shared_ptr<Mesh> mesh, shared_ptr<DofMap> dofmap){
        d_mesh = mesh;
        d_db = db;
        d_dof_map = dofmap;
    };
    ~ConstraintManager(){};
    // 不要把takeDB接口直接暴露给外部
    void init()
    {
        takeDB();
    }
    void takeDB();
    void buildDofs();
    void buildConstrintEquation();
    void assembleStiffness(){};
    void assembleInternalFoce(){};
    void assembleGMatrix(){};
    void assembleGVector(){};

    //
    vector<ConstraintEquation> d_CEs_vec;
    vector<shared_ptr<BaseConstraint>> d_constraints;
    vector<int> d_all_slave_dofs;
    vector<int> d_all_master_dofs;

    //
    shared_ptr<Mesh> d_mesh;
    shared_ptr<DataBase> d_db; 
    shared_ptr<DofMap> d_dof_map;
};


#endif