#include "LinearHex8.h"

void LinearHex8::init()
{
    BaseClass::init();
    BaseClass::setGuassInfoHex8();
}


void LinearHex8::allocateElementData(ElementData & elementData)
{
    BaseClass::allocateElementData(elementData);
}

void LinearHex8::updateElementData(ElementData & elementData){}