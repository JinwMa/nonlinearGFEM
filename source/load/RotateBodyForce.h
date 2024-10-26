#ifndef ROTATEBODYFORCE_H
#define ROTATEBODYFORCE_H

#include "BaseLoad.h"
#include <unordered_map>

class RotateBodyForce : public BaseLoad
{
    public:
    RotateBodyForce(){};
    ~RotateBodyForce(){};
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
                                  std::vector<double> & loadvector);

    private:
    std::vector<double> d_rotation_axis;
    std::vector<double> d_rotation_refer_point;
    double d_angular_velocity;
    std::vector<int> d_element_ids;
    std::unordered_map<std::string, std::vector<int>> d_set_elements;



};





#endif