#ifndef LINEARHEX81_H
#define LINEARHEX81_H

#include "BaseElement.h"

class LinearHex8 : public BaseElement
{    
    public:
    using BaseClass = BaseElement;
    LinearHex8(){};
    virtual ~LinearHex8() {};
    virtual void init();
    virtual void allocateElementData(ElementData & elementData);
    virtual void updateElementData(ElementData & elementData);


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
    
    //
};


#endif
