#include "BaseMaterial.h"

BaseMaterial::BaseMaterial(const std::string& name, double density)
    : d_name(name), d_density(density),
      d_youngsModulus(0.0), d_poissonsRatio(0.0),
      d_yieldStress(0.0), d_hardeningModulus(0.0) {}

std::string BaseMaterial::getName() const {
    return d_name;
}

double BaseMaterial::getDensity() const {
    return d_density;
}

void BaseMaterial::setDensity(double density) {
    d_density = density;
}

BaseMaterial::~BaseMaterial() {
    // Virtual destructor for proper cleanup in derived classes
}