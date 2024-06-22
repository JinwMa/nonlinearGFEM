#ifndef LINEARHEX81_H
#define LINEARHEX81_H

#include "BaseElement.h"

#include <iostream>
class LinearHex8 : public BaseElement{
    static const int num_edofs = 24;
    static const int num_nodes = 8;
    static const int num_node_dof = 3;
    public:
    void ComputeStiffness(int i)
    {
        std::cout<< i << std::endl;
    }
};

#endif