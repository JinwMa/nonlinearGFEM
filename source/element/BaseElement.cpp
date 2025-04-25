#include "BaseElement.h"


void BaseElement::takeDB(shared_ptr<DataBase> db)
{
    if(db->ifExist("integration_order")) d_integration_order = db->getInt("integration_order");
    std::string material_name = db->getString("material");
    auto modal_market_db = db->d_root_db->getDataBase("modal_market");
    auto material_db = modal_market_db->getDataBase(material_name);
    std::string material_type = material_db->getString("type");
    d_material = newMaterial(material_type);
    d_material->takeDB(material_db);
}





void BaseElement::setGuassInfoHex8()
{
    // 设置积分点数目
    int numPoints = d_integration_order * d_integration_order * d_integration_order;
    d_GuassPointVector.resize(numPoints);
    for (int i = 0; i < numPoints; i++) d_GuassPointVector[i].pcoord.resize(3);
    double xs[d_integration_order][2] = {0.0};
    if (d_integration_order == 1)
    {
        xs[0][0] = 0.0;
        xs[0][1] = 2.0;
    }
    else if (d_integration_order == 2)
    {
        xs[0][0] = -0.577350269189626;
        xs[0][1] = 1.0;

        xs[1][0] = 0.577350269189626;
        xs[1][1] = 1.0;
    }
    else if (d_integration_order == 3)
    {
        xs[0][0] = -0.774596669241483;
        xs[0][1] = 0.555555555555556;
        xs[1][0] = 0.0;
        xs[1][1] = 0.888888888888889;
        xs[2][0] = 0.774596669241483;
        xs[2][1] = 0.555555555555556;
    }
    else if (d_integration_order == 4)
    {
        xs[0][0] = -0.861136311594053;
        xs[0][1] = 0.347854845137454;
        xs[1][0] = -0.339981043584856;
        xs[1][1] = 0.652145154862546;
        xs[2][0] = 0.339981043584856;
        xs[2][1] = 0.652145154862546;
        xs[3][0] = 0.861136311594053;
        xs[3][1] = 0.347854845137454;
    }
    else if (d_integration_order == 6)
    {
        xs[0][0] = -0.932469514203152;
        xs[1][0] = -0.661209386466265;
        xs[2][0] = -0.238619186083197;
        xs[3][0] = 0.238619186083197;
        xs[4][0] = 0.661209386466265;
        xs[5][0] = 0.932469514203152;

        xs[0][1] = 0.171324492379170;
        xs[1][1] = 0.360761573048139;
        xs[2][1] = 0.467913934572691;
        xs[3][1] = 0.467913934572691;
        xs[4][1] = 0.360761573048139;
        xs[5][1] = 0.171324492379170;
    }
    else
    {
        toolbox::error("not support this order of integration");
    }
    int index = 0;
    for (int i = 0; i < d_integration_order; i++)
    {
        for (int j = 0; j < d_integration_order; j++)
        {
            for (int k = 0; k < d_integration_order; k++)
            {
                d_GuassPointVector[index].pcoord[0] = xs[i][0];
                d_GuassPointVector[index].pcoord[1] = xs[j][0];
                d_GuassPointVector[index].pcoord[2] = xs[k][0];
                d_GuassPointVector[index].weight = xs[i][1] * xs[j][1] * xs[k][1];
                index++;
            }
        }
    }
}
