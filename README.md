# Matrix Multiplication Optimizations

## Description
This project implements a minimalistic matrix multiplication focusing on various CPU optimization techniques such as:
- Tiling
- Loop flipping
- OpenMP for parallel processing
- BLAS (Basic Linear Algebra Subprograms)

## Features
- Efficient matrix multiplication with different optimization strategies.
- Code implementations designed for performance on modern CPUs.
- OpenMP integration for multi-threading.
- BLAS for high-performance matrix operations.

## Optimization Techniques and Performance Results

On a Apple M3 Max I get the following timings —
- Naive GEMM:
    ΔT=937,902µs (1.00x)
- Loop Flipping:
    ΔT=70,094µs (13.38x)
- Tiling:
    ΔT=71,804µs (13.06x)
- OpenMP (Parallelized):
    ΔT=10,752µs (87.23x)
- BLAS (Apple Accelerate Framework):
    ΔT=1,087µs (862.84x)

Note that the accelerate framework has access to special instructions, I take it as speed-of-light.

