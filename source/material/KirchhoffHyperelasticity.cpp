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

void KirchhoffHyperelasticity::getDt(const double F[3][3], const double jkb, double Dt[6][6])
{
    double C_SE[3][3][3][3] = {0.0};
    double Ct[3][3][3][3] = {0.0};
    getC_e_tensor(C_SE);
    transe_C_SE_to_Ct(C_SE, F, jkb, Ct);
    transeCtoD(Ct, Dt);
}

void KirchhoffHyperelasticity::getDt(std::array<std::array<double, 3>, 3> &F, const double jkb, double Dt[6][6])
{
    double C_SE[3][3][3][3] = {0.0};
    double Ct[3][3][3][3] = {0.0};
    getC_e_tensor(C_SE);
    transe_C_SE_to_Ct(C_SE, reinterpret_cast<double(*)[3]>(F.data()), jkb, Ct);
    transeCtoD(Ct, Dt);
}