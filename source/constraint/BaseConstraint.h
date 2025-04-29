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
#include"DofMap.h"


using namespace std;


// constraintEquation : const = c1 * dof1 + c2 * dof2 + c3 * dof3 ...
struct ConstraintEquation
{
    int equation_id;
    int slave_node_id;
    string slave_dof;
    double slave_factor;
    vector<int> master_node_ids;
    vector<string> master_dofs;
    vector<double> master_factors;
    double constant = 0.0;
};

class BaseConstraint
{
    public:
    // BaseConstraint();
    BaseConstraint(shared_ptr<DataBase> db, shared_ptr<Mesh> mesh, shared_ptr<DofMap> dofmap){
        d_db = db;
        d_mesh = mesh;
        d_dof_map = dofmap;
    };
    virtual ~BaseConstraint(){};
    virtual void takeDB() {};
    virtual string type() {return "unkown";}
    virtual void buildNodeDofs(){}; // 目前只会产生新的自由度标签
    virtual void buildConstraintEquations(vector<ConstraintEquation> & CEs,
                                          shared_ptr<set<int>> SlaveSet,
                                          shared_ptr<set<int>> MasterSet) {};
    
    void addDofToDofSet(const int node_id, const string dof, shared_ptr<set<int>> DofSet);

    bool checkIfdofTouchedMasterOrSlave(const int nodeId, const string dof, shared_ptr<set<int>>);

    public:
    shared_ptr<Mesh> d_mesh;
    shared_ptr<DataBase> d_db;
    shared_ptr<DofMap> d_dof_map;
};


#endif