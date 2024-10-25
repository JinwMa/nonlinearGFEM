#ifndef LOADMANGER_H
#define LOADMANGER_H


#include <Eigen/Sparse>
#include <Eigen/Dense>

#include "BaseLoad.h"
#include "RotateBodyForce.h"
class LoadManger
{
    public:
    LoadManger(){};
    ~LoadManger(){};
    void takeDB(Input * pinput, Mesh * mesh, Dof_Map * pdofmap);

    void buildLoadForce(Input * pinput, Mesh * mesh, Dof_Map * pdofmap);

    private:
    std::vector<std::string> d_load_list;

};

#endif