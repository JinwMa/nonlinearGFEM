#ifndef BASEMATERIAL_H
#define BASEMATERIAL_H

#include "ObjectElementData.h"
#include "input.h"
#include "mesh.h"

class BaseMaterial
{
    public:
    BaseMaterial(){};
    virtual ~BaseMaterial(){};
    virtual void takeDB(std::shared_ptr<DataBase> db) final;
    virtual void getD(double D[6][6]);
    virtual void getStressAndDsDu(ElementData & element_data); 

    //
    public:
    double d_E;
    double d_v;
    double d_lambda;
    double d_mu;
};




#endif