#ifndef HYPOELASTOPLASTIC_H
#define HYPOELASTOPLASTIC_H


#include "BaseMaterial.h"

class Hypoelastoplastic : public BaseMaterial
{
    public:
    Hypoelastoplastic(){};
    ~Hypoelastoplastic(){};
    void takeDB(Input * pinput, std::string & name);

    void getDSDu(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) override;
    void updateStress(ObjectElementData & element_data) override;
    void updateStressForBbarElement(ObjectElementData & element_data) override;
    void getDSDuForBbarElement(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) override;

    void updateStressAndDSDu(ObjectElementData &element_data, std::vector<std::vector<double>> &dS_du) override;
    void updateStressAndDSDuForBbarElement(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du) override;


};













#endif