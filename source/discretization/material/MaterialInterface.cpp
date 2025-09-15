#include "MaterialInterface.h"


std::shared_ptr<BaseMaterial> newMaterial(const string & type)
{
    if (type == "LinearElasticity")
    {
        return make_shared<LinearElastic>();
    }
    else
    {
        toolbox::error("not supported material type");
    }
    return nullptr;
}