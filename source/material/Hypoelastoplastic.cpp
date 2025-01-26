
#include "Hypoelastoplastic.h"


void Hypoelastoplastic::takeDB(Input * pinput, std::string & name)
{
    E = pinput->getDouble(name + "_modulus");
    u = pinput->getDouble(name + "_radio");
    d_init_kappa = pinput->getDouble(name + "_init_kappa");
    getC_e_tensor(d_C_e_tensor);
    if (pinput->ifExist(name + "_iner_loop_tol")) d_iner_loop_tol = pinput->getDouble(name + "_iner_loop_tol");
}

void Hypoelastoplastic::updateStress(ObjectElementData & element_data)
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
        // 1 形成Fm
        double Fm[3][3] = {0.0};
        double du_dX[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                Fm[ii][jj] = Fn[ii][jj];
                for (int kk = 0; kk < num_node; kk++)
                {
                    auto du_K = element_data.du[kk];
                    auto dNK_dX = element_data.sfdxyz[i][kk];
                    Fm[ii][jj] += (0.5 * du_K[ii] * dNK_dX[jj]);
                    du_dX[ii][jj] += du_K[ii] * dNK_dX[jj];
                }
            }
        }

        // 2形成Fm逆
        double Fm_inv[3][3];
        toolbox::invertMatrix(Fm, Fm_inv);
        // 3形成Im
        double Im[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Im[ii][jj] += du_dX[ii][kk] * Fm_inv[kk][jj];
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

void Hypoelastoplastic::getDSDu(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du)
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

        // 1形成Fm
        double Fm[3][3] = {0.0};
        double du_dX[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                Fm[ii][jj] = Fn[ii][jj];
                for (int kk = 0; kk < num_node; kk++)
                {
                    auto du_K = element_data.du[kk];
                    auto dNK_dX = element_data.sfdxyz[i][kk];
                    Fm[ii][jj] += (0.5 * du_K[ii] * dNK_dX[jj]);
                    du_dX[ii][jj] += du_K[ii] * dNK_dX[jj];
                }
            }
        }
        // 2形成Fm的逆
        double Fm_inv[3][3];
        toolbox::invertMatrix(Fm, Fm_inv);
        double Im[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Im[ii][jj] += du_dX[ii][kk] * Fm_inv[kk][jj];
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
        // 上述过程类似应力更新过程

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
                    dNK_dxm[jj] += dNK_dX[ii] * Fm_inv[ii][jj]; 
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
                                dQ_du_k[ii][jj][kk] += (0.5 * R1_inv[ii][p] * dW_du_K[p][q][kk] * Q[q][jj]);
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
                            dQ_du_k[ii][jj][kk] += (0.5 * R1_inv[ii][p] * dW_du_K[p][jj][kk]);
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




void Hypoelastoplastic::updateStressForBbarElement(ObjectElementData & element_data)
{
    int num_GP = element_data.num_integration_points;
    int num_node = element_data.num_nodes;
    double delt[3][3] = {0.0};

    //处理形心处应变
    double e_dil_Bbar = 0.0;
    auto centroid_Fn = element_data.centroid_F_n;

    // 形心处的变形梯度
    auto Fm_Bbar = centroid_Fn;
    for (int i = 0; i < num_node; i++)
    {
        auto du_I = element_data.du[i];
        auto dNI_dX = element_data.centroid_sfdxy[i];
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {                
                Fm_Bbar[ii][jj] += (0.5 * du_I[ii] * dNI_dX[jj]);
            }
        }
    }
    // 形心处变形梯度的逆
    double Fm_Bbar_inv[3][3] = {0.0};
    toolbox::invertMatrix(Fm_Bbar, Fm_Bbar_inv);


    // 形心处增量体应变
    for (int i = 0; i < num_node; i++)
    {
        auto centroid_dNI_dX = element_data.centroid_sfdxy[i];
        auto du_I = element_data.du[i];
        double centroid_dNI_dxm[3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                centroid_dNI_dxm[jj] += centroid_dNI_dX[ii] * Fm_Bbar_inv[ii][jj];
            }
        }
        for (int ii = 0; ii < 3; ii++)
        {
            e_dil_Bbar += du_I[ii] * centroid_dNI_dxm[ii];
        }
    }
    e_dil_Bbar = e_dil_Bbar / 3.0;


    // 标准积分点上的处理
    for (int ii = 0; ii < 3; ii++) delt[ii][ii] = 1.0;
    for (int i = 0; i < num_GP; i++) // 循环积分点
    {
        // #############################
        // #############################        
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
        double Fm[3][3] = {0.0};
        double du_dX[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                Fm[ii][jj] = Fn[ii][jj];
                for (int kk = 0; kk < num_node; kk++)
                {
                    auto du_K = element_data.du[kk];
                    auto dNK_dX = element_data.sfdxyz[i][kk];
                    Fm[ii][jj] += (0.5 * du_K[ii] * dNK_dX[jj]);
                    du_dX[ii][jj] += du_K[ii] * dNK_dX[jj];
                }
            }
        }
        double Fm_inv[3][3];
        toolbox::invertMatrix(Fm, Fm_inv);
        double Im[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Im[ii][jj] += du_dX[ii][kk] * Fm_inv[kk][jj];
                }
            }
        }

        double e_dil = 0.0;
        for (int ii = 0; ii < num_node; ii++)
        {
            auto du_K = element_data.du[ii];
            auto dNK_dX = element_data.sfdxyz[i][ii];
            double dNK_dxm[3] = {0.0};
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    dNK_dxm[kk] += dNK_dX[jj] * Fm_inv[jj][kk];
                }
            }

            for (int jj = 0; jj < 3; jj++)
            {
                e_dil += dNK_dxm[jj] * du_K[jj];            
            }
        } 
        e_dil = e_dil / 3.0;

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
                dm[ii][jj] = 0.5 * (Im[ii][jj] + Im[jj][ii]) -
                             e_dil * delt[ii][jj] + e_dil_Bbar * delt[ii][jj];
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





void Hypoelastoplastic::getDSDuForBbarElement(ObjectElementData & element_data, std::vector<std::vector<double>> & dS_du)
{
    double delt[3][3] = {0.0};
    for (int ii = 0; ii < 3; ii++) delt[ii][ii] = 1.0;
    int num_GP = element_data.num_integration_points;
    int num_node = element_data.num_nodes;
    int size = num_GP * num_node;
    dS_du.resize(size);
    for (int i = 0; i < size; i++) dS_du[i].resize(27);    

    std::vector<std::vector<double>> centroid_de_du;
    centroid_de_du.resize(num_node);
    for (int i = 0; i < num_node; i++) centroid_de_du[i].resize(27);

    auto centroid_Fn = element_data.centroid_F_n;
    auto Fm_Bbar = centroid_Fn;
    for (int i = 0; i < num_node; i++)
    {
        auto du_I = element_data.du[i];
        auto dNI_dX = element_data.centroid_sfdxy[i];
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {                
                Fm_Bbar[ii][jj] += (0.5 * du_I[ii] * dNI_dX[jj]);
            }
        }
    }
    double Fm_Bbar_inv[3][3] = {0.0};
    toolbox::invertMatrix(Fm_Bbar, Fm_Bbar_inv);

    double centroid_dNIs_dxm[num_node][3] = {0.0};
    for (int i = 0; i < num_node; i++)
    {
        auto centroid_dNI_dX = element_data.centroid_sfdxy[i];
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                centroid_dNIs_dxm[i][jj] += centroid_dNI_dX[ii] * Fm_Bbar_inv[ii][jj];
            }
        }
    }

    // 不用循环积分点，只需要循环节点即可
    for (int J = 0; J < num_node; J++)
    {        
        double T2[3][3][3] = {0.0};
        for (int K = 0; K < num_node; K++)
        {
            auto du_K = element_data.du[K];
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            T2[ii][jj][kk] += centroid_dNIs_dxm[K][kk] * centroid_dNIs_dxm[J][p] * du_K[p] * delt[ii][jj] * (1.0 / 3.0);
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
                    centroid_de_du[J][ii * 9 + jj * 3 + kk] = (1.0 / 3.0) * centroid_dNIs_dxm[J][kk] * delt[ii][jj] - T2[ii][jj][kk];
                }
            }
        }
    }


    for (int i = 0; i < num_GP; i++) // 循环积分点
    {
        auto stress = element_data.stress_n[i];
        auto & Fn = element_data.F_n[i];
        double Fm[3][3] = {0.0};
        double du_dX[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                Fm[ii][jj] = Fn[ii][jj];
                for (int kk = 0; kk < num_node; kk++)
                {
                    auto du_K = element_data.du[kk];
                    auto dNK_dX = element_data.sfdxyz[i][kk];
                    Fm[ii][jj] += (0.5 * du_K[ii] * dNK_dX[jj]);
                    du_dX[ii][jj] += du_K[ii] * dNK_dX[jj];
                }
            }
        }
        double Fm_inv[3][3];
        toolbox::invertMatrix(Fm, Fm_inv);
        double Im[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    Im[ii][jj] += du_dX[ii][kk] * Fm_inv[kk][jj];
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
                    dNK_dxm[jj] += dNK_dX[ii] * Fm_inv[ii][jj]; 
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

            double T1[3][3][3] = {0.0};
            for (int J = 0; J < num_node; J++)
            {
                auto du_J = element_data.du[J];
                auto dNJ_dX = element_data.sfdxyz[i][J];
                double dNJ_dxm[3] = {0.0};
                for (int ii = 0; ii < 3; ii++)
                {
                    for (int jj = 0; jj < 3; jj++)
                    {
                        dNJ_dxm[jj] += dNJ_dX[ii] * Fm_inv[ii][jj];
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
                                T1[ii][jj][kk] += dNJ_dxm[kk] * dNK_dxm[p] * du_J[p] * delt[ii][jj]; 
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
                        ddm_du_K[ii][jj][kk] = ddm_du_K[ii][jj][kk] - dNK_dxm[kk] * delt[ii][jj] * (1.0 / 3.0) + (1.0 / 3.0) * T1[ii][jj][kk]
                                               + centroid_de_du[K][ii * 9 + jj * 3 + kk];
                        // std::cout << - dNK_dxm[kk] * delt[ii][jj] * (1.0 / 3.0) + (1.0 / 3.0) * T1[ii][jj][kk] << "  " << centroid_de_du[K][ii * 9 + jj * 3 + kk] << std::endl;
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
                                dQ_du_k[ii][jj][kk] += (0.5 * R1_inv[ii][p] * dW_du_K[p][q][kk] * Q[q][jj]);
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
                            dQ_du_k[ii][jj][kk] += (0.5 * R1_inv[ii][p] * dW_du_K[p][jj][kk]);
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

void Hypoelastoplastic::updateStressAndDSDu(ObjectElementData &element_data, std::vector<std::vector<double>> &dS_du) 
{
    const double s23 = std::sqrt(2.0 / 3.0);
    const double mu = 0.5 * E / (u + 1.0);
    const double lambda = u * E / ((u + 1.0) * (1.0 - 2.0 * u));
    double delt[3][3] = {0.0};
    for (int ii = 0; ii < 3; ii++) delt[ii][ii] = 1.0;
    int num_GP = element_data.num_integration_points;
    int num_node = element_data.num_nodes;
    int size = num_GP * num_node;
    dS_du.resize(size);
    for (int i = 0; i < size; i++) dS_du[i].resize(27);  

    // 标准单元
    double du_dX[3][3] = {0.0};
    double Fm[3][3] = {0.0};
    double Fm_inv[3][3] = {0.0};
    double Im[3][3] = {0.0};
    double W[3][3] = {0.0};
    double dm[3][3] = {0.0};
    double Q[3][3] = {0.0};
    double R1_inv[3][3] = {0.0};
    double R2[3][3] = {0.0};

    // 
    double dIm_du_K[3][3][3];
    double ddm_du_K[3][3][3];
    double dW_du_K[3][3][3];
    double dQ_du_K[3][3][3];
    double dNK_dxm[3];

    for (int i = 0; i < num_GP; i++) // 循环积分点
    {
        const auto & stress = element_data.stress_n[i];
        auto & stress_n1  = element_data.stress_n1[i];  //待更新的应力

        // 弹塑性相关变量：
        const auto & epn = element_data.eff_p_strain_n[i];
        auto & epn1 = element_data.eff_p_strain_n1[i];
        auto & kappa_n1 = element_data.kappa_n1[i];
        double stress_trial[3][3] = {0.0}; // 试探应力
        double stress_trial_dev[3][3] = {0.0}; // 试探偏应力
        double stress_dev[3][3] = {0.0}; // 真实偏应力
        double trance_stress_trial = 0.0;  // 试探应力的迹
        double kappa_trial = element_data.kappa_n[i];
        double normal_stress_dev = 0.0;
        double direct_normal[3][3] = {0.0};
        double Cep[3][3][3][3] = {0.0};
        double deltaGama = 0.0;


        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                stress_n1[ii][jj] = 0.0;
            }
        }

        this->getQetc(element_data, i,
                      du_dX,
                      Fm,
                      Fm_inv,
                      Im,
                      W,
                      dm,
                      Q,
                      R1_inv,
                      R2);
        double QT[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                QT[ii][jj] = Q[jj][ii];
            }
        }

        // 获取试探应力
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    for (int ll = 0; ll < 3; ll++)
                    {
                        stress_trial[ii][jj] += (Q[ii][kk] * stress[kk][ll] * QT[ll][jj] +
                                              d_C_e_tensor[ii][jj][kk][ll] * dm[kk][ll]);
                    }
                }
            }
        }
        // 试探应力的迹
        trance_stress_trial = stress_trial[0][0] + stress_trial[1][1] + stress_trial[2][2];
        // 试探偏应力
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                stress_trial_dev[ii][jj] = stress_trial[ii][jj] - 
                                           trance_stress_trial * delt[ii][jj] * (1.0 / 3.0);
            }
        }
        // 获取偏应力的模
        toolbox::getTensorNormal(stress_trial_dev, normal_stress_dev);

        // 判断1：弹性阶段
        if (normal_stress_dev < s23 * kappa_trial)
        {
            //更新应力
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    stress_n1[ii][jj] = stress_trial_dev[ii][jj] + 
                                        trance_stress_trial * delt[ii][jj] * (1.0 / 3.0); 
                }
            }         
            //等效塑性应变
            epn1 = epn;
            // 屈服半径
            kappa_n1 = kappa_trial;       
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        for (int ll = 0; ll < 3; ll++)
                        {
                            Cep[ii][jj][kk][ll] = d_C_e_tensor[ii][jj][kk][ll];
                        }
                    }
                }
            }        
        }
        else
        {
            //获取单位normal张量
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    direct_normal[ii][jj] = stress_trial_dev[ii][jj] / normal_stress_dev;
                }
            }
            // 重中之重，计算deltaGamma
            this->getDeltaGamma(epn, normal_stress_dev, mu, d_init_kappa, deltaGama);
            // std::cout << deltaGama << std::endl;
            // 计算最终的偏应力状态
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    stress_n1[ii][jj] = stress_trial_dev[ii][jj] - 
                                        deltaGama * 2.0 * mu * direct_normal[ii][jj] + 
                                        trance_stress_trial * delt[ii][jj] * (1.0 / 3.0);
                }
            }
            epn1 = epn + s23 * deltaGama;
            kappa_n1 = kappa(d_init_kappa, epn1);
            this->getCep(lambda, mu, d_init_kappa, direct_normal, epn1, kappa_n1, normal_stress_dev, Cep);
        }

        for (int K = 0; K < num_node; K++)
        {
            auto & dS_du_K = dS_du[i * num_node + K];
            for (int iii = 0; iii < dS_du_K.size(); iii++) dS_du_K[iii] = 0.0; 

            this->getDQetc(element_data, i, K,
                           Fm_inv,
                           Im,
                           R1_inv,
                           Q,
                           dIm_du_K,
                           ddm_du_K,
                           dW_du_K,
                           dQ_du_K,
                           dNK_dxm);

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
                                dS_du_K[ii * 9 + jj * 3 + kk] += (dQ_du_K[ii][p][kk] * stress[p][q] * Q[jj][q] + 
                                                                  Q[ii][p] * stress[p][q] * dQ_du_K[jj][q][kk] + 
                                                                  Cep[ii][jj][p][q] * ddm_du_K[p][q][kk]);
                            }
                        }                        
                    }
                }
            }
        }
    } 
}





void Hypoelastoplastic::updateStressAndDSDuForBbarElement(ObjectElementData &element_data, std::vector<std::vector<double>> &dS_du) 
{
    const double s23 = std::sqrt(2.0 / 3.0);
    const double mu = 0.5 * E / (u + 1.0);
    const double lambda = u * E / ((u + 1.0) * (1.0 - 2.0 * u));
    double delt[3][3] = {0.0};
    for (int ii = 0; ii < 3; ii++) delt[ii][ii] = 1.0;
    int num_GP = element_data.num_integration_points;
    int num_node = element_data.num_nodes;
    int size = num_GP * num_node;
    dS_du.resize(size);
    for (int i = 0; i < size; i++) dS_du[i].resize(27);    

    std::vector<std::vector<double>> centroid_de_du;
    centroid_de_du.resize(num_node);
    for (int i = 0; i < num_node; i++) centroid_de_du[i].resize(27);  

    // 标准单元
    double du_dX[3][3] = {0.0};
    double Fm[3][3] = {0.0};
    double Fm_inv[3][3] = {0.0};
    double Im[3][3] = {0.0};
    double W[3][3] = {0.0};
    double dm[3][3] = {0.0};
    double Q[3][3] = {0.0};
    double R1_inv[3][3] = {0.0};
    double R2[3][3] = {0.0};

    // 
    double dIm_du_K[3][3][3];
    double ddm_du_K[3][3][3];
    double dW_du_K[3][3][3];
    double dQ_du_K[3][3][3];
    double dNK_dxm[3];

  
    
    double e_dil_Bbar = 0.0;
    double Fm_Bbar[3][3] = {0.0};
    double Fm_Bbar_inv[3][3] = {0.0};
    this->preBbar(element_data,
                  e_dil_Bbar,
                  Fm_Bbar,
                  Fm_Bbar_inv);

    this->get_centroid_de_du(element_data,
                             e_dil_Bbar,
                             Fm_Bbar,
                             Fm_Bbar_inv,
                             centroid_de_du);

    for (int i = 0; i < num_GP; i++) // 循环积分点
    {
        const auto & stress = element_data.stress_n[i];
        auto & stress_n1  = element_data.stress_n1[i];  //待更新的应力

        // 弹塑性相关变量：
        const auto & epn = element_data.eff_p_strain_n[i];
        auto & epn1 = element_data.eff_p_strain_n1[i];
        auto & kappa_n1 = element_data.kappa_n1[i];
        double stress_trial[3][3] = {0.0}; // 试探应力
        double stress_trial_dev[3][3] = {0.0}; // 试探偏应力
        double stress_dev[3][3] = {0.0}; // 真实偏应力
        double trance_stress_trial = 0.0;  // 试探应力的迹
        double kappa_trial = element_data.kappa_n[i];
        double normal_stress_dev = 0.0;
        double direct_normal[3][3] = {0.0};
        double Cep[3][3][3][3] = {0.0};
        double deltaGama = 0.0;


        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                stress_n1[ii][jj] = 0.0;
            }
        }

        this->getQetc(element_data, i,
                      du_dX,
                      Fm,
                      Fm_inv,
                      Im,
                      W,
                      dm,
                      Q,
                      R1_inv,
                      R2);

        double e_dil = 0.0;
        this->get_e_dil(element_data, i, Fm_inv, e_dil);

        //更新dm
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                dm[ii][jj] = dm[ii][jj] - e_dil * delt[ii][jj] + e_dil_Bbar * delt[ii][jj];
            }
        }
        double QT[3][3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                QT[ii][jj] = Q[jj][ii];
            }
        }

        // 获取试探应力
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    for (int ll = 0; ll < 3; ll++)
                    {
                        stress_trial[ii][jj] += (Q[ii][kk] * stress[kk][ll] * QT[ll][jj] +
                                              d_C_e_tensor[ii][jj][kk][ll] * dm[kk][ll]);
                    }
                }
            }
        }
        // 试探应力的迹
        trance_stress_trial = stress_trial[0][0] + stress_trial[1][1] + stress_trial[2][2];
        // 试探偏应力
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                stress_trial_dev[ii][jj] = stress_trial[ii][jj] - 
                                           trance_stress_trial * delt[ii][jj] * (1.0 / 3.0);
            }
        }
        // 获取偏应力的模
        toolbox::getTensorNormal(stress_trial_dev, normal_stress_dev);

        // 判断1：弹性阶段
        if (normal_stress_dev < s23 * kappa_trial)
        {
            //更新应力
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    stress_n1[ii][jj] = stress_trial_dev[ii][jj] + 
                                        trance_stress_trial * delt[ii][jj] * (1.0 / 3.0); 
                }
            }         
            //等效塑性应变
            epn1 = epn;
            // 屈服半径
            kappa_n1 = kappa_trial;       
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        for (int ll = 0; ll < 3; ll++)
                        {
                            Cep[ii][jj][kk][ll] = d_C_e_tensor[ii][jj][kk][ll];
                        }
                    }
                }
            }        
        }
        else
        {
            //获取单位normal张量
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    direct_normal[ii][jj] = stress_trial_dev[ii][jj] / normal_stress_dev;
                }
            }
            // 重中之重，计算deltaGamma
            this->getDeltaGamma(epn, normal_stress_dev, mu, d_init_kappa, deltaGama);
            // std::cout << deltaGama << std::endl;
            // 计算最终的偏应力状态
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    stress_n1[ii][jj] = stress_trial_dev[ii][jj] - 
                                        deltaGama * 2.0 * mu * direct_normal[ii][jj] + 
                                        trance_stress_trial * delt[ii][jj] * (1.0 / 3.0);
                }
            }
            epn1 = epn + s23 * deltaGama;
            kappa_n1 = kappa(d_init_kappa, epn1);
            this->getCep(lambda, mu, d_init_kappa, direct_normal, epn1, kappa_n1, normal_stress_dev, Cep);
        }

        for (int K = 0; K < num_node; K++)
        {
            auto & dS_du_K = dS_du[i * num_node + K];
            for (int iii = 0; iii < dS_du_K.size(); iii++) dS_du_K[iii] = 0.0; 

            this->getDQetc(element_data, i, K,
                           Fm_inv,
                           Im,
                           R1_inv,
                           Q,
                           dIm_du_K,
                           ddm_du_K,
                           dW_du_K,
                           dQ_du_K,
                           dNK_dxm);

            this->update_ddm_du_K_ForBbarElement(element_data,
                                                 i,
                                                 K,
                                                 Fm_inv,
                                                 dNK_dxm,
                                                 centroid_de_du,
                                                 ddm_du_K);

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
                                dS_du_K[ii * 9 + jj * 3 + kk] += (dQ_du_K[ii][p][kk] * stress[p][q] * Q[jj][q] + 
                                                                  Q[ii][p] * stress[p][q] * dQ_du_K[jj][q][kk] + 
                                                                  Cep[ii][jj][p][q] * ddm_du_K[p][q][kk]);
                            }
                        }                        
                    }
                }
            }
        }
    } 
}













void Hypoelastoplastic::getQetc(ObjectElementData & element_data, const int ip_num, 
                          double du_dX[3][3],
                          double Fm[3][3],
                          double Fm_inv[3][3],
                          double Im[3][3], 
                          double Wm[3][3], 
                          double Dm[3][3], 
                          double Q[3][3],
                          double R1_inv[3][3],
                          double R2[3][3])
{
    double delt[3][3] = {0.0};
    const auto & Fn = element_data.F_n[ip_num];
    int num_node = element_data.num_nodes;
    // 形成Fm 和 du_dX
    // 初始化各种变量
    for (int ii = 0; ii < 3; ii++)
    {
        delt[ii][ii] = 1.0;
        for (int jj = 0; jj < 3; jj++)
        {
            du_dX[ii][jj] = 0.0;
            Fm[ii][jj] = Fn[ii][jj]; 
            Fm_inv[ii][jj] = 0.0;
            Im[ii][jj] = 0.0;
            Wm[ii][jj] = 0.0;
            Dm[ii][jj] = 0.0;
            Q[ii][jj] = 0.0;
            R1_inv[ii][jj] = 0.0;
            R2[ii][jj] = 0.0;
        }
    }

    for (int K = 0; K < num_node; K++)
    {
        const auto & du_K = element_data.du[K];
        const auto & dNK_dX = element_data.sfdxyz[ip_num][K];
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                Fm[ii][jj] += 0.5 * du_K[ii] * dNK_dX[jj];
                du_dX[ii][jj] += du_K[ii] * dNK_dX[jj];
            }
        }
    }
    // 形成Fm的逆
    toolbox::invertMatrix(Fm, Fm_inv);
    //形成Im
    for (int ii = 0; ii < 3; ii++)
    {
        for (int jj = 0; jj < 3; jj++)
        {
            for (int kk = 0; kk < 3; kk++)
            {
                Im[ii][jj] += du_dX[ii][kk] * Fm_inv[kk][jj];
            }
        }
    }
    double R1[3][3] = {0.0};
    // 形成Wm 和 Dm
    for (int ii = 0; ii < 3; ii++)
    {
        for (int jj = 0; jj < 3; jj++)
        {
            Wm[ii][jj] = 0.5 * (Im[ii][jj] - Im[jj][ii]);
            R1[ii][jj] = delt[ii][jj] - 0.5 * Wm[ii][jj];
            R2[ii][jj] = delt[ii][jj] + 0.5 * Wm[ii][jj];
            Dm[ii][jj] = 0.5 * (Im[ii][jj] + Im[jj][ii]);
        }
    }
    // 形成Q
    toolbox::invertMatrix(R1, R1_inv);
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
}
void Hypoelastoplastic::getDQetc(ObjectElementData & element_data, const int ip_num, const int node_num,
                           const double Fm_inv[3][3],
                           const double Im[3][3],
                           const double R1_inv[3][3],
                           const double Q[3][3],
                           double dIm_du_K[3][3][3],
                           double ddm_du_K[3][3][3],
                           double dW_du_K[3][3][3],
                           double dQ_du_K[3][3][3],
                           double dNK_dxm[3])
{
    double delt[3][3] = {0.0};
    //初始化一些变量
    for (int i = 0; i < 3; i++)
    {
        delt[i][i] = 1.0;
        dNK_dxm[i] = 0.0;
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                dIm_du_K[i][j][k] = 0.0;
                ddm_du_K[i][j][k] = 0.0;
                dW_du_K[i][j][k] = 0.0;
                dQ_du_K[i][j][k] = 0.0;                
            }
        }
    }
    const auto & dNK_dX = element_data.sfdxyz[ip_num][node_num];
    for (int ii = 0; ii < 3; ii++)
    {
        for (int jj = 0; jj < 3; jj++)
        {
            dNK_dxm[jj] += dNK_dX[ii] * Fm_inv[ii][jj];
        }
    }

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
                        dQ_du_K[ii][jj][kk] += (0.5 * R1_inv[ii][p] * dW_du_K[p][q][kk] * Q[q][jj]);
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
                    dQ_du_K[ii][jj][kk] += (0.5 * R1_inv[ii][p] * dW_du_K[p][jj][kk]);
                }
            }
        }
    }
}



void Hypoelastoplastic::preBbar(ObjectElementData & element_data,
                          double & e_dil_Bbar,
                          double Fm_Bbar[3][3],
                          double Fm_Bbar_inv[3][3])
{
    const auto & centroid_Fn = element_data.centroid_F_n;    
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Fm_Bbar[i][j] = centroid_Fn[i][j];
            Fm_Bbar_inv[i][j] = 0.0;
        }
    } 
    int num_node = element_data.num_nodes;

    for (int i = 0; i < num_node; i++)
    {
        const auto & du_I = element_data.du[i];
        const auto & dNI_dX = element_data.centroid_sfdxy[i];
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {                
                Fm_Bbar[ii][jj] += (0.5 * du_I[ii] * dNI_dX[jj]);
            }
        }
    }

    // double Fm_Bbar_inv[3][3] = {0.0};
    toolbox::invertMatrix(Fm_Bbar, Fm_Bbar_inv);

    for (int i = 0; i < num_node; i++)
    {
        const auto & centroid_dNI_dX = element_data.centroid_sfdxy[i];
        const auto & du_I = element_data.du[i];
        double centroid_dNI_dxm[3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                centroid_dNI_dxm[jj] += centroid_dNI_dX[ii] * Fm_Bbar_inv[ii][jj];
            }
        }
        for (int ii = 0; ii < 3; ii++)
        {
            e_dil_Bbar += du_I[ii] * centroid_dNI_dxm[ii];
        }
    }
    e_dil_Bbar = e_dil_Bbar / 3.0;



}

void Hypoelastoplastic::get_centroid_de_du(ObjectElementData &element_data,
                                     const double e_dil_Bbar,
                                     const double Fm_Bbar[3][3],
                                     const double Fm_Bbar_inv[3][3],
                                     std::vector<std::vector<double>> & centroid_de_du)
{
    int num_node = element_data.num_nodes;    
    double delt[3][3] = {0.0};
    for (int i = 0; i < 3; i++) delt[i][i] = 1.0;
    double centroid_dNIs_dxm[num_node][3] = {0.0};
    for (int i = 0; i < num_node; i++)
    {
        const auto & centroid_dNI_dX = element_data.centroid_sfdxy[i];
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                centroid_dNIs_dxm[i][jj] += centroid_dNI_dX[ii] * Fm_Bbar_inv[ii][jj];
            }
        }
    }

    // 不用循环积分点，只需要循环节点即可
    for (int J = 0; J < num_node; J++)
    {        
        double T2[3][3][3] = {0.0};
        for (int K = 0; K < num_node; K++)
        {
            const auto & du_K = element_data.du[K];
            for (int ii = 0; ii < 3; ii++)
            {
                for (int jj = 0; jj < 3; jj++)
                {
                    for (int kk = 0; kk < 3; kk++)
                    {
                        for (int p = 0; p < 3; p++)
                        {
                            T2[ii][jj][kk] += 0.5 * centroid_dNIs_dxm[K][kk] * centroid_dNIs_dxm[J][p] * du_K[p] * delt[ii][jj] * (1.0 / 3.0);
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
                    centroid_de_du[J][ii * 9 + jj * 3 + kk] = (1.0 / 3.0) * centroid_dNIs_dxm[J][kk] * delt[ii][jj] - T2[ii][jj][kk];
                }
            }
        }
    }



}

void Hypoelastoplastic::get_e_dil(ObjectElementData & element_data,
                            const int ip_num,
                            const double Fm_inv[3][3],
                            double & e_dil)
{
    e_dil = 0.0;
    int num_node = element_data.num_nodes;
    for (int ii = 0; ii < num_node; ii++)
        {
            const auto & du_K = element_data.du[ii];
            const auto & dNK_dX = element_data.sfdxyz[ip_num][ii];
            double dNK_dxm[3] = {0.0};
            for (int jj = 0; jj < 3; jj++)
            {
                for (int kk = 0; kk < 3; kk++)
                {
                    dNK_dxm[kk] += dNK_dX[jj] * Fm_inv[jj][kk];
                }
            }

            for (int jj = 0; jj < 3; jj++)
            {
                e_dil += dNK_dxm[jj] * du_K[jj];            
            }
        } 
        e_dil = e_dil / 3.0;
}


void Hypoelastoplastic::update_ddm_du_K_ForBbarElement(ObjectElementData & element_data,
                                                 const int ip_num,
                                                 const int node_num,
                                                 const double Fm_inv[3][3],
                                                 const double dNK_dxm[3],
                                                 const std::vector<std::vector<double>> & centroid_de_du,
                                                 double ddm_du_K[3][3][3])
{
    double delt[3][3] = {0.0};
    for (int i = 0; i < 3; i++) delt[i][i] = 1.0;
    int num_node = element_data.num_nodes;
    double T1[3][3][3] = {0.0};
    for (int J = 0; J < num_node; J++)
    {
        const auto & du_J = element_data.du[J];
        const auto & dNJ_dX = element_data.sfdxyz[ip_num][J];
        double dNJ_dxm[3] = {0.0};
        for (int ii = 0; ii < 3; ii++)
        {
            for (int jj = 0; jj < 3; jj++)
            {
                dNJ_dxm[jj] += dNJ_dX[ii] * Fm_inv[ii][jj];
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
                        T1[ii][jj][kk] += 0.5 * dNJ_dxm[kk] * dNK_dxm[p] * du_J[p] * delt[ii][jj];
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
                 ddm_du_K[ii][jj][kk] = ddm_du_K[ii][jj][kk] -
                                        dNK_dxm[kk] * delt[ii][jj] * (1.0 / 3.0) + (1.0 / 3.0) * T1[ii][jj][kk] +
                                        centroid_de_du[node_num][ii * 9 + jj * 3 + kk];
                 // std::cout << - dNK_dxm[kk] * delt[ii][jj] * (1.0 / 3.0) + (1.0 / 3.0) * T1[ii][jj][kk] << "  " << centroid_de_du[K][ii * 9 + jj * 3 + kk] << std::endl;
             }
         }
     }
}

void Hypoelastoplastic::getDeltaGamma(const double epn,
                                      const double normal_stress_dev,
                                      const double mu,
                                      const double init_Y,
                                      double &deltaGamma)
{
    deltaGamma = 0.0;
    double G2 = G(epn, normal_stress_dev, mu, init_Y, deltaGamma);
    double G_abs = std::fabs(G2);
    double tol = d_iner_loop_tol;
    int i = 0;
    while (G_abs > tol)
    {
        i++;
        double G2 = G(epn, normal_stress_dev, mu, init_Y, deltaGamma);
        double G_dx2 = G_dx(epn, mu, init_Y, deltaGamma);
        deltaGamma = deltaGamma - G2 / G_dx2;
        G_abs = std::fabs(G2);
        if (i > 1000) toolbox::error("too many tries in iner loop");
    }
    if (deltaGamma < 0.0) toolbox::error("deltaGamma error");
}

void Hypoelastoplastic::getCep(const double lambda,
                               const double mu,
                               const double init_Y,
                               const double n[3][3],
                               const double epn1,
                               const double kappa_n1,
                               const double normal_stress_dev,
                               double Cep[3][3][3][3])
{
    const double c = std::sqrt(2.0 / 3.0);
    double I[3][3] = {0.0};
    for (int i = 0; i < 3; i++) I[i][i] = 1.0;
    double I_dev[3][3][3][3] = {0.0};
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    I_dev[i][j][k][l] = 0.5 * (I[i][k] * I[j][l] + I[i][l] * I[j][k] ) - 
                                        (1.0 / 3.0) * I[i][j] * I[k][l];
                }
            }
        }
    }
    double beta1 = c * kappa_n1 / normal_stress_dev - 1.0;
    double beta2 = mu + kappa_dx(init_Y, epn1) / 3.0;
    beta2 = mu / beta2;
    beta2 = beta2 + beta1;
    const double kk1 = lambda + 2.0 * mu / 3.0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    Cep[i][j][k][l] = d_C_e_tensor[i][j][k][l] - 
                                      2.0 * mu * (-beta1 * I_dev[i][j][k][l] + beta2 * n[i][j] * n[k][l]);
                }
            }
        }
    }



}
