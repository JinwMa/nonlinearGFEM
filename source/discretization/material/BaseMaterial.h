#ifndef BASE_MATERIAL_H
#define BASE_MATERIAL_H

#include <string>
#include <vector>
#include <Eigen/Dense> // For matrix operations
#include "Matrix.h"

class BaseMaterial {
public:
    // Constructor
    BaseMaterial(const std::string& name, double density);

    // Virtual destructor for polymorphism
    virtual ~BaseMaterial();

    // Getters
    std::string getName() const;
    double getDensity() const;
    // Setters
    void setDensity(double density);

    // Virtual methods to be implemented by derived classes
    virtual Matrix<double> computeStress(const Eigen::MatrixXd& strain) const = 0;
    virtual Matrix<double> computeTangentStiffnessMatrix() const = 0;

    // Additional utility methods
    virtual bool isLinear() const = 0;

protected:
    std::string name_; // Material name
    double density_;   // Material density
};

#endif // BASE_MATERIAL_H