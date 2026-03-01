#include "BaseMaterial.h"

BaseMaterial::BaseMaterial(const std::string& name, double density)
    : name_(name), density_(density) {}

std::string BaseMaterial::getName() const {
    return name_;
}

double BaseMaterial::getDensity() const {
    return density_;
}

void BaseMaterial::setDensity(double density) {
    density_ = density;
}

BaseMaterial::~BaseMaterial() {
    // Virtual destructor for proper cleanup in derived classes
}