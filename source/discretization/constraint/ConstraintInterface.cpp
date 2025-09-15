#include "ConstraintInterface.h"
#include "SPC.h"


std::shared_ptr<BaseConstraint> newConstraint(const string & type,
                                              shared_ptr<DataBase> db, 
                                              shared_ptr<Mesh> mesh,
                                              shared_ptr<DofMap> dofmap)
{
    if (type == "SPC")
    {
        return make_shared<SPC>(db, mesh, dofmap); 
    }
    else
    {
        toolbox::error("not supported constraint type");
    }
    return nullptr;
}