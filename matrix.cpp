#include <initializer_list>
#include <exception>
#include "matrix.h"

static const float EPSILON = 1e-5;

template <typename T>
Matrix<T>::Matrix()
	: Rows(0), Cols(0), mm(nullptr) {}

template <typename T>
Matrix<T>::Matrix(unsigned int rows, unsigned int cols)
	: Rows(rows), Cols(cols), mm(new T[rows*cols])
{
	std::fill(mm, mm+rows*cols, 0);
}

template <typename T>
Matrix<T>::Matrix(unsigned int rows, unsigned int cols, const std::initializer_list<T>& init)
	: Rows(rows), Cols(cols)
{
	if(init.size() != rows * cols) {
		throw std::invalid_argument("Invalid List size");
	}
	mm = new T[rows * cols];
	auto it = init.begin();
	for(unsigned int i = 0; i < rows; i++) {
		for(unsigned int j = 0; j < cols; j++) {
			mm[i * cols + j] = *it++;
		}
	}
}

template <typename T>
Matrix<T>::Matrix(const Matrix &other)
	: Rows(other.Rows), Cols(other.Cols), mm(new T[other.Rows * other.Cols])
{
	std::copy(other.mm,other.mm+Rows*Cols, mm);
}

template <typename T>
Matrix<T>::Matrix(Matrix &&other) noexcept: Rows(other.Rows), Cols(other.Cols), mm(other.mm) {
	other.Rows = 0;
	other.Cols = 0;
	other.mm = nullptr;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix &other){
	if( this != &other) {
		if(mm) delete[] mm;
		Rows = other.Rows;
		Cols = other.Cols;
		mm = new T[Rows * Cols];
		std::copy(other.mm, other.mm+Rows*Cols, mm);
	}
	return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(Matrix &&other) noexcept{
	if(this != &other) {
		if(mm) delete[] mm;
		Rows = other.Rows;
		Cols = other.Cols;
		mm = other.mm;

		other.Rows = 0;
		other.Cols = 0;
		other.mm = nullptr;
	}
	return *this;
}

template <typename T>
Matrix<T> Matrix<T>::operator-(const Matrix<T> &other) {
	Matrix<T> Result(Rows,Cols); 
	for(int i = 0; i < Rows*Cols; i++) {
		Result[i] = mm[i] - other.mm[i];
	}
	return Result;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const T v) const {
	Matrix<T> Result(Rows,Cols); 
	for(int i = 0; i < Rows*Cols; i++) {
		Result[i] = mm[i] * v;
	}
	return Result;
}

template <>
bool Matrix<int>::operator==(const Matrix &other) const {
	if(Rows != other.Rows) return false;
    if(Cols != other.Cols) return false;
    for(int i=0; i<Rows*Cols; ++i) {
        if(mm[i] != other.mm[i]) return false;
    }
    return true;
}

template <typename T>
bool Matrix<T>::operator==(const Matrix &other) const {
	if(Rows != other.Rows) {
		std::cout << "Rows are not similar/n";
		return false;
	}
    if(Cols != other.Cols) {
		std::cout << "Rows are not similar/n";
		return false;
	}
    for(int i=0; i<Rows*Cols; ++i) {
        if(std::abs(mm[i] - other.mm[i]) > EPSILON) {
			std::cout << "values are not similar\n";
			std::cout << "i = " << i << "\n";
			std::cout << "v1 = " << mm[i] << " v2 = " << other.mm[i] << " delta = " << (mm[i]-other.mm[i]) << "\n";
			return false;
		}
    }
    return true;
}

template <typename T>
T& Matrix<T>::operator[](unsigned int idx) {
	return mm[idx];
}

template <typename T>
const T& Matrix<T>::operator[](unsigned int idx) const{
	return mm[idx];
}

template <typename T>
inline T& Matrix<T>::operator()(unsigned int row, unsigned int col) {
	return mm[row*Cols + col];
}
template <typename T>
inline const T& Matrix<T>::operator()(unsigned int row, unsigned int col) const{
	return mm[row*Cols + col];
}

template <typename T>
Matrix<T>::~Matrix(){
	delete[] mm;
}

template <typename T>
void rand(Matrix<T>& mat, std::mt19937_64& rnd, std::normal_distribution<T>& distr){
	for(int i = 0 ; i < mat.getRows() * mat.getCols(); i++) {
		mat.getData()[i] = distr(rnd);
	}
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const Matrix<T>& mat){
	for(unsigned int i= 0 ; i < mat.getRows(); i++) {
		for(unsigned int j = 0; j< mat.getCols(); j++) {
			out << mat(i , j) << " ";
		}
		out << std::endl;
	}
	return out;
}

template class Matrix<int>;
template class Matrix<float>;
template class Matrix<double>;

template void rand(Matrix<float>&, std::mt19937_64& , std::normal_distribution<float>& );
template void rand(Matrix<double>&, std::mt19937_64& , std::normal_distribution<double>& );

template std::ostream& operator<<(std::ostream&, const Matrix<int>&);
template std::ostream& operator<<(std::ostream&, const Matrix<float>&);
template std::ostream& operator<<(std::ostream&, const Matrix<double>&);

