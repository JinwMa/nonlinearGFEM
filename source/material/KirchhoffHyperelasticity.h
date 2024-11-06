#ifndef KIRCHHOFFHYPERELASTICITY_H
#define KIRCHHOFFHYPERELASTICITY_H

#include "BaseMaterial.h"

class KirchhoffHyperelasticity : public BaseMaterial
{
    public:
    KirchhoffHyperelasticity(){};
    ~KirchhoffHyperelasticity(){};
    void takeDB(Input * pinput, std::string & name);
};

#endif
