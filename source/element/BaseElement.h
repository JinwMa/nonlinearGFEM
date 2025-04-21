#ifndef BASEELEMENT_H
#define BASEELEMENT_H

#include "input.h"
#include "mesh.h"
#include "ObjectElementData.h"
#include "ObjectControlParam.h"
#include "toolbox.h"
struct GaussPointInfo
{

};

class BaseElement
{
    BaseElement(){};
    virtual ~BaseElement();

    virtual void init(ElementData & elementData);
    virtual void takeDB(shared_ptr<DataBase> db);

    virtual void elementStiffness(ElementData & elementData,
                                  std::vector<double> & elementMat,
                                  std::shared_ptr<ObjectControlParam> control = nullptr);
    virtual void elementInterForce(ElementData & elementData,
                                   std::vector<double> & elementVector,
                                   std::shared_ptr<ObjectControlParam> control = nullptr);
    virtual void updateElementData(ElementData & elementData);


};



#endif