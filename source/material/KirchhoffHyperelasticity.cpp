#include "KirchhoffHyperelasticity.h"

void KirchhoffHyperelasticity::takeDB(Input *pinput, std::string &name)
{
    E = pinput->getDouble(name + "_modulus");
    u = pinput->getDouble(name + "_radio");
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
                                    Ct[ii][jj][kk][ll] += (C_e_tensor[ii][jj][kk][ll] * F[ii][i] * F[jj][j] * F[kk][k] * F[ll][l] / jkb);
                                }

//    for (int i = 0; i < 3; i++)
//     {
//         for (int j = 0; j < 3; j++)
//         {
//             for (int k = 0; k < 3; k++)
//             {
//                 for (int l = 0; l < 3; l++)
//                 {
//                     std::cout << Ct[i][j][k][l] << std::endl;
//                 }
//             }
//         }
//     }
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