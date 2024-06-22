#include "mesh.h"
#include <string>
#include <iostream>
#include "mesh.h"
#include "input.h"
#include "LinearHex8.h"
void solve(Input & input, Mesh & mesh)
{
    for (auto key : input.keys)
    std::cout << key << std::endl;
    auto element = new LinearHex8;
    // element->ComputeStiffness();

}