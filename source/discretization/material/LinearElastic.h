#ifndef LINEARELASTIC_H
#define LINEARELASTIC_H

#include"BaseMaterial.h"

class LinearElastic : public BaseMaterial
{
    public:
    LinearElastic(){};
    virtual ~LinearElastic(){};

    void getD(double D[6][6]){};

};


#endif