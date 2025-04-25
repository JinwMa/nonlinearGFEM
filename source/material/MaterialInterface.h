
#ifndef MATERIALINTERFACE
#define MATERIALINTERFACE

#include"BaseMaterial.h"
#include"LinearElastic.h"



#include<memory>
#include<string>



std::shared_ptr<BaseMaterial> newMaterial(const string & type);



#endif