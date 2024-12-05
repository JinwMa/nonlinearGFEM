#ifndef KIRCHHOFFHYPERELASTICITY_H
#define KIRCHHOFFHYPERELASTICITY_H

#include "BaseMaterial.h"

class KirchhoffHyperelasticity : public BaseMaterial
{
    public:
    KirchhoffHyperelasticity(){};
    ~KirchhoffHyperelasticity(){};
    void takeDB(Input * pinput, std::string & name); 

    void getCt(const double Ce[3][3][3][3], const double F[3][3], const double jkb, double Ct[3][3][3][3]);    
    void getStress(const double C[3][3][3][3], const double F[3][3], const double jkb, double stress[3][3]) override;
    void updateStressOnIntegrationPoint(ObjectElementData & element_data, const int ip_order, double Ct[3][3][3][3]) override;

private:

    void transe_C_SE_to_Ct(const double C_SE[3][3][3][3],
                           const double F[3][3],
                           const double jkb,
                           double Ct[3][3][3][3]);

    void getPK2(const double C[3][3][3][3], const double F[3][3], double PK2[3][3]);
};

#endif
