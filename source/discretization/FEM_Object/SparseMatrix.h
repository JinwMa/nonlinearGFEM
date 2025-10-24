#ifndef GFEMSPARSEMATRIX_H
#define GFEMSPARSEMATRIX_H


#include <string>
#include <iostream>
#include <vector>
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <omp.h>
#include <chrono>
#include <sstream>
#include <map>
#include <unordered_set>
#include <thread>
#include <Eigen/IterativeLinearSolvers>

/**
 * @class SparseMatrix
 * @brief A wrapper class for Eigen sparse matrices with additional functionality
 * 
 * This class provides a convenient interface for working with sparse matrices
 * using Eigen as the backend, with support for various operations and solvers.
 */
class SparseMatrix
{
public:
    // Matrix types
    using EigenSparseMatrix = Eigen::SparseMatrix<double>;
    using EigenTriplet = Eigen::Triplet<double>;
    
    /**
     * @brief Default constructor
     */
    SparseMatrix() = default;
    
    /**
     * @brief Constructor with matrix dimensions
     * @param rows Number of rows
     * @param cols Number of columns
     * @param name Optional matrix name
     */
    explicit SparseMatrix(int rows, int cols, const std::string& name = "");
    
    /**
     * @brief Constructor from Eigen sparse matrix
     * @param matrix Eigen sparse matrix
     * @param name Optional matrix name
     */
    explicit SparseMatrix(const EigenSparseMatrix& matrix, const std::string& name = "");
    
    /**
     * @brief Copy constructor
     */
    SparseMatrix(const SparseMatrix& other);
    
    /**
     * @brief Move constructor
     */
    SparseMatrix(SparseMatrix&& other) noexcept;
    
    /**
     * @brief Destructor
     */
    virtual ~SparseMatrix() = default;
    
    /**
     * @brief Copy assignment operator
     */
    SparseMatrix& operator=(const SparseMatrix& other);
    
    /**
     * @brief Move assignment operator
     */
    SparseMatrix& operator=(SparseMatrix&& other) noexcept;
    
    // Basic matrix operations
    /**
     * @brief Set matrix element
     * @param row Row index
     * @param col Column index
     * @param value Value to set
     */
    void set(int row, int col, double value);
    
    /**
     * @brief Get matrix element
     * @param row Row index
     * @param col Column index
     * @return Element value
     */
    double get(int row, int col) const;
    
    /**
     * @brief Add value to matrix element
     * @param row Row index
     * @param col Column index
     * @param value Value to add
     */
    void add(int row, int col, double value);
    
    // Matrix assembly
    /**
     * @brief Assemble matrix from triplets
     * @param triplets Vector of triplets (i, j, value)
     */
    void assembleFromTriplets(const std::vector<EigenTriplet>& triplets);
    
    /**
     * @brief Reserve space for non-zero elements
     * @param nonzeros Estimated number of non-zero elements
     */
    void reserve(int nonzeros);
    
    // Matrix properties
    /**
     * @brief Get number of rows
     * @return Number of rows
     */
    int rows() const { return d_matrix.rows(); }
    
    /**
     * @brief Get number of columns
     * @return Number of columns
     */
    int cols() const { return d_matrix.cols(); }
    
    /**
     * @brief Get number of non-zero elements
     * @return Number of non-zero elements
     */
    int nonZeros() const { return d_matrix.nonZeros(); }
    
    /**
     * @brief Check if matrix is square
     * @return True if matrix is square
     */
    bool isSquare() const { return d_matrix.rows() == d_matrix.cols(); }
    
    /**
     * @brief Check if matrix is empty
     * @return True if matrix is empty
     */
    bool isEmpty() const { return d_matrix.size() == 0; }
    
    // Matrix operations
    /**
     * @brief Matrix-vector multiplication: y = A * x
     * @param x Input vector
     * @return Result vector
     */
    Eigen::VectorXd operator*(const Eigen::VectorXd& x) const;
    
    /**
     * @brief Matrix addition
     * @param other Other matrix
     * @return Sum of matrices
     */
    SparseMatrix operator+(const SparseMatrix& other) const;
    
    /**
     * @brief Matrix subtraction
     * @param other Other matrix
     * @return Difference of matrices
     */
    SparseMatrix operator-(const SparseMatrix& other) const;
    
    /**
     * @brief Matrix multiplication
     * @param other Other matrix
     * @return Product of matrices
     */
    SparseMatrix operator*(const SparseMatrix& other) const;
    
    /**
     * @brief Scalar multiplication
     * @param scalar Scalar value
     * @return Scaled matrix
     */
    SparseMatrix operator*(double scalar) const;
    
    // Matrix transformations
    /**
     * @brief Transpose the matrix
     * @return Transposed matrix
     */
    SparseMatrix transpose() const;
    
    /**
     * @brief Get conjugate transpose
     * @return Conjugate transposed matrix
     */
    SparseMatrix adjoint() const;
    
    // Solver interface
    /**
     * @brief Solve linear system A*x = b using Pardiso
     * @param b Right-hand side vector
     * @return Solution vector
     */
    Eigen::VectorXd solve(const Eigen::VectorXd& b) const;
    
    /**
     * @brief Solve linear system with multiple right-hand sides
     * @param b Right-hand side matrix
     * @return Solution matrix
     */
    Eigen::MatrixXd solve(const Eigen::MatrixXd& b) const;
    
    // I/O operations
    /**
     * @brief Save matrix to file in Matrix Market format
     * @param filename Output filename
     */
    void saveToFile(const std::string& filename) const;
    
    /**
     * @brief Load matrix from file in Matrix Market format
     * @param filename Input filename
     */
    void loadFromFile(const std::string& filename);
    
    /**
     * @brief Print matrix information
     */
    void printInfo() const;
    
    /**
     * @brief Print matrix contents (for small matrices)
     */
    void printMatrix() const;
    
    // Accessors
    /**
     * @brief Get matrix name
     * @return Matrix name
     */
    const std::string& name() const { return d_name; }
    
    /**
     * @brief Set matrix name
     * @param name New matrix name
     */
    void setName(const std::string& name) { d_name = name; }
    
    /**
     * @brief Get reference to underlying Eigen matrix
     * @return Reference to Eigen sparse matrix
     */
    EigenSparseMatrix& matrix() { return d_matrix; }
    
    /**
     * @brief Get const reference to underlying Eigen matrix
     * @return Const reference to Eigen sparse matrix
     */
    const EigenSparseMatrix& matrix() const { return d_matrix; }
    
    // Utility functions
    /**
     * @brief Set matrix to identity
     */
    void setIdentity();
    
    /**
     * @brief Set matrix to zero
     */
    void setZero();
    
    /**
     * @brief Make compressed format (required before some operations)
     */
    void makeCompressed() { d_matrix.makeCompressed(); }
    
    /**
     * @brief Check if matrix is compressed
     * @return True if matrix is in compressed format
     */
    bool isCompressed() const { return d_matrix.isCompressed(); }

private:
    std::string d_name;
    EigenSparseMatrix d_matrix;
    
    // Pardiso solver (mutable for const solve methods)
    mutable Eigen::PardisoLU<EigenSparseMatrix> d_solver;
    mutable bool d_solver_initialized = false;
    
    /**
     * @brief Initialize solver if not already initialized
     */
    void initializeSolver() const;
};

// Free functions
/**
 * @brief Scalar multiplication from left
 */
SparseMatrix operator*(double scalar, const SparseMatrix& matrix);

/**
 * @brief Output stream operator
 */
std::ostream& operator<<(std::ostream& os, const SparseMatrix& matrix);

#endif // GFEMSPARSEMATRIX_H