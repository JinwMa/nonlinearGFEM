#ifndef NODALFORCE_H
#define NODALFORCE_H


#include "BaseLoad.h"

class NodalForce : public BaseLoad
{
    public:
    NodalForce(){};
    ~NodalForce(){};
    void takeDB(Input *pinput, Mesh *pmesh, std::string name = "");
    void buildLoadVector(Input * pinput,
                         Mesh * pmesh,
                         Dof_Map * pdofmap,
                         std::vector<int> & dofs_index,
                         std::vector<double> & loadvector);
    void buildLoadStiffnessMatrix(Input * pinput,
                                  Mesh * pmesh,
                                  Dof_Map * pdofmap,
                                  std::vector<int> & dofs_index,
                                  std::vector<double> & loadvector){};

    private:
    std::vector<int> node_ids;
    std::vector<int> node_set_ids;
    std::vector<double> load_vector;
    double load_value; 
};
#endif 