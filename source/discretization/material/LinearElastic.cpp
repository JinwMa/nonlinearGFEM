#include "LinearElastic.h"
#include "input.h"
#include <Eigen/Dense>
#include <iostream>
#include <stdexcept>

// Constructor with explicit parameters
LinearElastic::LinearElastic(const std::string& name, double density,
                             double youngs_modulus, double poissons_ratio)
    : BaseMaterial(name, density),
      youngs_modulus_(youngs_modulus),
      poissons_ratio_(poissons_ratio) {
    // Validate parameters
    if (youngs_modulus <= 0.0) {
        throw std::invalid_argument("Young's modulus must be positive");
    }
    if (poissons_ratio <= -1.0 || poissons_ratio >= 0.5) {
        throw std::invalid_argument("Poisson's ratio must be in (-1, 0.5)");
    }
}

// Constructor taking only name and density (parameters to be set via takeDB)
LinearElastic::LinearElastic(const std::string& name, double density)
    : BaseMaterial(name, density),
      youngs_modulus_(0.0),
      poissons_ratio_(0.0) {
}

void LinearElastic::takeDB(std::shared_ptr<DataBase> db) {
    if (!db) {
        throw std::invalid_argument("Database pointer is null");
    }

    // Read material parameters from database
    // Expected keys: youngs_modulus, poissons_ratio
    if (db->ifExist("youngs_modulus")) {
        youngs_modulus_ = db->getDouble("youngs_modulus");
    } else if (db->ifExist("E")) {
        youngs_modulus_ = db->getDouble("E");
    } else {
        throw std::runtime_error("LinearElastic: youngs_modulus or E not found in database");
    }

    if (db->ifExist("poissons_ratio")) {
        poissons_ratio_ = db->getDouble("poissons_ratio");
    } else if (db->ifExist("nu")) {
        poissons_ratio_ = db->getDouble("nu");
    } else {
        throw std::runtime_error("LinearElastic: poissons_ratio or nu not found in database");
    }

    // Optional: read density from database if not set in constructor
    if (db->ifExist("density")) {
        setDensity(db->getDouble("density"));
    }

    // Validate parameters
    if (youngs_modulus_ <= 0.0) {
        throw std::invalid_argument("Young's modulus must be positive");
    }
    if (poissons_ratio_ <= -1.0 || poissons_ratio_ >= 0.5) {
        throw std::invalid_argument("Poisson's ratio must be in (-1, 0.5)");
    }
}

bool LinearElastic::updateStress(const Eigen::VectorXd& strain_inc,
                                 Eigen::VectorXd& stress,
                                 Eigen::VectorXd& strain,
                                 std::vector<double>& state_vars) {
    // Linear elastic stress update: Δσ = D : Δε
    // stress and strain are updated in-place

    // Check dimensions
    int strain_size = strain_inc.size();
    if (stress.size() != strain_size || strain.size() != strain_size) {
        std::cerr << "LinearElastic::updateStress: dimension mismatch" << std::endl;
        return false;
    }

    // Get elasticity matrix
    Eigen::MatrixXd D = getDSDu();

    // Update strain
    strain += strain_inc;

    // Update stress: σ_new = σ_old + D * Δε
    stress += D * strain_inc;

    return true;
}

Eigen::MatrixXd LinearElastic::getDSDu() const {
    // Return elasticity matrix based on problem dimension
    // Default to 3D (6x6)
    return computeElasticityMatrix3D();
}

Eigen::VectorXd LinearElastic::computeStress(const Eigen::VectorXd& strain) const {
    // σ = D : ε
    Eigen::MatrixXd D = getDSDu();
    if (strain.size() != D.rows()) {
        throw std::invalid_argument("Strain vector size does not match elasticity matrix dimension");
    }
    return D * strain;
}

Eigen::MatrixXd LinearElastic::computeTangentStiffnessMatrix() const {
    // For linear elastic, tangent stiffness matrix is the same as elasticity matrix
    return getDSDu();
}

void LinearElastic::initializeStateVariables(std::vector<double>& state_vars) const {
    // Linear elastic has no state variables, just clear the vector
    state_vars.clear();
}

int LinearElastic::getProblemDimension(int strain_size) const {
    switch (strain_size) {
        case 3:  // 2D plane stress/strain
            return 2;
        case 4:  // Axisymmetric? (not implemented)
            return 2;
        case 6:  // 3D
            return 3;
        default:
            throw std::invalid_argument("Unknown strain vector size");
    }
}

Eigen::MatrixXd LinearElastic::computeElasticityMatrix3D() const {
    // 3D elasticity matrix (6x6) in Voigt notation
    // Order: ε_xx, ε_yy, ε_zz, γ_xy, γ_yz, γ_zx
    double E = youngs_modulus_;
    double nu = poissons_ratio_;
    double lambda = E * nu / ((1 + nu) * (1 - 2 * nu));
    double mu = E / (2 * (1 + nu));

    Eigen::MatrixXd D = Eigen::MatrixXd::Zero(6, 6);
    D(0,0) = D(1,1) = D(2,2) = lambda + 2*mu;
    D(0,1) = D(0,2) = D(1,0) = D(1,2) = D(2,0) = D(2,1) = lambda;
    D(3,3) = D(4,4) = D(5,5) = mu;

    return D;
}

Eigen::MatrixXd LinearElastic::computeElasticityMatrix2DPlaneStress() const {
    // 2D plane stress elasticity matrix (3x3)
    // Order: ε_xx, ε_yy, γ_xy
    double E = youngs_modulus_;
    double nu = poissons_ratio_;
    double factor = E / (1 - nu * nu);

    Eigen::MatrixXd D = Eigen::MatrixXd::Zero(3, 3);
    D(0,0) = D(1,1) = factor;
    D(0,1) = D(1,0) = factor * nu;
    D(2,2) = factor * (1 - nu) / 2;

    return D;
}

Eigen::MatrixXd LinearElastic::computeElasticityMatrix2DPlaneStrain() const {
    // 2D plane strain elasticity matrix (3x3)
    // Order: ε_xx, ε_yy, γ_xy
    double E = youngs_modulus_;
    double nu = poissons_ratio_;
    double lambda = E * nu / ((1 + nu) * (1 - 2 * nu));
    double mu = E / (2 * (1 + nu));

    Eigen::MatrixXd D = Eigen::MatrixXd::Zero(3, 3);
    D(0,0) = D(1,1) = lambda + 2*mu;
    D(0,1) = D(1,0) = lambda;
    D(2,2) = mu;

    return D;
}