#include "LinearHex8.h"

void LinearHex8::init()
{
    BaseClass::init();
    BaseClass::setGuassInfoHex8();
}


void LinearHex8::initElementData(ElementData & elementData)
{

}

void LinearHex8::updateElementData(ElementData & elementData){}
void LinearHex8::elementStiffness(ElementData & elementData,
                      std::vector<double> & elementMat,
                      std::shared_ptr<ObjectControlParam> control){}
void LinearHex8::elementInterForce(ElementData & elementData,
                       std::vector<double> & elementVector,
                       std::shared_ptr<ObjectControlParam> control){}