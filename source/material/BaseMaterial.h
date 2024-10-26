#ifndef BASEMATERIAL_H
#define BASEMATERIAL_H


#include<iostream>
#include<string>
#include<vector>
#include<unordered_set>

#include"input.h"
#include"mesh.h"
#include"toolbox.h"

class BaseMaterial
{
    public:
    BaseMaterial(){};
    virtual ~BaseMaterial(){};
    virtual void takeDB(Input * pinput, std::string &) = 0;
};




#endif