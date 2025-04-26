#ifndef OBJECTSPARSFactor
#define OBJECTSPARSFactor
#include<string>
#include<vector>


using namespace std;

struct RowInSparse
{
    vector<double> values;
    vector<int> index;
};


class SparseFactor
{
    public:
    SparseFactor(string name = "K", string type = "Full")
    {
        d_name = name;
        d_type = type;
    }
    virtual ~SparseFactor(){};


//
    
    int d_dimension = 0;
    string d_name;
    string d_type;    
    
    vector<int> d_minIndex;
    vector<int> d_maxIndex;
    vector<RowInSparse> d_Rows;

};


#endif