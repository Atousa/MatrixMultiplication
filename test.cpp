#include "matmul.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <format>

void testDefaultConstructor() {
    Matrix<int> mat;
    assert(mat.getRows() == 0);
    assert(mat.getCols() == 0);

    std::cout << "testdefaultConstructor.\n";
}

void testParameterizedConstructor() {
    Matrix<int> mat(3, 3);
    assert(mat.getRows() == 3);
    assert(mat.getCols() == 3);

    std::cout << "testParameterizedConstructor.\n";
}

void testCopyConstructor() {
    Matrix<int> mat1(4,2, {1, 2, 3, 4, 5, 6, 7, 8 });

    Matrix<int> mat2(mat1);
    assert(mat2 == mat1);

    std::cout << "testCopyConstructor passed.\n";
}

void testMoveConstructor() {
    Matrix<int> mat1(3, 2, { 8, 2, 3, 3, 9, 6 });
    auto mat1Ref(mat1);

    Matrix<int> mat2(std::move(mat1));
    assert(mat1.getRows() == 0);
    assert(mat1.getCols() == 0);
    assert(mat1.getData() == 0);

    assert(mat2.getRows() == 3);
    assert(mat2.getCols() == 2);
    assert(mat2 == mat1Ref);
   
    std::cout << "testMoveConstructor passed.\n";
}

void testCopyAssignmentOperator() {
    Matrix<int> mat1(3, 2, { 1, 2, 3, 4, 5, 6 });
    auto mat1Ref(mat1);

    Matrix<int> mat2(1, 2);
    mat2 = mat1;

    assert(mat1 == mat1Ref);
    assert(mat2 == mat1);

    std::cout << "testCopyAssignmentOperator passed.\n";  
}

void testMoveAssignmentOperator() {
    Matrix<int> mat1(3, 2, { 1, 2, 3, 4, 5, 6 });
    auto mat1Ref(mat1);

    Matrix<int> mat2(2, 2);
    mat2 = std::move(mat1);
    assert(mat2 == mat1Ref);

    assert(mat1.getRows() == 0);
    assert(mat1.getCols() == 0);
    assert(mat1.getData() == 0);

    std::cout << "testMoveAssignmentOperator passed.\n";
}

void testMultiplicationOperator() {
    std::cout.precision(20);
    Matrix<float> mat1(2, 3, { 1, 2, 3, 4, 5, 6 });
    Matrix<float> mat2(3, 2, { 7, 8, 9, 10, 11, 12 });
    Matrix<float> result = mat1 * mat2;
    assert(result.getRows() == 2);
    assert(result.getCols() == 2);
    Matrix<float> resultRef(2, 2, { 58, 64, 139, 154});
    assert(result == resultRef);

    Matrix<float> mat3(3, 3, { 1.2f,2.4f,3.7f,7.6f,6.9f,3.2f,0.8f,2.5f,3.4f });
    Matrix<float> mat4(3, 3, { 4.3f,5.8f,6.2f,9.1f,5.3f,7.2f,4.2f,4.8f,6.2f });
    Matrix<float> result2 = mat3 * mat4;
    assert(result2.getRows() == 3);
    assert(result2.getCols() == 3);
    Matrix<float> resultRef2(3, 3, { 42.54f,37.44f,47.66f,108.91f,96.01f,116.64f,40.47f,34.21f,44.04f});
    Matrix<float> delta = result2-resultRef2;
    assert(result2 == resultRef2);

    std::cout << "testMultiplicationOperator passed.\n";
}

void testMultiplicationScalarOperator() {
    Matrix<int> mat(2, 2, { 1, 2, 3, 4 });

    Matrix<int> result = mat * 3;
    assert(result.getRows() == 2);
    assert(result.getCols() == 2);
    Matrix<int> resultRef(2, 2, { 3, 6, 9, 12});
    assert(result == resultRef);

    std::cout << "testMultiplicationScalarOperator passed.\n";
}

void testSubscriptOperator() {
    Matrix<int> mat(3, 3, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });
    
    for (int i = 0; i < mat.getRows() * mat.getCols(); ++i) {
        assert(mat[i] == i + 1);  
    }
    
    std::cout << "testSubscriptOperator passed.\n";
}

void testGEMM() {
    static const int DIM=1024;
    Matrix<float> mat1(DIM, DIM);
    Matrix<float> mat2(DIM, DIM);
    Matrix<float> result;
 
    std::seed_seq seed{42};
    std::mt19937_64 engine(seed);
    std::normal_distribution<float> normal_dist(0.0, 1.0);

    rand(mat1, engine, normal_dist);
    rand(mat2, engine, normal_dist);



    Matrix<float>::SetMatMulPolicy(NaiveStrategy);
    auto start = std::chrono::high_resolution_clock::now();
    result = mat1 * mat2;
    auto stop = std::chrono::high_resolution_clock::now();
    auto deltaT = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout.imbue(std::locale(""));
    std::cout << "GEMM Naive ΔT=" << deltaT << " (1.00x)\n";

    float Ref = deltaT.count();
    Matrix<float> resultRef;
    resultRef = result; // known good
    float timeRef = deltaT.count();

    Matrix<float>::SetMatMulPolicy(LoopFlipping);
    start = std::chrono::high_resolution_clock::now();
    result = mat1 * mat2;
    stop = std::chrono::high_resolution_clock::now();
    deltaT = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "GEMM Loop Flipping ΔT=" << deltaT << " (" << std::format("{:.2f}", timeRef/deltaT.count()) << "x)\n";
    assert(result == resultRef);

    Matrix<float>::SetMatMulPolicy(Tiling);
    start = std::chrono::high_resolution_clock::now();
    result = mat1 * mat2;
    stop = std::chrono::high_resolution_clock::now();
    deltaT = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "GEMM Tiling ΔT=" << deltaT << " (" << std::format("{:.2f}", timeRef/deltaT.count()) << "x)\n";
    assert(result == resultRef);

    Matrix<float>::SetMatMulPolicy(OpenMP);
    start = std::chrono::high_resolution_clock::now();\
    result = mat1 * mat2;
    stop = std::chrono::high_resolution_clock::now();
    deltaT = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "GEMM OpenMP ΔT=" << deltaT << " (" << std::format("{:.2f}", timeRef/deltaT.count()) << "x)\n";
    assert(result == resultRef);

    Matrix<float>::SetMatMulPolicy(BLAS);
    start = std::chrono::high_resolution_clock::now();
    result = mat1 * mat2;
    stop = std::chrono::high_resolution_clock::now();
    deltaT = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "GEMM BLAS ΔT=" << deltaT << " (" << std::format("{:.2f}", timeRef/deltaT.count()) << "x)\n";
}

int main() {
    Matrix<float>::SetMatMulPolicy(NaiveStrategy);

    testDefaultConstructor();
    testParameterizedConstructor();
    testCopyConstructor();
    testMoveConstructor();
    testCopyAssignmentOperator();
    testMoveAssignmentOperator();
    testMultiplicationOperator();
    testMultiplicationScalarOperator();
    testSubscriptOperator();

    testGEMM();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
