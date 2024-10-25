#ifndef ROTATEBODYFORCE_H
#define ROTATEBODYFORCE_H

#include "BaseLoad.h"

class RotateBodyForce : public BaseLoad
{
    public:
    RotateBodyForce(){};
    ~RotateBodyForce(){};
    void takeDB(Input *pinput, Mesh *pmesh, std::string name = "");
    void buildLoadVector(std::vector<int> & dofs_index, std::vector<double> & loadvector);
    void buildLoadStiffnessMatrix(std::vector<int> & dofs_index, std::vector<double> & loadstiffnessmatrix);

    private:
    std::vector<double> d_rotation_axis;
    std::vector<double> d_rotation_refer_point;
    double d_angular_velocity;
    std::vector<int> d_element_ids;

};





#endif