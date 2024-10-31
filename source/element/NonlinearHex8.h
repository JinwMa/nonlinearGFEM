#ifndef NONLINEARHEX81_H
#define NONLINEARHEX81_H

#include "BaseElement.h"

class NonlinearHex8 : public BaseElement
{
    public:
    static const int num_edofs = 24;
    static const int num_nodes = 8;
    static const int num_node_dof = 3; 
    public:
        string ReturnElementType() { return "NonlinearFEM"; };
        ~NonlinearHex8() {};
};



#endif