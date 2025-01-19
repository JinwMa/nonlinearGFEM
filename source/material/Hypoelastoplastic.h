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

    private:
    void getQetc(ObjectElementData & element_data, const int ip_num,
                 double du_dX[3][3],
                 double Fm[3][3],
                 double Fm_inv[3][3],
                 double Im[3][3],
                 double Wm[3][3],
                 double Dm[3][3],
                 double Q[3][3],
                 double R1_inv[3][3],
                 double R2[3][3]);

    void getDQetc(ObjectElementData & element_data, const int ip_num, const int node_num,
                  const double Fm_inv[3][3],
                  const double Im[3][3],
                  const double R1_inv[3][3],
                  const double Q[3][3],
                  double dIm_du_K[3][3][3],
                  double ddm_du_K[3][3][3],
                  double dW_du_K[3][3][3],
                  double dQ_du_K[3][3][3],
                  double dNK_dxm[3]);



    void preBbar(ObjectElementData & element_data,
                 double & e_dil_Bbar,
                 double Fm_Bbar[3][3],
                 double Fm_Bbar_inv[3][3]);


    void get_centroid_de_du(ObjectElementData & element_data,
                            const double e_dil_Bbar,
                            const double Fm_Bbar[3][3],
                            const double Fm_Bbar_inv[3][3],
                            std::vector<std::vector<double>> & centroid_de_du);


    void get_e_dil(ObjectElementData & element_data,
                   const int ip_num,
                   const double Fm_inv[3][3],
                   double & e_dil);

    void update_ddm_du_K_ForBbarElement(ObjectElementData &element_data,
                                        const int ip_num,
                                        const int node_num,
                                        const double Fm_inv[3][3],
                                        const double dNK_dxm[3],
                                        const std::vector<std::vector<double>> & centroid_de_du,
                                        double ddm_du_K[3][3][3]);


};













#endif