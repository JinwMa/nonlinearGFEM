#include "ConstraintInterface.h"



std::shared_ptr<BaseConstraint> newConstraint(const string & type)
{
    if (type == "")
    {

    }
    else
    {
        toolbox::error("not supported constraint type");
    }
    return nullptr;
}