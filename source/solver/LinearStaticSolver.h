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
#include"input.h"
#include"mesh.h"
#include"BaseSolver.h"
using namespace std;
class LinearStaticSolver : public BaseSolver
{
    public:
    LinearStaticSolver(){}
    void solve();
    void takeDB(Input & input, Mesh & mesh);
};

#endif // POST1_H