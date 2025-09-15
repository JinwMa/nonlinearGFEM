#ifndef CONSTRAINTINTERFACE
#define CONSTRAINTINTERFACE

#include "BaseConstraint.h"
#include <memory>
#include <string>

std::shared_ptr<BaseConstraint> newConstraint(const string & type,
                                              shared_ptr<DataBase> db, 
                                              shared_ptr<Mesh> mesh,
                                              shared_ptr<DofMap> dofmap);







#endif