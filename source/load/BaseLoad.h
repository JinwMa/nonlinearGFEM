#ifndef BASELOAD_H
#define BASELOAD_H

#include <string>
#include <iostream>
#include <vector>

#include "dof_map.h"
#include "input.h"
#include "mesh.h"
#include "toolbox.h"

class BaseLoad
{
    public:
    BaseLoad(){};
    virtual ~BaseLoad(){};
    virtual void takeDB(Input * pinput, Mesh * pmesh, std::string name = "") = 0;
    virtual void buildLoadVector(std::vector<int> & dofs_index, std::vector<double> & loadvector) = 0;
    virtual void buildLoadStiffnessMatrix(std::vector<int> & dofs_index, std::vector<double> & loadstiffnessmatrix) = 0;
    private:

};



#endif