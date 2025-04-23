#include "BaseMaterial.h"

void BaseMaterial::takeDB(std::shared_ptr<DataBase> db)
{
    d_E = db->getDouble("E");
    d_v = db->getDouble("v");
}