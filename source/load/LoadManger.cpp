#include "LoadManger.h"


void LoadManger::takeDB(Input * pinput, Mesh * pmesh, Dof_Map * pdofmap)
{
   if (pinput->ifExist("load_list")) d_load_list = pinput->getVectorString("load_list");
}


void LoadManger::buildLoadForce(Input * pinput, Mesh * pmesh, Dof_Map * pdofmap, Eigen::VectorXd & forcevector)
{
    forcevector.resize(pdofmap->dof_size);
    forcevector.setZero();
    BaseLoad * load;
    for (auto name : d_load_list)
    {
        std::vector<int> dof_index;
        std::vector<double> load_force_vector;
        std::string load_type = pinput->getString(name + "_type");
        if (load_type == "rotate_body_force")
        {
            load = new RotateBodyForce;
        }
        else if (load_type == "nodal_force")
        {
            load = new NodalForce;
        }
        else
        {
            toolbox::error("not support type of " + load_type + " in " + name);
        }
        load->takeDB(pinput, pmesh, name);
        load->buildLoadVector(pinput, pmesh, pdofmap, dof_index, load_force_vector);
        assembleForceVector(dof_index, load_force_vector, forcevector);
        delete load;
    }
}

 void LoadManger::assembleForceVector(std::vector<int> & dof_index,
                                      std::vector<double> & force,
                                      Eigen::VectorXd & forcevector)
{
    //对齐检查
    if (dof_index.size() != force.size()) toolbox::error("the size of dof_index and force is not matched");
    for (size_t i = 0; i < force.size(); i++)
    {
        int dof_id = dof_index[i];
        double value = force[i];
        forcevector(dof_id) += value;
    }
}