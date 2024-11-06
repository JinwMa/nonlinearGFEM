#include "KirchhoffHyperelasticity.h"

void KirchhoffHyperelasticity::takeDB(Input * pinput, std::string & name)
{
    E = pinput->getDouble(name + "_modulus");
    u = pinput->getDouble(name + "_radio");
}