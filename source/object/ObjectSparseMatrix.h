#ifndef OBJECTSPARSMATRIX
#define OBJECTSPARSMATRIX

#include<string>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/PardisoSupport>

class SparseMatrix
{
    public:
    SparseMatrix(std::string name = "")
    {
        d_name = name;
    }
    virtual ~SparseMatrix(){}

    std::string d_name;
    Eigen::SparseMatrix<double> d_matrix;
    
};




#endif