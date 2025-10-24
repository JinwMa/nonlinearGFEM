#ifndef GFEMSPARSEVECTOR_H
#define GFEMSPARSEVECTOR_H

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

/**
 * @class SparseVector
 * @brief A wrapper class for Eigen sparse vectors with additional functionality
 * 
 * This class provides a convenient interface for working with sparse vectors
 * using Eigen as the backend, with support for various operations and utilities.
 */
class SparseVector
{
public:
    // Vector types
    using EigenSparseVector = Eigen::SparseVector<double>;
    using EigenDenseVector = Eigen::VectorXd;
    using ValueType = double;
    
    /**
     * @brief Default constructor
     */
    SparseVector() = default;
    
    /**
     * @brief Constructor with vector size
     * @param size Vector size
     * @param name Optional vector name
     */
    explicit SparseVector(int size, const std::string& name = "");
    
    /**
     * @brief Constructor from Eigen sparse vector
     * @param vector Eigen sparse vector
     * @param name Optional vector name
     */
    explicit SparseVector(const EigenSparseVector& vector, const std::string& name = "");
    
    /**
     * @brief Constructor from dense vector
     * @param denseVector Dense vector
     * @param name Optional vector name
     */
    explicit SparseVector(const EigenDenseVector& denseVector, const std::string& name = "");
    
    /**
     * @brief Constructor from initializer list
     * @param initList Initializer list of values
     * @param name Optional vector name
     */
    SparseVector(std::initializer_list<double> initList, const std::string& name = "");
    
    /**
     * @brief Copy constructor
     */
    SparseVector(const SparseVector& other);
    
    /**
     * @brief Move constructor
     */
    SparseVector(SparseVector&& other) noexcept;
    
    /**
     * @brief Destructor
     */
    virtual ~SparseVector() = default;
    
    /**
     * @brief Copy assignment operator
     */
    SparseVector& operator=(const SparseVector& other);
    
    /**
     * @brief Move assignment operator
     */
    SparseVector& operator=(SparseVector&& other) noexcept;
    
    /**
     * @brief Assignment from Eigen sparse vector
     */
    SparseVector& operator=(const EigenSparseVector& other);
    
    /**
     * @brief Assignment from dense vector
     */
    SparseVector& operator=(const EigenDenseVector& other);
    
    // Basic vector operations
    /**
     * @brief Set vector element
     * @param index Element index
     * @param value Value to set
     */
    void set(int index, double value);
    
    /**
     * @brief Get vector element
     * @param index Element index
     * @return Element value
     */
    double get(int index) const;
    
    /**
     * @brief Add value to vector element
     * @param index Element index
     * @param value Value to add
     */
    void add(int index, double value);
    
    /**
     * @brief Insert value into vector (if zero, might not actually insert due to sparsity)
     * @param index Element index
     * @param value Value to insert
     */
    void insert(int index, double value);
    
    // Vector assembly
    /**
     * @brief Reserve space for non-zero elements
     * @param nonzeros Estimated number of non-zero elements
     */
    void reserve(int nonzeros);
    
    /**
     * @brief Assemble vector from indices and values
     * @param indices Vector of indices
     * @param values Vector of values
     */
    void assembleFromPairs(const std::vector<int>& indices, const std::vector<double>& values);
    
    // Vector properties
    /**
     * @brief Get vector size
     * @return Vector size
     */
    int size() const { return d_vector.size(); }
    
    /**
     * @brief Get number of non-zero elements
     * @return Number of non-zero elements
     */
    int nonZeros() const { return d_vector.nonZeros(); }
    
    /**
     * @brief Check if vector is empty
     * @return True if vector is empty
     */
    bool isEmpty() const { return d_vector.size() == 0; }
    
    /**
     * @brief Get sparsity ratio (non-zeros / total size)
     * @return Sparsity ratio
     */
    double sparsity() const;
    
    // Vector operations
    /**
     * @brief Dot product with another sparse vector
     * @param other Other vector
     * @return Dot product result
     */
    double dot(const SparseVector& other) const;
    
    /**
     * @brief Dot product with dense vector
     * @param other Dense vector
     * @return Dot product result
     */
    double dot(const EigenDenseVector& other) const;
    
    /**
     * @brief Vector addition
     * @param other Other vector
     * @return Sum of vectors
     */
    SparseVector operator+(const SparseVector& other) const;
    
    /**
     * @brief Vector subtraction
     * @param other Other vector
     * @return Difference of vectors
     */
    SparseVector operator-(const SparseVector& other) const;
    
    /**
     * @brief Scalar multiplication
     * @param scalar Scalar value
     * @return Scaled vector
     */
    SparseVector operator*(double scalar) const;
    
    /**
     * @brief Scalar division
     * @param scalar Scalar value
     * @return Scaled vector
     */
    SparseVector operator/(double scalar) const;
    
    /**
     * @brief Compound addition
     */
    SparseVector& operator+=(const SparseVector& other);
    
    /**
     * @brief Compound subtraction
     */
    SparseVector& operator-=(const SparseVector& other);
    
    /**
     * @brief Compound scalar multiplication
     */
    SparseVector& operator*=(double scalar);
    
    /**
     * @brief Compound scalar division
     */
    SparseVector& operator/=(double scalar);
    
    // Norm operations
    /**
     * @brief Compute L2 norm
     * @return L2 norm
     */
    double norm() const;
    
    /**
     * @brief Compute squared L2 norm
     * @return Squared L2 norm
     */
    double squaredNorm() const;
    
    /**
     * @brief Compute L1 norm
     * @return L1 norm
     */
    double l1Norm() const;
    
    /**
     * @brief Compute Linf norm (maximum absolute value)
     * @return Linf norm
     */
    double linfNorm() const;
    
    /**
     * @brief Normalize the vector (L2 norm)
     * @return Reference to this vector
     */
    SparseVector& normalize();
    
    // Conversion operations
    /**
     * @brief Convert to dense vector
     * @return Dense vector
     */
    EigenDenseVector toDense() const;
    
    /**
     * @brief Get non-zero indices
     * @return Vector of non-zero indices
     */
    std::vector<int> getNonZeroIndices() const;
    
    /**
     * @brief Get non-zero values
     * @return Vector of non-zero values
     */
    std::vector<double> getNonZeroValues() const;
    
    /**
     * @brief Get non-zero pairs (index, value)
     * @return Vector of non-zero pairs
     */
    std::vector<std::pair<int, double>> getNonZeroPairs() const;
    
    // Utility functions
    /**
     * @brief Set vector to zero
     */
    void setZero();
    
    /**
     * @brief Make compressed format (for SparseVector this is a no-op as it's always compressed)
     */
    void makeCompressed() { /* No-op for SparseVector, it's always compressed */ }
    
    /**
     * @brief Check if vector is compressed
     * @return True if vector is in compressed format (always true for SparseVector)
     */
    bool isCompressed() const { return true; }  // SparseVector is always compressed
    
    /**
     * @brief Prune small elements below threshold
     * @param threshold Threshold value
     * @param reference Reference value for relative threshold (optional)
     */
    void prune(double threshold, double reference = 1.0);
    
    /**
     * @brief Finalize the vector (prepare for efficient operations)
     * This is equivalent to makeCompressed() for matrices
     */
    // void finalize() { d_vector.makeCompressed(); }  // Actually call makeCompressed on the underlying vector
    
    // I/O operations
    /**
     * @brief Save vector to file
     * @param filename Output filename
     */
    void saveToFile(const std::string& filename) const;
    
    /**
     * @brief Load vector from file
     * @param filename Input filename
     */
    void loadFromFile(const std::string& filename);
    
    /**
     * @brief Print vector information
     */
    void printInfo() const;
    
    /**
     * @brief Print vector contents (for small vectors)
     */
    void printVector() const;
    
    // Accessors
    /**
     * @brief Get vector name
     * @return Vector name
     */
    const std::string& name() const { return d_name; }
    
    /**
     * @brief Set vector name
     * @param name New vector name
     */
    void setName(const std::string& name) { d_name = name; }
    
    /**
     * @brief Get reference to underlying Eigen vector
     * @return Reference to Eigen sparse vector
     */
    EigenSparseVector& vector() { return d_vector; }
    
    /**
     * @brief Get const reference to underlying Eigen vector
     * @return Const reference to Eigen sparse vector
     */
    const EigenSparseVector& vector() const { return d_vector; }
    
    // Iterator support (for non-zero elements)
    /**
     * @brief Get iterator to first non-zero element
     */
    EigenSparseVector::InnerIterator begin() { return EigenSparseVector::InnerIterator(d_vector); }
    
    /**
     * @brief Get const iterator to first non-zero element
     */
    EigenSparseVector::InnerIterator begin() const { return EigenSparseVector::InnerIterator(d_vector); }
    
    /**
     * @brief Get iterator to end of non-zero elements
     */
    EigenSparseVector::InnerIterator end() { return EigenSparseVector::InnerIterator(d_vector, 0); }
    
    /**
     * @brief Get const iterator to end of non-zero elements
     */
    EigenSparseVector::InnerIterator end() const { return EigenSparseVector::InnerIterator(d_vector, 0); }

private:
    std::string d_name;
    EigenSparseVector d_vector;
};

// Free functions
/**
 * @brief Scalar multiplication from left
 */
SparseVector operator*(double scalar, const SparseVector& vector);

/**
 * @brief Output stream operator
 */
std::ostream& operator<<(std::ostream& os, const SparseVector& vector);

/**
 * @brief Compute outer product of two sparse vectors
 */
Eigen::SparseMatrix<double> outerProduct(const SparseVector& lhs, const SparseVector& rhs);

/**
 * @brief Concatenate two sparse vectors vertically
 */
SparseVector concatenateVertical(const SparseVector& top, const SparseVector& bottom);

/**
 * @brief Concatenate two sparse vectors horizontally (as diagonal matrix)
 */
Eigen::SparseMatrix<double> concatenateHorizontal(const SparseVector& left, const SparseVector& right);

#endif // GFEMSPARSEVECTOR_H