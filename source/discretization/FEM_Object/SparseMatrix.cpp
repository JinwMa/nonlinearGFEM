#include "SparseMatrix.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

SparseMatrix::SparseMatrix(int rows, int cols, const std::string& name)
    : d_name(name), d_matrix(rows, cols), d_solver_initialized(false)
{
    if (rows < 0 || cols < 0) {
        throw std::invalid_argument("Matrix dimensions must be non-negative");
    }
}

SparseMatrix::SparseMatrix(const EigenSparseMatrix& matrix, const std::string& name)
    : d_name(name), d_matrix(matrix), d_solver_initialized(false)
{
}

SparseMatrix::SparseMatrix(const SparseMatrix& other)
    : d_name(other.d_name), 
      d_matrix(other.d_matrix),
      d_solver_initialized(false)  // Solver not copied, will be reinitialized if needed
{
}

SparseMatrix::SparseMatrix(SparseMatrix&& other) noexcept
    : d_name(std::move(other.d_name)),
      d_matrix(std::move(other.d_matrix)),
      d_solver_initialized(other.d_solver_initialized)
{
    other.d_solver_initialized = false;
}

SparseMatrix& SparseMatrix::operator=(const SparseMatrix& other)
{
    if (this != &other) {
        d_name = other.d_name;
        d_matrix = other.d_matrix;
        d_solver_initialized = false;  // Solver needs reinitialization
    }
    return *this;
}

SparseMatrix& SparseMatrix::operator=(SparseMatrix&& other) noexcept
{
    if (this != &other) {
        d_name = std::move(other.d_name);
        d_matrix = std::move(other.d_matrix);
        d_solver_initialized = other.d_solver_initialized;
        other.d_solver_initialized = false;
    }
    return *this;
}

void SparseMatrix::set(int row, int col, double value)
{
    if (row < 0 || row >= rows() || col < 0 || col >= cols()) {
        throw std::out_of_range("Matrix indices out of range");
    }
    d_matrix.coeffRef(row, col) = value;
    d_solver_initialized = false;  // Matrix changed, solver needs reinitialization
}

double SparseMatrix::get(int row, int col) const
{
    if (row < 0 || row >= rows() || col < 0 || col >= cols()) {
        throw std::out_of_range("Matrix indices out of range");
    }
    return d_matrix.coeff(row, col);
}

void SparseMatrix::add(int row, int col, double value)
{
    if (row < 0 || row >= rows() || col < 0 || col >= cols()) {
        throw std::out_of_range("Matrix indices out of range");
    }
    d_matrix.coeffRef(row, col) += value;
    d_solver_initialized = false;  // Matrix changed, solver needs reinitialization
}

void SparseMatrix::assembleFromTriplets(const std::vector<EigenTriplet>& triplets)
{
    // Check if triplets are within bounds
    for (const auto& triplet : triplets) {
        if (triplet.row() < 0 || triplet.row() >= rows() || 
            triplet.col() < 0 || triplet.col() >= cols()) {
            throw std::out_of_range("Triplet indices out of matrix bounds");
        }
    }
    
    d_matrix.setFromTriplets(triplets.begin(), triplets.end());
    d_solver_initialized = false;
}

void SparseMatrix::reserve(int nonzeros)
{
    if (nonzeros < 0) {
        throw std::invalid_argument("Number of nonzeros must be non-negative");
    }
    d_matrix.reserve(nonzeros);
}

Eigen::VectorXd SparseMatrix::operator*(const Eigen::VectorXd& x) const
{
    if (cols() != x.size()) {
        throw std::invalid_argument("Matrix and vector dimensions don't match for multiplication");
    }
    return d_matrix * x;
}

SparseMatrix SparseMatrix::operator+(const SparseMatrix& other) const
{
    if (rows() != other.rows() || cols() != other.cols()) {
        throw std::invalid_argument("Matrix dimensions don't match for addition");
    }
    return SparseMatrix(d_matrix + other.d_matrix);
}

SparseMatrix SparseMatrix::operator-(const SparseMatrix& other) const
{
    if (rows() != other.rows() || cols() != other.cols()) {
        throw std::invalid_argument("Matrix dimensions don't match for subtraction");
    }
    return SparseMatrix(d_matrix - other.d_matrix);
}

SparseMatrix SparseMatrix::operator*(const SparseMatrix& other) const
{
    if (cols() != other.rows()) {
        throw std::invalid_argument("Matrix dimensions don't match for multiplication");
    }
    return SparseMatrix(d_matrix * other.d_matrix);
}

SparseMatrix SparseMatrix::operator*(double scalar) const
{
    return SparseMatrix(d_matrix * scalar);
}

SparseMatrix SparseMatrix::transpose() const
{
    return SparseMatrix(d_matrix.transpose());
}

SparseMatrix SparseMatrix::adjoint() const
{
    return SparseMatrix(d_matrix.adjoint());
}

void SparseMatrix::initializeSolver() const
{
    if (!d_solver_initialized) {
        if (!isSquare()) {
            throw std::runtime_error("Solver requires a square matrix");
        }
        if (isEmpty()) {
            throw std::runtime_error("Cannot solve with empty matrix");
        }
        
        // Make sure matrix is compressed before solving
        if (!isCompressed()) {
            const_cast<EigenSparseMatrix&>(d_matrix).makeCompressed();
        }
        
        d_solver.compute(d_matrix);
        if (d_solver.info() != Eigen::Success) {
            throw std::runtime_error("Solver factorization failed");
        }
        d_solver_initialized = true;
    }
}

Eigen::VectorXd SparseMatrix::solve(const Eigen::VectorXd& b) const
{
    if (b.size() != rows()) {
        throw std::invalid_argument("Right-hand side vector dimension doesn't match matrix");
    }
    
    initializeSolver();
    Eigen::VectorXd x = d_solver.solve(b);
    
    if (d_solver.info() != Eigen::Success) {
        throw std::runtime_error("Linear system solve failed");
    }
    
    return x;
}

Eigen::MatrixXd SparseMatrix::solve(const Eigen::MatrixXd& b) const
{
    if (b.rows() != rows()) {
        throw std::invalid_argument("Right-hand side matrix dimension doesn't match matrix");
    }
    
    initializeSolver();
    Eigen::MatrixXd x = d_solver.solve(b);
    
    if (d_solver.info() != Eigen::Success) {
        throw std::runtime_error("Linear system solve failed");
    }
    
    return x;
}

void SparseMatrix::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        // 保存矩阵基本信息
        file << "SparseMatrix" << std::endl;
        file << d_matrix.rows() << " " << d_matrix.cols() << " " << d_matrix.nonZeros() << std::endl;
        
        // Save non-zero elements using COO format
        for (int ii = 0; ii < d_matrix.outerSize(); ++ii)
        {
            for (Eigen::SparseMatrix<double>::InnerIterator it(d_matrix, ii); it; ++it)
            {
                 file << it.row() << " " << it.col() << " " << it.value() << std::endl;
            }
        }
        file.close();
    }
}

void SparseMatrix::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string header;
        std::getline(file, header); // Read header
        
        if (header != "SparseMatrix") {
            std::cerr << "Invalid file format" << std::endl;
            return;
        }
        
        int rows, cols, nonZeros;
        file >> rows >> cols >> nonZeros;
        
        d_matrix.resize(rows, cols);
        std::vector<Eigen::Triplet<double>> triplets;
        triplets.reserve(nonZeros);
        
        for (int i = 0; i < nonZeros; ++i) {
            int row, col;
            double value;
            file >> row >> col >> value;
            triplets.emplace_back(row, col, value);
        }
        
        d_matrix.setFromTriplets(triplets.begin(), triplets.end());
        file.close();
    }
}

void SparseMatrix::printInfo() const
{
    std::cout << "Matrix: " << (d_name.empty() ? "Unnamed" : d_name) << "\n"
              << "Dimensions: " << rows() << " x " << cols() << "\n"
              << "Non-zero elements: " << nonZeros() << "\n"
              << "Sparsity: " << (100.0 * nonZeros() / (rows() * cols())) << "%\n"
              << "Compressed: " << (isCompressed() ? "Yes" : "No") << "\n"
              << "Square: " << (isSquare() ? "Yes" : "No") << "\n"
              << "Solver initialized: " << (d_solver_initialized ? "Yes" : "No") << std::endl;
}

void SparseMatrix::printMatrix() const
{
    if (rows() > 100 || cols() > 100) {
        std::cout << "Matrix too large to print (" << rows() << "x" << cols() 
                  << "). Use printInfo() for summary." << std::endl;
        return;
    }
    
    std::cout << "Matrix " << (d_name.empty() ? "Unnamed" : d_name) 
              << " (" << rows() << "x" << cols() << "):" << std::endl;
    
    for (int i = 0; i < rows(); ++i) {
        for (int j = 0; j < cols(); ++j) {
            double value = d_matrix.coeff(i, j);
            if (std::abs(value) < 1e-15) {
                std::cout << " 0     ";
            } else {
                std::cout << " " << value << " ";
            }
        }
        std::cout << std::endl;
    }
}

void SparseMatrix::setIdentity()
{
    d_matrix.setIdentity();
    d_solver_initialized = false;
}

void SparseMatrix::setZero()
{
    d_matrix.setZero();
    d_solver_initialized = false;
}

// Free functions implementation
SparseMatrix operator*(double scalar, const SparseMatrix& matrix)
{
    return matrix * scalar;
}

std::ostream& operator<<(std::ostream& os, const SparseMatrix& matrix)
{
    os << "SparseMatrix: " << (matrix.name().empty() ? "Unnamed" : matrix.name()) 
       << " (" << matrix.rows() << "x" << matrix.cols() 
       << "), nonzeros: " << matrix.nonZeros();
    return os;
}