#include "BaseConstraint.h"

void BaseConstraint::takeDB(Input * input, Mesh * mesh)
{
    d_input = input;
    d_mesh = mesh;
}