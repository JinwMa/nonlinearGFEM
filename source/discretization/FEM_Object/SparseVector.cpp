#include "SparseVector.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

SparseVector::SparseVector(int size, const std::string& name)
    : d_name(name), d_vector(size)
{
    if (size < 0) {
        throw std::invalid_argument("Vector size must be non-negative");
    }
}

SparseVector::SparseVector(const EigenSparseVector& vector, const std::string& name)
    : d_name(name), d_vector(vector)
{
}

SparseVector::SparseVector(const EigenDenseVector& denseVector, const std::string& name)
    : d_name(name), d_vector(denseVector.sparseView())
{
}

SparseVector::SparseVector(std::initializer_list<double> initList, const std::string& name)
    : d_name(name), d_vector(static_cast<int>(initList.size()))
{
    int index = 0;
    for (double value : initList) {
        if (value != 0.0) {
            d_vector.insert(index) = value;
        }
        ++index;
    }
}

SparseVector::SparseVector(const SparseVector& other)
    : d_name(other.d_name), d_vector(other.d_vector)
{
}

SparseVector::SparseVector(SparseVector&& other) noexcept
    : d_name(std::move(other.d_name)), d_vector(std::move(other.d_vector))
{
}

SparseVector& SparseVector::operator=(const SparseVector& other)
{
    if (this != &other) {
        d_name = other.d_name;
        d_vector = other.d_vector;
    }
    return *this;
}

SparseVector& SparseVector::operator=(SparseVector&& other) noexcept
{
    if (this != &other) {
        d_name = std::move(other.d_name);
        d_vector = std::move(other.d_vector);
    }
    return *this;
}

SparseVector& SparseVector::operator=(const EigenSparseVector& other)
{
    d_vector = other;
    return *this;
}

SparseVector& SparseVector::operator=(const EigenDenseVector& other)
{
    d_vector = other.sparseView();
    return *this;
}

void SparseVector::set(int index, double value)
{
    if (index < 0 || index >= size()) {
        throw std::out_of_range("Vector index out of range");
    }
    d_vector.coeffRef(index) = value;
}

double SparseVector::get(int index) const
{
    if (index < 0 || index >= size()) {
        throw std::out_of_range("Vector index out of range");
    }
    return d_vector.coeff(index);
}

void SparseVector::add(int index, double value)
{
    if (index < 0 || index >= size()) {
        throw std::out_of_range("Vector index out of range");
    }
    d_vector.coeffRef(index) += value;
}

void SparseVector::insert(int index, double value)
{
    if (index < 0 || index >= size()) {
        throw std::out_of_range("Vector index out of range");
    }
    d_vector.insert(index) = value;
}

void SparseVector::reserve(int nonzeros)
{
    if (nonzeros < 0) {
        throw std::invalid_argument("Number of nonzeros must be non-negative");
    }
    d_vector.reserve(nonzeros);
}

void SparseVector::assembleFromPairs(const std::vector<int>& indices, const std::vector<double>& values)
{
    if (indices.size() != values.size()) {
        throw std::invalid_argument("Indices and values vectors must have same size");
    }
    
    // Clear existing non-zeros and reserve space
    d_vector.setZero();
    d_vector.reserve(static_cast<int>(indices.size()));
    
    for (size_t i = 0; i < indices.size(); ++i) {
        int index = indices[i];
        double value = values[i];
        
        if (index < 0 || index >= size()) {
            throw std::out_of_range("Index out of vector bounds");
        }
        if (value != 0.0) {
            d_vector.insert(index) = value;
        }
    }
}

double SparseVector::sparsity() const
{
    if (isEmpty()) {
        return 1.0;
    }
    return 1.0 - static_cast<double>(nonZeros()) / size();
}

double SparseVector::dot(const SparseVector& other) const
{
    if (size() != other.size()) {
        throw std::invalid_argument("Vector sizes don't match for dot product");
    }
    return d_vector.dot(other.d_vector);
}

double SparseVector::dot(const EigenDenseVector& other) const
{
    if (size() != other.size()) {
        throw std::invalid_argument("Vector sizes don't match for dot product");
    }
    return d_vector.dot(other);
}


SparseVector SparseVector::operator+(const SparseVector& other) const
{
    if (size() != other.size()) {
        throw std::invalid_argument("Vector sizes don't match for addition");
    }
    // 显式转换为 EigenSparseVector 类型
    EigenSparseVector result = d_vector + other.d_vector;
    return SparseVector(result);
}
SparseVector SparseVector::operator-(const SparseVector& other) const
{
    if (size() != other.size()) {
        throw std::invalid_argument("Vector sizes don't match for subtraction");
    }
    // 显式转换为 EigenSparseVector 类型
    EigenSparseVector result = d_vector - other.d_vector;
    return SparseVector(result);
}

SparseVector SparseVector::operator*(double scalar) const
{
    // 显式转换为 EigenSparseVector 类型
    EigenSparseVector result = d_vector * scalar;
    return SparseVector(result);
}

SparseVector SparseVector::operator/(double scalar) const
{
    if (scalar == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    // 显式转换为 EigenSparseVector 类型
    EigenSparseVector result = d_vector / scalar;
    return SparseVector(result);
}

SparseVector& SparseVector::operator+=(const SparseVector& other)
{
    if (size() != other.size()) {
        throw std::invalid_argument("Vector sizes don't match for addition");
    }
    d_vector += other.d_vector;
    return *this;
}

SparseVector& SparseVector::operator-=(const SparseVector& other)
{
    if (size() != other.size()) {
        throw std::invalid_argument("Vector sizes don't match for subtraction");
    }
    d_vector -= other.d_vector;
    return *this;
}

SparseVector& SparseVector::operator*=(double scalar)
{
    d_vector *= scalar;
    return *this;
}

SparseVector& SparseVector::operator/=(double scalar)
{
    if (scalar == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    d_vector /= scalar;
    return *this;
}

double SparseVector::norm() const
{
    return std::sqrt(squaredNorm());
}

double SparseVector::squaredNorm() const
{
    return d_vector.squaredNorm();
}

// double SparseVector::l1Norm() const
// {
//     return d_vector.lpNorm<1>();
// }

// double SparseVector::linfNorm() const
// {
//     return d_vector.lpNorm<Eigen::Infinity>();
// }

SparseVector& SparseVector::normalize()
{
    double n = norm();
    if (n > 0.0) {
        d_vector /= n;
    }
    return *this;
}

Eigen::VectorXd SparseVector::toDense() const
{
    return Eigen::VectorXd(d_vector);
}

std::vector<int> SparseVector::getNonZeroIndices() const
{
    std::vector<int> indices;
    indices.reserve(nonZeros());
    
    for (EigenSparseVector::InnerIterator it(d_vector); it; ++it) {
        indices.push_back(it.index());
    }
    
    return indices;
}

std::vector<double> SparseVector::getNonZeroValues() const
{
    std::vector<double> values;
    values.reserve(nonZeros());
    
    for (EigenSparseVector::InnerIterator it(d_vector); it; ++it) {
        values.push_back(it.value());
    }
    
    return values;
}

std::vector<std::pair<int, double>> SparseVector::getNonZeroPairs() const
{
    std::vector<std::pair<int, double>> pairs;
    pairs.reserve(nonZeros());
    
    for (EigenSparseVector::InnerIterator it(d_vector); it; ++it) {
        pairs.emplace_back(it.index(), it.value());
    }
    
    return pairs;
}

void SparseVector::setZero()
{
    d_vector.setZero();
}

void SparseVector::prune(double threshold, double reference)
{
    double actualThreshold = std::abs(threshold * reference);
    // For SparseVector, we need to manually prune since there's no prune method
    if (actualThreshold > 0.0) {
        std::vector<int> indicesToRemove;
        std::vector<double> valuesToKeep;
        std::vector<int> indicesToKeep;
        
        // Collect elements to keep
        for (EigenSparseVector::InnerIterator it(d_vector); it; ++it) {
            if (std::abs(it.value()) >= actualThreshold) {
                indicesToKeep.push_back(it.index());
                valuesToKeep.push_back(it.value());
            }
        }
        
        // Rebuild the vector
        d_vector.setZero();
        d_vector.reserve(static_cast<int>(indicesToKeep.size()));
        for (size_t i = 0; i < indicesToKeep.size(); ++i) {
            d_vector.insert(indicesToKeep[i]) = valuesToKeep[i];
        }
    }
}

void SparseVector::saveToFile(const std::string& filename) const
{
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be empty");
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    
    // Save in simple format: index value pairs
    file << size() << " " << nonZeros() << std::endl;
    for (EigenSparseVector::InnerIterator it(d_vector); it; ++it) {
        file << it.index() << " " << it.value() << std::endl;
    }
}

void SparseVector::loadFromFile(const std::string& filename)
{
    if (filename.empty()) {
        throw std::invalid_argument("Filename cannot be empty");
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }
    
    int size, nonzeros;
    file >> size >> nonzeros;
    
    if (size < 0 || nonzeros < 0 || nonzeros > size) {
        throw std::runtime_error("Invalid file format: corrupted size information");
    }
    
    d_vector.resize(size);
    d_vector.setZero();
    d_vector.reserve(nonzeros);
    
    for (int i = 0; i < nonzeros; ++i) {
        int index;
        double value;
        file >> index >> value;
        
        if (index < 0 || index >= size) {
            throw std::runtime_error("Invalid file format: index out of range");
        }
        
        d_vector.insert(index) = value;
    }
}

void SparseVector::printInfo() const
{
    std::cout << "Vector: " << (d_name.empty() ? "Unnamed" : d_name) << "\n"
              << "Size: " << size() << "\n"
              << "Non-zero elements: " << nonZeros() << "\n"
              << "Sparsity: " << (100.0 * sparsity()) << "%\n"
              << "L2 Norm: " << norm() << "\n"
              << "L1 Norm: " << l1Norm() << "\n"
              << "Linf Norm: " << linfNorm() << std::endl;
}

void SparseVector::printVector() const
{
    if (size() > 100) {
        std::cout << "Vector too large to print (" << size() 
                  << " elements). Use printInfo() for summary." << std::endl;
        return;
    }
    
    std::cout << "Vector " << (d_name.empty() ? "Unnamed" : d_name) 
              << " (" << size() << " elements):" << std::endl;
    
    auto dense = toDense();
    for (int i = 0; i < size(); ++i) {
        std::cout << "[" << i << "] = " << dense(i) << std::endl;
    }
}

// Free functions implementation
SparseVector operator*(double scalar, const SparseVector& vector)
{
    return vector * scalar;
}

std::ostream& operator<<(std::ostream& os, const SparseVector& vector)
{
    os << "SparseVector: " << (vector.name().empty() ? "Unnamed" : vector.name()) 
       << " (" << vector.size() << " elements), nonzeros: " << vector.nonZeros();
    return os;
}

Eigen::SparseMatrix<double> outerProduct(const SparseVector& lhs, const SparseVector& rhs)
{
    return lhs.vector() * rhs.vector().transpose();
}

// SparseVector concatenateVertical(const SparseVector& top, const SparseVector& bottom)
// {
//     SparseVector result(top.size() + bottom.size());
    
//     // Copy top elements
//     for (EigenSparseVector::InnerIterator it(top.vector()); it; ++it) {
//         result.insert(it.index()) = it.value();
//     }
    
//     // Copy bottom elements with offset
//     for (EigenSparseVector::InnerIterator it(bottom.vector()); it; ++it) {
//         result.insert(top.size() + it.index()) = it.value();
//     }
    
//     return result;
// }

// Eigen::SparseMatrix<double> concatenateHorizontal(const SparseVector& left, const SparseVector& right)
// {
//     int totalSize = left.size() + right.size();
//     Eigen::SparseMatrix<double> result(totalSize, totalSize);
    
//     // Set left vector on diagonal
//     for (EigenSparseVector::InnerIterator it(left.vector()); it; ++it) {
//         result.insert(it.index(), it.index()) = it.value();
//     }
    
//     // Set right vector on diagonal with offset
//     for (EigenSparseVector::InnerIterator it(right.vector()); it; ++it) {
//         int index = left.size() + it.index();
//         result.insert(index, index) = it.value();
//     }
    
//     return result;
// }