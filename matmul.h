#include "matrix.h"

template <typename T>
Matrix<T> NaiveStrategy(const Matrix<T> &A, const Matrix<T> &B);

template <typename T>
Matrix<T> LoopFlipping(const Matrix<T> &A, const Matrix<T> &B);

template <typename T>
Matrix<T> Tiling(const Matrix<T> &A, const Matrix<T> &B);

template <typename T>
Matrix<T> OpenMP(const Matrix<T> &A, const Matrix<T> &B);

template <typename T>
Matrix<T> BLAS(const Matrix<T> &A, const Matrix<T> &B);



