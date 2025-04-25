#ifndef CONSTRAINTINTERFACE
#define CONSTRAINTINTERFACE

#include "BaseConstraint.h"
#include <memory>
#include <string>

std::shared_ptr<BaseConstraint> newConstraint(const string & type);







#endif