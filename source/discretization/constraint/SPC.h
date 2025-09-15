#ifndef SPC_H
#define SPC_H

#include "BaseConstraint.h"
class SPC : public BaseConstraint
{
    public:
    using BaseClass = BaseConstraint;
    
    SPC(shared_ptr<DataBase> db, shared_ptr<Mesh> mesh, shared_ptr<DofMap> dofmap) : 
      BaseClass::BaseConstraint(db, mesh, dofmap){};
    virtual ~SPC(){};

    void takeDB();
    string type(){return "SPC";}
    void buildNodeDofs();
    void buildConstraintEquations(vector<ConstraintEquation> &CEs,
                                  shared_ptr<set<int>> SlaveSet,
                                  shared_ptr<set<int>> MasterSet);

    private: // 元件自己维护
    vector<int> d_nodes;
    vector<string> d_dofs;
    vector<vector<double>> d_values; // 两层数组,第一层对应自由度,第二层对应节点
};

#endif