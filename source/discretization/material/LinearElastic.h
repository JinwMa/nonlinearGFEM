#ifndef LINEAR_ELASTIC_H
#define LINEAR_ELASTIC_H

#include "BaseMaterial.h"
#include <memory>

// Forward declaration
class DataBase;

class LinearElastic : public BaseMaterial {
public:
    // Constructor with explicit parameters
    LinearElastic(const std::string& name, double density, double youngsModulus, double poissonsRatio);

    // Constructor taking database
    LinearElastic(const std::string& name, double density);

    virtual ~LinearElastic() override = default;

    // Material type
    virtual MaterialType getMaterialType() const override { return MaterialType::LINEAR_ELASTIC; }

    // Database initialization
    virtual void takeDB(std::shared_ptr<DataBase> db) override;

    // Stress update for linear elastic (simple incremental form)
    virtual bool updateStress(const Eigen::VectorXd& strain_inc,
                              Eigen::VectorXd& stress,
                              Eigen::VectorXd& strain,
                              std::vector<double>& state_vars) override;

    // Get constitutive matrix (elasticity matrix)
    virtual Eigen::MatrixXd getDSDu() const override;

    // Compute stress directly from strain (for linear materials)
    virtual Eigen::VectorXd computeStress(const Eigen::VectorXd& strain) const override;

    // Compute tangent stiffness matrix (same as elasticity matrix for linear elastic)
    virtual Eigen::MatrixXd computeTangentStiffnessMatrix() const override;

    // Check if material is linear
    virtual bool isLinear() const override { return true; }

    // Number of state variables (linear elastic has none)
    virtual int getNumStateVariables() const override { return 0; }

    // Initialize state variables (empty for linear elastic)
    virtual void initializeStateVariables(std::vector<double>& state_vars) const override;

    // Getters for material properties
    double getYoungsModulus() const { return d_youngsModulus; }
    double getPoissonsRatio() const { return d_poissonsRatio; }

    // Setters
    void setYoungsModulus(double E) { d_youngsModulus = E; }
    void setPoissonsRatio(double nu) { d_poissonsRatio = nu; }

private:
    double d_youngsModulus;
    double d_poissonsRatio;

    // Helper method to compute elasticity matrix (3D)
    Eigen::MatrixXd computeElasticityMatrix3D() const;

    // Helper method to compute elasticity matrix (2D plane stress)
    Eigen::MatrixXd computeElasticityMatrix2DPlaneStress() const;

    // Helper method to compute elasticity matrix (2D plane strain)
    Eigen::MatrixXd computeElasticityMatrix2DPlaneStrain() const;

    // Determine problem dimension based on input vector size
    int getProblemDimension(int strain_size) const;
};

#endif // LINEAR_ELASTIC_H