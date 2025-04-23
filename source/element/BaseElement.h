#ifndef BASEELEMENT_H
#define BASEELEMENT_H

#include "input.h"
#include "mesh.h"
#include "ObjectElementData.h"
#include "ObjectControlParam.h"
#include "toolbox.h"
#include "MaterialInterface.h"
struct GaussPointInfo
{
    std::vector<double> pcoord; // 参数坐标
    double weight; // 积分权重
    // 备注：不能存放雅可比,因为每个单元的雅可比不一样
};

class BaseElement
{
    public:
    BaseElement(){};
    virtual ~BaseElement(){};

    virtual void init(){};
    virtual void takeDB(shared_ptr<DataBase> db);

    virtual void elementStiffness(ElementData & elementData,
                                  std::vector<double> & elementMat,
                                  std::shared_ptr<ObjectControlParam> control = nullptr){};
    virtual void elementInterForce(ElementData & elementData,
                                   std::vector<double> & elementVector,
                                   std::shared_ptr<ObjectControlParam> control = nullptr){};
    virtual void updateElementData(ElementData & elementData){};

    virtual void initElementData(ElementData & elementData){};

    void setGuassInfoHex8();

    //
    int d_integration_order;
    std::vector<GaussPointInfo> d_GuassPointVector;
    int d_numEdofs;
    int d_numNodes;
    // 单元上的自由度标签
    std::vector<std::string> d_dof_labs;
    std::shared_ptr<BaseMaterial> d_material;


};



#endif