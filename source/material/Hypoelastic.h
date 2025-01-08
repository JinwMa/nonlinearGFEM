#ifndef HYPOELASTIC_H
#define HYPOELASTIC_H


#include "BaseMaterial.h"

class Hypoelastic : public BaseMaterial
{
    public:
    Hypoelastic(){};
    ~Hypoelastic(){};
    void takeDB(Input * pinput, std::string & name);

    void getDSDu(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) override;
    void updateStress(ObjectElementData & element_data) override;
    void updateStressForBbarElement(ObjectElementData & element_data) override;
    void getDSDuForBbarElement(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) override;


};













#endif