#ifndef ELEMENTINTERFACE_H
#define ELEMENTINTERFACE_H

#include "BaseElement.h"
#include "LinearHex8.h"
#include "input.h"
#include "mesh.h"

#include<memory>
#include<string>
#include<vector>

std::shared_ptr<BaseElement> newElement(const std::string & type); 


// 获取单元群id id对应外部输入的id
void getGroup(std::shared_ptr<DataBase> db,
              std::shared_ptr<Mesh> mesh,
              std::vector<int> & igroup);

#endif