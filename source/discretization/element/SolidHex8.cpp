#include "SolidHex8.h"
#include "BaseMaterial.h"
#include <Eigen/Dense>
#include <iostream>
#include <stdexcept>

// 2x2x2 Gauss integration points and weights for hexahedron
static const int NUM_GAUSS_POINTS = 8;
static const double GAUSS_POINTS[8][3] = {
    {-0.577350269189626, -0.577350269189626, -0.577350269189626},
    { 0.577350269189626, -0.577350269189626, -0.577350269189626},
    { 0.577350269189626,  0.577350269189626, -0.577350269189626},
    {-0.577350269189626,  0.577350269189626, -0.577350269189626},
    {-0.577350269189626, -0.577350269189626,  0.577350269189626},
    { 0.577350269189626, -0.577350269189626,  0.577350269189626},
    { 0.577350269189626,  0.577350269189626,  0.577350269189626},
    {-0.577350269189626,  0.577350269189626,  0.577350269189626}
};
static const double GAUSS_WEIGHTS[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

SolidHex8::SolidHex8()
{
    // 初始化8个高斯积分点 (2x2x2)
    d_integrationPoints.resize(NUM_GAUSS_POINTS);
    for (int i = 0; i < NUM_GAUSS_POINTS; ++i) {
        IntegrationPoint& ip = d_integrationPoints[i];
        ip.naturalCoords.assign({GAUSS_POINTS[i][0], GAUSS_POINTS[i][1], GAUSS_POINTS[i][2]});
        ip.weight = GAUSS_WEIGHTS[i];
        ip.jacobian = 0.0; // 将在实际计算中设置
    }
}

void SolidHex8::createDofsInElement(int element_id, std::vector<int> nodes, std::vector<Dof> & dofs)
{
    // std::cout << element_id << std::endl;
    dofs.clear();
    int nodeSize = nodes.size();
    int dofSize = d_node_dofTag.size();
    dofs.resize(nodeSize * dofSize);
    for (int i = 0; i < nodeSize; i++)
    {
        int node_id = nodes[i];
        for (int j = 0; j < dofSize; j++)
        {
            auto dofTag = d_node_dofTag[j];
            dofs[i * dofSize + j].dofTag = dofTag;
            dofs[i * dofSize + j].entity_type = Entity::EntityType::NODE;
            dofs[i * dofSize + j].entity_id = node_id;
        }
    }
}

// Shape functions for 8-node hexahedron at natural coordinates (xi, eta, zeta)
static void hex8ShapeFunctions(double xi, double eta, double zeta, double N[8]) {
    N[0] = 0.125 * (1 - xi) * (1 - eta) * (1 - zeta);
    N[1] = 0.125 * (1 + xi) * (1 - eta) * (1 - zeta);
    N[2] = 0.125 * (1 + xi) * (1 + eta) * (1 - zeta);
    N[3] = 0.125 * (1 - xi) * (1 + eta) * (1 - zeta);
    N[4] = 0.125 * (1 - xi) * (1 - eta) * (1 + zeta);
    N[5] = 0.125 * (1 + xi) * (1 - eta) * (1 + zeta);
    N[6] = 0.125 * (1 + xi) * (1 + eta) * (1 + zeta);
    N[7] = 0.125 * (1 - xi) * (1 + eta) * (1 + zeta);
}

// Derivatives of shape functions with respect to natural coordinates
static void hex8ShapeFunctionDerivatives(double xi, double eta, double zeta, double dNdxi[8][3]) {
    // dN/dxi
    dNdxi[0][0] = -0.125 * (1 - eta) * (1 - zeta);
    dNdxi[1][0] =  0.125 * (1 - eta) * (1 - zeta);
    dNdxi[2][0] =  0.125 * (1 + eta) * (1 - zeta);
    dNdxi[3][0] = -0.125 * (1 + eta) * (1 - zeta);
    dNdxi[4][0] = -0.125 * (1 - eta) * (1 + zeta);
    dNdxi[5][0] =  0.125 * (1 - eta) * (1 + zeta);
    dNdxi[6][0] =  0.125 * (1 + eta) * (1 + zeta);
    dNdxi[7][0] = -0.125 * (1 + eta) * (1 + zeta);

    // dN/deta
    dNdxi[0][1] = -0.125 * (1 - xi) * (1 - zeta);
    dNdxi[1][1] = -0.125 * (1 + xi) * (1 - zeta);
    dNdxi[2][1] =  0.125 * (1 + xi) * (1 - zeta);
    dNdxi[3][1] =  0.125 * (1 - xi) * (1 - zeta);
    dNdxi[4][1] = -0.125 * (1 - xi) * (1 + zeta);
    dNdxi[5][1] = -0.125 * (1 + xi) * (1 + zeta);
    dNdxi[6][1] =  0.125 * (1 + xi) * (1 + zeta);
    dNdxi[7][1] =  0.125 * (1 - xi) * (1 + zeta);

    // dN/dzeta
    dNdxi[0][2] = -0.125 * (1 - xi) * (1 - eta);
    dNdxi[1][2] = -0.125 * (1 + xi) * (1 - eta);
    dNdxi[2][2] = -0.125 * (1 + xi) * (1 + eta);
    dNdxi[3][2] = -0.125 * (1 - xi) * (1 + eta);
    dNdxi[4][2] =  0.125 * (1 - xi) * (1 - eta);
    dNdxi[5][2] =  0.125 * (1 + xi) * (1 - eta);
    dNdxi[6][2] =  0.125 * (1 + xi) * (1 + eta);
    dNdxi[7][2] =  0.125 * (1 - xi) * (1 + eta);
}

// Compute Jacobian matrix (3x3) and its inverse
static bool computeJacobian(const double coords[24], // 8 nodes * 3 coordinates
                           const double dNdxi[8][3],
                           double J[3][3],
                           double& detJ) {
    // Initialize Jacobian to zero
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            J[i][j] = 0.0;
        }
    }

    // J = sum(dNdxi_i * x_i)
    for (int node = 0; node < 8; node++) {
        double x = coords[node*3];
        double y = coords[node*3 + 1];
        double z = coords[node*3 + 2];

        J[0][0] += dNdxi[node][0] * x;
        J[0][1] += dNdxi[node][0] * y;
        J[0][2] += dNdxi[node][0] * z;

        J[1][0] += dNdxi[node][1] * x;
        J[1][1] += dNdxi[node][1] * y;
        J[1][2] += dNdxi[node][1] * z;

        J[2][0] += dNdxi[node][2] * x;
        J[2][1] += dNdxi[node][2] * y;
        J[2][2] += dNdxi[node][2] * z;
    }

    // Compute determinant
    detJ = J[0][0] * (J[1][1]*J[2][2] - J[2][1]*J[1][2])
         - J[0][1] * (J[1][0]*J[2][2] - J[2][0]*J[1][2])
         + J[0][2] * (J[1][0]*J[2][1] - J[2][0]*J[1][1]);

    if (detJ <= 1e-12) {
        return false; // Singular or near-singular Jacobian
    }

    return true;
}

// Compute B matrix (6x24) for 3D solid element
static void computeBMatrix(const double dNdx[8][3], double B[6][24]) {
    // Initialize B matrix to zero
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 24; j++) {
            B[i][j] = 0.0;
        }
    }

    // Fill B matrix (Voigt notation: ε_xx, ε_yy, ε_zz, γ_xy, γ_yz, γ_zx)
    for (int node = 0; node < 8; node++) {
        int col = node * 3;
        // ε_xx = dN/dx * u_x
        B[0][col]     = dNdx[node][0];
        // ε_yy = dN/dy * u_y
        B[1][col + 1] = dNdx[node][1];
        // ε_zz = dN/dz * u_z
        B[2][col + 2] = dNdx[node][2];
        // γ_xy = dN/dy * u_x + dN/dx * u_y
        B[3][col]     = dNdx[node][1];
        B[3][col + 1] = dNdx[node][0];
        // γ_yz = dN/dz * u_y + dN/dy * u_z
        B[4][col + 1] = dNdx[node][2];
        B[4][col + 2] = dNdx[node][1];
        // γ_zx = dN/dx * u_z + dN/dz * u_x
        B[5][col]     = dNdx[node][2];
        B[5][col + 2] = dNdx[node][0];
    }
}


void SolidHex8::ComputeStiffness(int elementId,
                                 ElementInfoPack * pack,
                                 Matrix<double> & K)
{
    // Check if material is set
    if (!hasMaterial()) {
        throw std::runtime_error("SolidHex8::ComputeStiffness: Material not set for element " + std::to_string(elementId));
    }

    auto material = getMaterial();

    // Get elasticity matrix from material
    Eigen::MatrixXd D_eigen = material->getDSDu();

    // Convert to plain array for compatibility
    double D[6][6];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            D[i][j] = D_eigen(i, j);
        }
    }

    // Get node coordinates from pack
    // pack->coord should contain 8 nodes * 3 coordinates = 24 values
    const double* coords = pack->coord.data();

    // Initialize stiffness matrix to zero
    K = Matrix<double>(24, 24); // 8 nodes * 3 dofs per node
    for (int i = 0; i < 24; i++) {
        for (int j = 0; j < 24; j++) {
            K(i, j) = 0.0;
        }
    }

    // Gauss integration
    for (int gp = 0; gp < NUM_GAUSS_POINTS; gp++) {
        double xi = GAUSS_POINTS[gp][0];
        double eta = GAUSS_POINTS[gp][1];
        double zeta = GAUSS_POINTS[gp][2];
        double weight = GAUSS_WEIGHTS[gp];

        // Shape function derivatives in natural coordinates
        double dNdxi[8][3];
        hex8ShapeFunctionDerivatives(xi, eta, zeta, dNdxi);

        // Compute Jacobian
        double J[3][3], invJ[3][3];
        double detJ;
        if (!computeJacobian(coords, dNdxi, J, detJ)) {
            throw std::runtime_error("SolidHex8::ComputeStiffness: Singular Jacobian at Gauss point");
        }

        // Compute inverse of Jacobian
        toolbox::invertMatrix(J, invJ);

        // Transform derivatives to global coordinates: dNdx = invJ * dNdxi
        double dNdx[8][3];
        for (int node = 0; node < 8; node++) {
            for (int i = 0; i < 3; i++) {
                dNdx[node][i] = 0.0;
                for (int j = 0; j < 3; j++) {
                    dNdx[node][i] += invJ[i][j] * dNdxi[node][j];
                }
            }
        }

        // Compute B matrix (6x24)
        double B[6][24];
        computeBMatrix(dNdx, B);

        // Compute B^T * D * B * detJ * weight
        double temp[24][6]; // B^T * D
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 6; j++) {
                temp[i][j] = 0.0;
                for (int k = 0; k < 6; k++) {
                    temp[i][j] += B[k][i] * D[k][j]; // Note: B[k][i] because B is 6x24
                }
            }
        }

        // Add to stiffness matrix: K += temp * B * detJ * weight
        double factor = detJ * weight;
        for (int i = 0; i < 24; i++) {
            for (int j = 0; j < 24; j++) {
                double sum = 0.0;
                for (int k = 0; k < 6; k++) {
                    sum += temp[i][k] * B[k][j];
                }
                K(i, j) += sum * factor;
            }
        }
    }
}

void SolidHex8::ComputeInternalForce(int elementId,
                                     ElementInfoPack * pack,
                                     std::vector<double> & fint)
{
    // Check if material is set
    if (!hasMaterial()) {
        throw std::runtime_error("SolidHex8::ComputeInternalForce: Material not set for element " + std::to_string(elementId));
    }

    auto material = getMaterial();

    // For linear elastic material, internal force = K * u
    // But for nonlinear materials, we need to compute stress at integration points
    // This is a simplified implementation for linear elasticity

    // Get node displacements from pack
    // pack->disp should contain 8 nodes * 3 dofs = 24 values
    const double* displacements = pack->disp.data();

    // Get node coordinates
    const double* coords = pack->coord.data();

    // Initialize internal force vector to zero
    fint.resize(24);
    for (int i = 0; i < 24; i++) {
        fint[i] = 0.0;
    }

    // Get elasticity matrix
    Eigen::MatrixXd D_eigen = material->getDSDu();
    double D[6][6];
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            D[i][j] = D_eigen(i, j);
        }
    }

    // Gauss integration
    for (int gp = 0; gp < NUM_GAUSS_POINTS; gp++) {
        double xi = GAUSS_POINTS[gp][0];
        double eta = GAUSS_POINTS[gp][1];
        double zeta = GAUSS_POINTS[gp][2];
        double weight = GAUSS_WEIGHTS[gp];

        // Shape function derivatives in natural coordinates
        double dNdxi[8][3];
        hex8ShapeFunctionDerivatives(xi, eta, zeta, dNdxi);

        // Compute Jacobian
        double J[3][3], invJ[3][3];
        double detJ;
        if (!computeJacobian(coords, dNdxi, J, detJ)) {
            throw std::runtime_error("SolidHex8::ComputeInternalForce: Singular Jacobian at Gauss point");
        }

        // Compute inverse of Jacobian
        toolbox::invertMatrix(J, invJ);

        // Transform derivatives to global coordinates
        double dNdx[8][3];
        for (int node = 0; node < 8; node++) {
            for (int i = 0; i < 3; i++) {
                dNdx[node][i] = 0.0;
                for (int j = 0; j < 3; j++) {
                    dNdx[node][i] += invJ[i][j] * dNdxi[node][j];
                }
            }
        }

        // Compute B matrix (6x24)
        double B[6][24];
        computeBMatrix(dNdx, B);

        // Compute strain at Gauss point: ε = B * u
        double strain[6] = {0.0};
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 24; j++) {
                strain[i] += B[i][j] * displacements[j];
            }
        }

        // Compute stress at Gauss point: σ = D * ε
        double stress[6] = {0.0};
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 6; j++) {
                stress[i] += D[i][j] * strain[j];
            }
        }

        // Compute internal force contribution: f_int += B^T * σ * detJ * weight
        double factor = detJ * weight;
        for (int i = 0; i < 24; i++) {
            double sum = 0.0;
            for (int j = 0; j < 6; j++) {
                sum += B[j][i] * stress[j]; // B^T is transpose of B
            }
            fint[i] += sum * factor;
        }
    }
}

// 形状函数相关接口实现
void SolidHex8::evaluateShapeFunctions(const std::vector<double>& naturalCoords,
                                       std::vector<double>& shapeFunctions) const
{
    if (naturalCoords.size() < 3) {
        throw std::runtime_error("SolidHex8::evaluateShapeFunctions: naturalCoords must have at least 3 components");
    }
    double xi = naturalCoords[0];
    double eta = naturalCoords[1];
    double zeta = naturalCoords[2];

    shapeFunctions.resize(8);
    hex8ShapeFunctions(xi, eta, zeta, shapeFunctions.data());
}

void SolidHex8::evaluateShapeFunctionDerivatives(const std::vector<double>& naturalCoords,
                                                 std::vector<std::vector<double>>& derivatives) const
{
    if (naturalCoords.size() < 3) {
        throw std::runtime_error("SolidHex8::evaluateShapeFunctionDerivatives: naturalCoords must have at least 3 components");
    }
    double xi = naturalCoords[0];
    double eta = naturalCoords[1];
    double zeta = naturalCoords[2];

    double dNdxi[8][3];
    hex8ShapeFunctionDerivatives(xi, eta, zeta, dNdxi);

    derivatives.resize(8);
    for (int i = 0; i < 8; ++i) {
        derivatives[i].resize(3);
        derivatives[i][0] = dNdxi[i][0];
        derivatives[i][1] = dNdxi[i][1];
        derivatives[i][2] = dNdxi[i][2];
    }
}

const std::vector<IntegrationPoint>& SolidHex8::getIntegrationPoints() const
{
    return d_integrationPoints;
}

ElementDimension SolidHex8::getElementDimension() const
{
    // SolidHex8: 3D, 8节点, 每个节点3个自由度, 8个积分点, 6个应力分量
    return ElementDimension(3, 8, 3, 8, 6);
}

std::string SolidHex8::getElementTypeName() const
{
    return "SolidHex8";
}