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
    double weight;              // 积分权重
    // 备注：不能存放雅可比,因为每个单元的雅可比不一样
};

class BaseElement
{
public:
    BaseElement() {};
    virtual ~BaseElement() {};

    
    // input文件交互
    virtual void takeDB(shared_ptr<DataBase> db);
    // 设置积分点等信息
    virtual void init() {};

    virtual void elementStiffness(ElementData &elementData,
                                  std::vector<double> & xyz,
                                  std::vector<double> & u,
                                  std::vector<double> & du,
                                  std::vector<double> & elementMat,
                                  std::shared_ptr<ObjectControlParam> control = nullptr) {};
    virtual void elementInterForce(ElementData &elementData,
                                   std::vector<double> & xyz,
                                   std::vector<double> & u,
                                   std::vector<double> & du,
                                   std::vector<double> & elementVector,
                                   std::shared_ptr<ObjectControlParam> control = nullptr) {};






    virtual void updateElementData(ElementData &elementData) {};      // 单元内变量更新
    virtual void allocateElementData(ElementData &elementData);       // 为单元内变量开辟空间 -- 调用该函数不要并行


    void setGuassInfoHex8();                                          // 设置六面体积分信息
    // 后续补充计算形函数形函数导数模块


    virtual vector<string> getDofLab(){return d_dof_labs;}


    

    //
    int d_integration_order;                                           // 积分阶次 - 单元上需要设置默认值
    std::vector<GaussPointInfo> d_GuassPointVector;                    // 积分点信息向量
    int d_numEdofs;                                                    // 单元上的自由度数
    int d_numNodes;                                                    // 单元上的节点数目
    std::vector<std::string> d_dof_labs = {};                               // 单元上的自由度标签
    std::shared_ptr<BaseMaterial> d_material;                          // 材料指针 -- 后续补充截面指针
    int d_variableSize = 0;                                           // 单元内变量长度
};

#endif