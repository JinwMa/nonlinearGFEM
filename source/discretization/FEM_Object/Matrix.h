#ifndef GFEM_MATRIX_H
#define GFEM_MATRIX_H
#include <vector>
#include <stdexcept>

template<typename T>
class Matrix {
private:
    int d_row, d_col;
    std::vector<T> d_data;

public:
    Matrix(int rows, int cols) : d_row(rows), d_col(cols), d_data(rows * cols) {}
    
    // 快速访问 - 无边界检查
    T& operator()(int row, int col) noexcept {
        return d_data[row * d_col + col];
    }
    
    const T& operator()(int row, int col) const noexcept {
        return d_data[row * d_col + col];
    }
    
    // 数组式访问 - 返回行首指针
    T* operator[](int row) noexcept {
        return &d_data[row * d_col];
    }
    
    const T* operator[](int row) const noexcept {
        return &d_data[row * d_col];
    }
    
    // 安全访问 - 带边界检查
    T& at(int row, int col) {
        if (row < 0 || row >= d_row || col < 0 || col >= d_col)
            throw std::out_of_range("Matrix indices out of range");
        return (*this)(row, col);
    }
};


#endif