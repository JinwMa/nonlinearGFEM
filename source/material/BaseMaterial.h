#ifndef BASEMATERIAL_H
#define BASEMATERIAL_H


#include<iostream>
#include<string>
#include<vector>
#include<unordered_set>

#include"input.h"
#include"mesh.h"
#include"toolbox.h"
#include "ObjectElementData.h"

class BaseMaterial
{
    public:
    BaseMaterial(){};
    virtual ~BaseMaterial(){};
    virtual void takeDB(Input * pinput, std::string &) = 0;
    void getC_e_tensor(double C_e_tensor[3][3][3][3]);
    void transeCtoD(const double C[3][3][3][3], double D[6][6]);

    virtual void getCt(const double Ce[3][3][3][3], const double F[3][3], const double jkb, double Ct[3][3][3][3]){};

    virtual void getStress(const double C[3][3][3][3], const double F[3][3], const double jkb, double stress[3][3]){};

    virtual void updateCt(ObjectElementData & element_data, const int ip_order, double Ct[3][3][3][3]){};

    virtual void updateStress(ObjectElementData & element_data){};

    virtual void getDSDu(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) {};

    virtual void updateStressForBbarElement(ObjectElementData & element_data){};

    virtual void getDSDuForBbarElement(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) {};


    virtual void updateStressAndDSDu(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) {};
    virtual void updateStressAndDSDuForBbarElement(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) {};


public:
    double E = 0.0;
    double u = 0.0;

    protected:
    double d_C_e_tensor[3][3][3][3];

    public:
    double d_init_kappa = 0.0;
};




#endif