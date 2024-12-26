#include "KirchhoffHyperelasticity.h"

void KirchhoffHyperelasticity::takeDB(Input *pinput, std::string &name)
{
    E = pinput->getDouble(name + "_modulus");
    u = pinput->getDouble(name + "_radio");
    getC_e_tensor(d_C_e_tensor);
}

void KirchhoffHyperelasticity::transe_C_SE_to_Ct(const double C_e_tensor[3][3][3][3],
                                                 const double F[3][3], const double jkb,
                                                 double Ct[3][3][3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    Ct[i][j][k][l] = 0.0;
                }
            }
        }
    }
    for (int ii = 0; ii < 3; ii++)
        for (int jj = 0; jj < 3; jj++)
            for (int kk = 0; kk < 3; kk++)
                for (int ll = 0; ll < 3; ll++)
                    for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                            for (int k = 0; k < 3; k++)
                                for (int l = 0; l < 3; l++)
                                {
                                    Ct[ii][jj][kk][ll] += (C_e_tensor[i][j][k][l] * F[ii][i] * F[jj][j] * F[kk][k] * F[ll][l] / jkb);
                                }
}




void KirchhoffHyperelasticity::getStress(const double C[3][3][3][3], const double F[3][3], const double jkb, double stress[3][3])
{
    double PK2[3][3] = {0.0};
    getPK2(C, F, PK2);
    double tc[3][3] = {0.0};
    toolbox::AmnXBpq(&F[0][0], 3, 3, &PK2[0][0], 3, 3, &tc[0][0]);
    double Ft[3][3] = {0.0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Ft[i][j] = F[j][i];
        }
    }
    toolbox::AmnXBpq(&tc[0][0], 3, 3, &Ft[0][0], 3, 3, &stress[0][0]);   
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            stress[i][j] = stress[i][j] / jkb;
        }
    }

}


void KirchhoffHyperelasticity::getPK2(const double C[3][3][3][3], const double F[3][3], double PK2[3][3])
{
    double Ft[3][3] = {0.0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Ft[i][j] = F[j][i];
        }
    }
    double I[3][3] = {0.0};
    for (int i = 0; i < 3; i++) I[i][i] = 1.0;
    double E[3][3] = {0.0};
    toolbox::AmnXBpq(&Ft[0][0], 3, 3, &F[0][0], 3, 3, &E[0][0]);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            E[i][j] = 0.5 * (E[i][j] - I[i][j]);
        }
    }
    toolbox::Tensor4dotdotTensor2(C, E, PK2);
}

void KirchhoffHyperelasticity::getCt(const double Ce[3][3][3][3], const double F[3][3], const double jkb, double Ct[3][3][3][3])
{
    transe_C_SE_to_Ct(Ce, F, jkb, Ct);
}

void KirchhoffHyperelasticity::updateCt(ObjectElementData & element_data,
                                                              const int ip_order,
                                                              double Ct[3][3][3][3])
{    
    auto & F_n1 = element_data.F_n1[ip_order];
    double F[3][3] = {0};
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        F[i][j] = F_n1[i][j];

    double jkb = element_data.jkb_n1[ip_order];

    getCt(d_C_e_tensor, F, jkb, Ct);
}


void KirchhoffHyperelasticity::updateStress(ObjectElementData & element_data)
{
    int ip_num = element_data.num_integration_points;
    for (int ip_order = 0; ip_order < ip_num; ip_order++)
    {
        auto &stress_n1 = element_data.stress_n1[ip_order];
        auto &F_n1 = element_data.F_n1[ip_order];
        double F[3][3] = {0};
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                F[i][j] = F_n1[i][j];

        double jkb = element_data.jkb_n1[ip_order];

        double stress[3][3] = {0};

        getStress(d_C_e_tensor, F, jkb, stress);

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                stress_n1[i][j] = stress[i][j];
    }    
}

void KirchhoffHyperelasticity::getDSDu(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du)
{
    int num_Gp = element_data.num_integration_points;
    int num_node = element_data.num_nodes;
    int size = num_Gp * num_node;
    dS_du.resize(size);
    for (int i = 0; i < size; i++) dS_du[i].resize(27);

    double Ct[3][3][3][3] = {0.0};         //切线模量
    double dNK_dx[3] = {0.0};
    
    double delt[3][3] = {0.0};   
    for (int i = 0; i < 3; i++)
    {
        delt[i][i] = 1.0;
    }

    for (int i = 0; i < num_Gp; i++)
    {
        auto stress = element_data.stress_n1[i];
        updateCt(element_data, i, Ct);
        for (int j = 0; j < num_node; j++)
        {
            double sfdx = element_data.sfdxyz[i][j][0];
            double sfdy = element_data.sfdxyz[i][j][1];
            double sfdz = element_data.sfdxyz[i][j][2];
            double sf_dxnow = element_data.Finv_n1[i][0][0] * sfdx +
                              element_data.Finv_n1[i][1][0] * sfdy +
                              element_data.Finv_n1[i][2][0] * sfdz;

            double sf_dynow = element_data.Finv_n1[i][0][1] * sfdx +
                              element_data.Finv_n1[i][1][1] * sfdy +
                              element_data.Finv_n1[i][2][1] * sfdz;

            double sf_dznow = element_data.Finv_n1[i][0][2] * sfdx +
                              element_data.Finv_n1[i][1][2] * sfdy +
                              element_data.Finv_n1[i][2][2] * sfdz;        

            dNK_dx[0] = sf_dxnow;
            dNK_dx[1] = sf_dynow;
            dNK_dx[2] = sf_dznow;    
            auto & dS_duI = dS_du[i * num_node + j];
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)                    
                    {
                        for (int ll = 0; ll < 3; ll++)
                        {
                            dS_duI[ii * 9 + jj * 3 + kk] += (Ct[ii][jj][kk][ll] * dNK_dx[ll] -
                                                            stress[ii][jj] * dNK_dx[ll] * delt[ll][kk] + 
                                                            dNK_dx[ll] * stress[ll][jj] * delt[ii][kk] + 
                                                            stress[ii][ll] * dNK_dx[ll] * delt[jj][kk]);
                        }
                    }
                }
            }
        }
    }
}