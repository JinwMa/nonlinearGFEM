#ifndef LINEARSTATICSOLVER1_H
#define LINEARSTATICSOLVER1_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_set>
#include "dof_map.h"
using namespace std;
class LinearStaticSolver
{
    public:
    LinearStaticSolver(){}
    void solve();
};

#endif // POST1_H