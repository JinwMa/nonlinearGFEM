#ifndef LINEARELASTICITY_H
#define LINEARELASTICITY_H


#include "BaseMaterial.h"

class LinearElasticity : public BaseMaterial
{
    public:
    LinearElasticity(){};
    ~LinearElasticity(){};
    void takeDB(Input * pinput, std::string & name);
    private:
    double E = 0.0;
    double u = 0.0;
};



#endif