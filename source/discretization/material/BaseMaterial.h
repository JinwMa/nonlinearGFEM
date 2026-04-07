#ifndef BASE_MATERIAL_H
#define BASE_MATERIAL_H

#include <string>
#include <vector>
#include <memory>
#include <Eigen/Dense> // For matrix operations
#include "Matrix.h"

// Forward declaration of DataBase
namespace std {
    template<typename T> class shared_ptr;
}
class DataBase;

// Material type enumeration
enum class MaterialType {
    LINEAR_ELASTIC,
    NONLINEAR_ELASTIC,
    ELASTIC_PLASTIC,
    VISCOELASTIC,
    HYPERELASTIC,
    USER_DEFINED
};

class BaseMaterial {
public:
    // Constructor
    BaseMaterial(const std::string& name, double density);

    // Virtual destructor for polymorphism
    virtual ~BaseMaterial();

    // Getters
    std::string getName() const;
    double getDensity() const;
    virtual MaterialType getMaterialType() const = 0;

    // Setters
    void setDensity(double density);

    // Database initialization - read material parameters from input database
    virtual void takeDB(std::shared_ptr<DataBase> db) = 0;

    // Stress update interface for nonlinear materials
    // strain_inc: strain increment (6x1 vector for 3D, 3x1 for 2D)
    // stress: current stress (input), updated stress (output)
    // strain: current strain (input), updated strain (output)
    // state_vars: vector of state variables (plastic strain, equivalent plastic strain, etc.)
    // Returns true if update succeeded, false if failed (e.g., material failure)
    virtual bool updateStress(const Eigen::VectorXd& strain_inc,
                              Eigen::VectorXd& stress,
                              Eigen::VectorXd& strain,
                              std::vector<double>& state_vars) = 0;

    // Get constitutive matrix (stress increment vs strain increment)
    // Returns the tangent modulus matrix (6x6 for 3D, 3x3 for 2D)
    virtual Eigen::MatrixXd getDSDu() const = 0;

    // For linear materials: compute stress directly from strain
    virtual Eigen::VectorXd computeStress(const Eigen::VectorXd& strain) const = 0;

    // For linear materials: compute tangent stiffness matrix
    virtual Eigen::MatrixXd computeTangentStiffnessMatrix() const = 0;

    // Check if material is linear (for solver selection)
    virtual bool isLinear() const = 0;

    // Get number of state variables required by this material
    virtual int getNumStateVariables() const = 0;

    // Initialize state variables (e.g., set to zero)
    virtual void initializeStateVariables(std::vector<double>& state_vars) const = 0;

protected:
    std::string name_; // Material name
    double density_;   // Material density

    // Common material parameters (optional, derived classes can store their own)
    double youngs_modulus_;
    double poissons_ratio_;
    double yield_stress_;
    double hardening_modulus_;
};

#endif // BASE_MATERIAL_H