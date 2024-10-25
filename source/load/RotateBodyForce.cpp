#include "RotateBodyForce.h"


void RotateBodyForce::takeDB(Input * pinput, Mesh * pmesh, std::string name)
{
    if (name.empty()) toolbox::error("name of RotateBodyForce should not be empty");
    d_rotation_axis = pinput->getVectorDouble(name + "_rotation_axis");
    d_rotation_refer_point = pinput->getVectorDouble(name + "_rotation_refer_point");
    d_angular_velocity = pinput->getDouble(name + "_angular_velocity");
    if (!pinput->ifExist(name + "_set_id"))
    {
        d_element_ids = pmesh->ElementIdList;
    }
    else
    {
        int set_id = pinput->getInt(name + "_set_id");
        d_element_ids = pmesh->element_sets[set_id];
    }
}

void RotateBodyForce::buildLoadVector(std::vector<int> & dofs_index, std::vector<double> & loadvector)
{
    
}

void RotateBodyForce::buildLoadStiffnessMatrix(std::vector<int> & dofs_index, std::vector<double> & loadstiffnessmatrix)
{

}