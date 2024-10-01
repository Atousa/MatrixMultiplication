#pragma once 

#include <stdio.h>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <random>
#include <algorithm>


template <typename T>
class Matrix {
    typedef Matrix<T> (*MultiplyFunction)(const Matrix<T>&, const Matrix<T>&);
public:
    Matrix();
    Matrix(unsigned int rows, unsigned int cols);
    Matrix(unsigned int Rows, unsigned int Cols, const std::initializer_list<T>& init);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other) noexcept;
    Matrix& operator=(const Matrix& other);
    Matrix& operator=(Matrix&& other) noexcept;
    Matrix operator*(const Matrix &other);
    Matrix operator-(const Matrix &other);
    Matrix operator*(const T v) const;
    bool operator==(const Matrix &other) const; 
    T& operator[](unsigned int idx);
    const T& operator[](unsigned int idx)const;
    T& operator()(unsigned int row, unsigned int col);
    const T& operator()(unsigned int row, unsigned int col) const;
    unsigned int getRows() const { return Rows; }
    unsigned int getCols() const { return Cols; }
    T* getData() const { return mm; }
    ~Matrix();
    static void SetMatMulPolicy(MultiplyFunction fn) { matmul = fn; }
private:
    unsigned int Rows,Cols;
    T* mm;
    static MultiplyFunction matmul;
};

template <typename T>
void rand(Matrix<T>& mat, std::mt19937_64& rnd, std::normal_distribution<T>& distr);

template <typename T>
std::ostream& operator<<(std::ostream& s, const Matrix<T>& mat);

