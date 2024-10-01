# Matrix Multiplication Optimizations

## Description
This project implements matrix multiplication using various CPU optimization techniques such as:
- Tiling
- Loop flipping
- OpenMP for parallel processing
- BLAS (Basic Linear Algebra Subprograms)

## Features
- Efficient matrix multiplication with different optimization strategies.
- Code implementations designed for performance on modern CPUs.
- OpenMP integration for multi-threading.
- BLAS usage for high-performance matrix operations.

## Optimization Techniques and Performance Results

- Naive GEMM:
    ΔT = 8,047,948µs (1.00x)
- Loop Flipping:
    ΔT = 590,462µs (13.63x)
- Tiling:
    ΔT = 585,108µs (13.75x)
- OpenMP (Parallelized):
    ΔT = 386,984µs (20.80x)
- BLAS:
    ΔT = 7,296µs (1103.06x)