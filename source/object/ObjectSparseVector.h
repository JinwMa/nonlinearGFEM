#ifndef OBJECTSPARSEVECTOR
#define OBJECTSPARSEVECTOR

#include<vector>

class SparseVector
{
    SparseVector(){};
    virtual ~SparseVector();

    std::vector<double> d_vec;
};



#endif