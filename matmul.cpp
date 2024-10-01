#define ACCELERATE_NEW_LAPACK 1
#include <cblas_new.h>
#include <cassert>
#include "matrix.h"
#include <iostream>
#include <algorithm>
#include <cstring>


static const int TilingFactorM = 2048;
static const int TilingFactorN = 1024;
static const int TilingFactorK = 16;

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> &other) {
    assert(matmul);
    return matmul(*this, other);
}

template <typename T>
Matrix<T> NaiveStrategy(const Matrix<T> &A, const Matrix<T> &B) {
    if(A.getCols() != B.getRows()) {
        throw std::invalid_argument("The dimentions of Matrices are invalid for multiplication");
    }
    Matrix<T> Result(A.getRows(), B.getCols());

    T * pA = A.getData();
    T * pB = B.getData();
    unsigned int Rows = A.getRows();
    unsigned int Cols = B.getCols();
    unsigned int Inner = A.getCols();

    for(unsigned i =0; i < Rows; i++) {
        for(unsigned j = 0; j < Cols ; j++ ) {
            T sum = 0;
            for(unsigned k = 0; k < Inner; k++) {
                sum += pA[i*Inner+k] * pB[k*Cols+j];
            }
            Result[i*Cols+j] = sum;
        }
    }
    return Result;
}

template <typename T>
Matrix<T> LoopFlipping(const Matrix<T> &A, const Matrix<T> &B){
    if(A.getCols() != B.getRows()) {
        throw std::invalid_argument("The dimentions of Matrices are invalid for multiplication");
    }
    Matrix<T> Result(A.getRows(), B.getCols());

    T * pA = A.getData();
    T * pB = B.getData();
    T * pC = Result.getData();
    unsigned int Rows = A.getRows();
    unsigned int Cols = B.getCols();
    unsigned int Inner = A.getCols();

    for(unsigned int i = 0; i < Rows; i++) {
        for(unsigned int k = 0; k < Inner ; k++ ) {
            for(unsigned int j = 0; j < Cols; j++) {
                pC[i*Cols+j] += pA[i*Inner+k] * pB[k*Cols+j];
            }
        }
    }
    return Result;
}

template <typename T>
Matrix<T> Tiling(const Matrix<T> &A, const Matrix<T> &B){
    
    if(A.getCols() != B.getRows()) {
        throw std::invalid_argument("The dimentions of the Matrices are invalid for multiplication");
    }
    T * pA = A.getData();
    T * pB = B.getData();
    unsigned int Rows = A.getRows();
    unsigned int Cols = B.getCols();
    unsigned int Inner = A.getCols();

    Matrix<T> Result(Rows , Cols);
    T * pC = Result.getData();

    for (unsigned int  rowTile = 0; rowTile < Rows; rowTile += TilingFactorM) {
        unsigned int  rowTileEnd = std::min(Rows, rowTile + TilingFactorM);
        for (unsigned int  colTile = 0; colTile < Cols; colTile += TilingFactorN) {
            unsigned int  colTileEnd = std::min(Cols, colTile + TilingFactorN);
            for (unsigned int  innerTile = 0; innerTile < Inner; innerTile += TilingFactorK) {
                unsigned int  innerTileEnd = std::min(Inner, innerTile + TilingFactorK);
                for(unsigned int  i = rowTile; i < rowTileEnd; i++) {
                    for (unsigned int  k = innerTile; k < innerTileEnd; k++) {
                        for(unsigned int  j = colTile ; j < colTileEnd ; j++) {
                            pC[i*Cols+j]+=pA[i*Inner+k] * pB[k*Cols+j];
                        }
                    }
                }
            }
        }
    }
    return Result;
}

template <typename T>
Matrix<T> OpenMP(const Matrix<T> &A, const Matrix<T> &B){
    if(A.getCols() != B.getRows()) {
        throw std::invalid_argument("The dimentions of the Matrices are invalid for multiplication");
    }
    Matrix<T> Result(A.getRows(), B.getCols());

    T * pA = A.getData();
    T * pB = B.getData();
    T * pC = Result.getData();
    unsigned int Rows = A.getRows();
    unsigned int Cols = B.getCols();
    unsigned int Inner = A.getCols();

    #pragma omp parallel for shared(pA, pB, pC, Rows, Cols, Inner, TilingFactorM, TilingFactorN, TilingFactorK) default(none) collapse(2) num_threads(12)
    for (unsigned int  rowTile = 0; rowTile < Rows; rowTile += TilingFactorM) {
        unsigned int  rowTileEnd = std::min(Rows, rowTile + TilingFactorM);
        for (unsigned int  colTile = 0; colTile < Cols; colTile += TilingFactorN) {
            unsigned int  colTileEnd = std::min(Cols, colTile + TilingFactorN);
            for (unsigned int  innerTile = 0; innerTile < Inner; innerTile += TilingFactorK) {
                unsigned int  innerTileEnd = std::min(Inner, innerTile + TilingFactorK);
                for(unsigned int  i = rowTile; i < rowTileEnd; i++) {
                    for (unsigned int  k = innerTile; k < innerTileEnd; k++) {
                        for(unsigned int  j = colTile ; j < colTileEnd ; j++) {
                            pC[i*Cols+j]+=pA[i*Inner+k] * pB[k*Cols+j];
                        }
                    }
                }
            }
        }
    }
    return Result; 
}

static void blas_gemm(const Matrix<float> &A, const Matrix<float> &B, Matrix<float> &C) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A.getRows(), B.getCols(), A.getCols(), 1.0, A.getData(), A.getCols(), B.getData(), B.getCols(), 1.0, C.getData(), C.getCols());
}

static void blas_gemm(const Matrix<double> &A, const Matrix<double> &B, Matrix<double> &C) {
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, A.getRows(), B.getCols(), A.getCols(), 1.0, A.getData(), A.getCols(), B.getData(), B.getCols(), 1.0, C.getData(), C.getCols());
}

static void blas_gemm(const Matrix<int> &A, const Matrix<int> &B, Matrix<int> &C) {
    assert(0); // should not be called with integer matrices
}

template <typename T>
Matrix<T> BLAS(const Matrix<T> &A, const Matrix<T> &B) {
    if(A.getCols() != B.getRows()) {
        throw std::invalid_argument("The dimentions of the Matrices are invalid for multiplication");
    }
    Matrix<T> Result(A.getRows(), B.getCols());

    blas_gemm(A, B, Result);
return Result;
}

template <typename T> Matrix<T>::MultiplyFunction Matrix<T>::matmul = nullptr;

template class Matrix<int>;
template class Matrix<float>;
template class Matrix<double>;

template Matrix<int> NaiveStrategy(const Matrix<int> &A, const Matrix<int> &B);
template Matrix<float> NaiveStrategy(const Matrix<float> &A, const Matrix<float> &B);
template Matrix<double> NaiveStrategy(const Matrix<double> &A, const Matrix<double> &B);

template Matrix<int> LoopFlipping(const Matrix<int> &A, const Matrix<int> &B);
template Matrix<float> LoopFlipping(const Matrix<float> &A, const Matrix<float> &B);
template Matrix<double> LoopFlipping(const Matrix<double> &A, const Matrix<double> &B);

template Matrix<int> Tiling(const Matrix<int> &A, const Matrix<int> &B);
template Matrix<float> Tiling(const Matrix<float> &A, const Matrix<float> &B);
template Matrix<double> Tiling(const Matrix<double> &A, const Matrix<double> &B);

template Matrix<int> OpenMP(const Matrix<int> &A, const Matrix<int> &B);
template Matrix<float> OpenMP(const Matrix<float> &A, const Matrix<float> &B);
template Matrix<double> OpenMP(const Matrix<double> &A, const Matrix<double> &B);

template Matrix<int> BLAS(const Matrix<int> &A, const Matrix<int> &B);
template Matrix<float> BLAS(const Matrix<float> &A, const Matrix<float> &B);
template Matrix<double> BLAS(const Matrix<double> &A, const Matrix<double> &B);

