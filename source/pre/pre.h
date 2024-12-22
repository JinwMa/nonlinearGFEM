#ifndef PRE1_H
#define PRE1_H

//处理一些有特殊需求的案例
//特殊案例通过input文件的case_id来进行判断


#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>
#include "input.h"
#include "mesh.h"
#include "toolbox.h"

class Pre
{
    public:
    Pre(){};
    ~Pre(){};
    void preprocess(Input * pinput, Mesh * pmesh);
};


#endif