
#include "Hypoelastic.h"


void Hypoelastic::takeDB(Input * pinput, std::string & name)
{
    E = pinput->getDouble(name + "_modulus");
    u = pinput->getDouble(name + "_radio");
    getC_e_tensor(d_C_e_tensor);
}


void Hypoelastic::updateStress(ObjectElementData & element_data)
{
    int num_GP = element_data.num_integration_points;
    int num_node = element_data.num_nodes;
    double delt[3][3] = {0.0};
    for (int ii = 0; ii < 3; ii++) delt[ii][ii] = 1.0;
    for (int i = 0; i < num_GP; i++) // 循环积分点
    {
        auto & stress_n = element_data.stress_n[i];     //平衡构型下的应力
        auto & stress_n1  = element_data.stress_n1[i];  //待更新的应力
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                stress_n1[ii][jj] = 0.0;
            }
        }
        auto & Fn = element_data.F_n[i];
        double dxm_dX[3][3] = {0.0};
        double du_dX[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                dxm_dX[ii][jj] = Fn[ii][jj];
                for (int kk = 0; kk < num_node; kk++)
                {
                    auto du_K = element_data.du[kk];
                    auto dNK_dX = element_data.sfdxyz[i][kk];
                    dxm_dX[ii][jj] += (0.5 * du_K[ii] * dNK_dX[jj]);
                    du_dX[ii][jj] += du_K[ii] * dNK_dX[jj];
                }
            }
        }

        double dxm_dX_inv[3][3];
        toolbox::invertMatrix(dxm_dX, dxm_dX_inv);
        double Im[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Im[ii][jj] += du_dX[ii][kk] * dxm_dX_inv[kk][jj];
                }
            }
        } 
        double R1[3][3] = {0.0};
        double R2[3][3] = {0.0};
        double W[3][3] = {0.0};
        double dm[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                W[ii][jj] = 0.5 * (Im[ii][jj] - Im[jj][ii]);
                R1[ii][jj] = delt[ii][jj] - 0.5 * W[ii][jj];
                R2[ii][jj] = delt[ii][jj] + 0.5 * W[ii][jj];
                dm[ii][jj] = 0.5 * (Im[ii][jj] + Im[jj][ii]);
            }
        }
        double R1_inv[3][3] = {0.0};
        toolbox::invertMatrix(R1, R1_inv);
        double Q[3][3] = {0.0};
        double QT[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Q[ii][jj] += R1_inv[ii][kk] * R2[kk][jj];
                }
            }
        } 
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                QT[ii][jj] = Q[jj][ii];
            }
        }

        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    for (int ll = 0; ll < 3; ll++)
                    {
                        stress_n1[ii][jj] += (Q[ii][kk] * stress_n[kk][ll] * QT[ll][jj] +
                                              d_C_e_tensor[ii][jj][kk][ll] * dm[kk][ll]);                                           
                    }
                }
            }
        } 
    }
}

void Hypoelastic::getDSDu(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du)
{
    int num_GP = element_data.num_integration_points;
    int num_node = element_data.num_nodes;
    int size = num_GP * num_node;
    dS_du.resize(size);
    for (int i = 0; i < size; i++) dS_du[i].resize(27);

    double delt[3][3] = {0.0};
    for (int ii = 0; ii < 3; ii++) delt[ii][ii] = 1.0;
    for (int i = 0; i < num_GP; i++) // 循环积分点
    {
        auto stress = element_data.stress_n[i];
        auto & Fn = element_data.F_n[i];
        double dxm_dX[3][3] = {0.0};
        double du_dX[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                dxm_dX[ii][jj] = Fn[ii][jj];
                for (int kk = 0; kk < num_node; kk++)
                {
                    auto du_K = element_data.du[kk];
                    auto dNK_dX = element_data.sfdxyz[i][kk];
                    dxm_dX[ii][jj] += (0.5 * du_K[ii] * dNK_dX[jj]);
                    du_dX[ii][jj] += du_K[ii] * dNK_dX[jj];
                }
            }
        }
        double dxm_dX_inv[3][3];
        toolbox::invertMatrix(dxm_dX, dxm_dX_inv);
        double Im[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Im[ii][jj] += du_dX[ii][kk] * dxm_dX_inv[kk][jj];
                }
            }
        } 
        double R1[3][3] = {0.0};
        double R2[3][3] = {0.0};
        double W[3][3] = {0.0};
        double dm[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                W[ii][jj] = 0.5 * (Im[ii][jj] - Im[jj][ii]);
                R1[ii][jj] = delt[ii][jj] - 0.5 * W[ii][jj];
                R2[ii][jj] = delt[ii][jj] + 0.5 * W[ii][jj];
                dm[ii][jj] = 0.5 * (Im[ii][jj] + Im[jj][ii]);
            }
        }
        double R1_inv[3][3] = {0.0};
        toolbox::invertMatrix(R1, R1_inv);
        double Q[3][3] = {0.0};
        double QT[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Q[ii][jj] += R1_inv[ii][kk] * R2[kk][jj];
                }
            }
        }        
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                QT[ii][jj] = Q[jj][ii];
            }
        }

        for (int K = 0; K < num_node; K++)
        {
            auto & dS_du_K = dS_du[i * num_node + K];
            for (int iii = 0; iii < dS_du_K.size(); iii++) dS_du_K[iii] = 0.0; 
            auto dNK_dX = element_data.sfdxyz[i][K];
            double dNK_dxm[3] = {0.0};
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    dNK_dxm[jj] += dNK_dX[ii] * dxm_dX_inv[ii][jj]; 
                }
            }
            double dIm_du_K[3][3][3] = {0.0};
            double ddm_du_K[3][3][3] = {0.0};
            double dW_du_K[3][3][3] = {0.0};
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        dIm_du_K[ii][jj][kk] = dNK_dxm[jj] * delt[ii][kk] - 
                                               0.5 * Im[ii][kk] * dNK_dxm[jj];
                    }
                }
            }
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        ddm_du_K[ii][jj][kk] = 0.5 * (dIm_du_K[ii][jj][kk] + dIm_du_K[jj][ii][kk]);
                        dW_du_K[ii][jj][kk] = 0.5 * (dIm_du_K[ii][jj][kk] - dIm_du_K[jj][ii][kk]);
                    }
                }
            }
            double dQ_du_k[3][3][3] = {0.0};
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            for (int q = 0; q < 3; q++)
                            {
                                dQ_du_k[ii][jj][kk] += (0.5 * R1_inv[ii][p] * dW_du_K[p][q][kk] * Q[q][jj] +
                                                        0.5 * R1_inv[ii][p] * dW_du_K[p][jj][kk]);
                            }
                        }
                    }
                }
            }

            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            for (int q = 0; q < 3; q++)
                            {
                                dS_du_K[ii * 9 + jj * 3 + kk] += (dQ_du_k[ii][p][kk] * stress[p][q] * Q[jj][q] + 
                                                                  Q[ii][p] * stress[p][q] * dQ_du_k[jj][q][kk] + 
                                                                  d_C_e_tensor[ii][jj][p][q] * ddm_du_K[p][q][kk]);
                            }
                        }                        
                    }
                }
            }
        }
    }
}