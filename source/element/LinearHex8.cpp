#include "LinearHex8.h"

void LinearHex8::init()
{
    d_integration_order = 2;
    d_numEdofs = 24;
    d_numNodes = 8;
    d_dof_labs = {"ux", "uy", "uz"};
    d_variableSize = 0;
    BaseClass::init();    
    BaseClass::setGuassInfoHex8();
}


void LinearHex8::allocateElementData(ElementData & elementData)
{
    BaseClass::allocateElementData(elementData);
}

void LinearHex8::updateElementData(ElementData & elementData){}