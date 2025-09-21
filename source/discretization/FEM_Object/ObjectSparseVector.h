#ifndef OBJECTSPARSEVECTOR
#define OBJECTSPARSEVECTOR

#include<vector>

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include<string>

class SparseVector
{
    public:
    SparseVector(std::string name = "")
    {
        d_name = name;
    };
    virtual ~SparseVector(){};

    std::vector<double> d_vec;
    Eigen::VectorXd d_EigenVector;
    std::string d_name;
};



#endif