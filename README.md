# Optimization of a Digital Communication Chain (SIMD & Multi-threading)

## Overview
This repository contains a C/C++ project focused on optimizing a digital communication chain simulation[cite: 18, 19]. Developed as part of the PPSE mini-project at Sorbonne Université (Master SESI)[cite: 17], the goal is to maximize the simulation throughput (in Mbps) by leveraging both data-level and task-level parallelization techniques[cite: 19, 31].

The simulation models a complete transmission chain including a Source, Repetition Encoder, Modulator, AWGN Channel, Demodulator, and Decoder[cite: 19].

## Key Features & Optimizations

### 1. Data-Level Parallelization (SIMD & Architecture)
*   **ARM NEON Vectorization:** Acceleration of the modulator, demodulator, and monitor blocks using SIMD instructions to process multiple data points in a single clock cycle[cite: 19].
*   **Bit-packing:** Efficient data storage (packing 8 bits of information into a single byte) using compiler intrinsics (`__builtin_popcount`), drastically reducing load/store memory operations[cite: 22].
*   **Fixed-Point Arithmetic:** Implementation of Soft and Hard decision decoding using fixed-point representation (Soft FP / Hard FP) to compare performance against standard floating-point execution[cite: 24, 25].

### 2. Task-Level Parallelization (Multi-threading)
*   **Pthreads Integration:** Parallelization of the entire communication chain across 6 CPU cores, running standalone simulations simultaneously[cite: 26].
*   **Atomic Synchronization:** Use of atomic variables (`n_bit_errors`, `n_frame_errors`) to manage the frame error counters across threads without the heavy overhead of standard mutex locks[cite: 26].
*   **Massive Speedup:** The synergy of SIMD vectorization (spatial parallelization) and multi-threading (temporal parallelization) significantly multiplied the global throughput[cite: 31].

### 3. Data Analysis & Visualization
*   **Python Plotting Scripts:** A suite of Python scripts to automatically parse simulation results and generate Bit Error Rate (BER) and Frame Error Rate (FER) curves depending on the Signal-to-Noise Ratio (Eb/N0)[cite: 32].

## Technologies Used
*   **Languages:** C/C++, Python (for data visualization)[cite: 26, 32].
*   **Hardware Optimizations:** ARM NEON (SIMD), Multi-threading (pthread), Bit-packing[cite: 19, 22, 26].
*   **Build System:** CMake / Makefile[cite: 31].
