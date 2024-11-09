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
    void getC_e_tensor(double C_e_tensor[3][3][3][3]);
    void transeCtoD(const double C[3][3][3][3], double D[6][6]);

    //对外接口
    virtual void getDt(const double F[3][3], const double jkb, double Dt[6][6]) {};
    virtual void getDt(std::array<std::array<double, 3>, 3> &F, const double jkb, double Dt[6][6]) {};


public:
    double E = 0.0;
    double u = 0.0;
};




#endif