#include "LoadManger.h"


void LoadManger::takeDB(Input * pinput, Mesh * pmesh, Dof_Map * pdofmap)
{
    d_load_list = pinput->getVectorString("load_list");
}


void LoadManger::buildLoadForce(Input * pinput, Mesh * pmesh, Dof_Map * pdofmap)
{
    BaseLoad * load;
    for (auto name : d_load_list)
    {
        std::string load_type = pinput->getString(name + "_type");
        if (load_type == "rotate_body_force")
        {
            load = new RotateBodyForce;
        }
        else
        {
            toolbox::error("not support type of " + load_type + " in " + name);
        }
        load->takeDB(pinput, pmesh, name);
    }
}