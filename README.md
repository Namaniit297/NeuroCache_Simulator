# 🧠 NeuroCache Simulator

A compiler-integrated **L1 Cache Simulator** for evaluating cache performance under various memory access patterns. This simulator emphasizes **replacement policies** like **RidxLRU** and **SRRIP**, and supports trace generation in a manner similar to the **SPADA** paper, helping to reduce energy loss due to memory access inefficiencies.

---

## 📌 Objective

This project aims to simulate and evaluate the effects of different cache replacement policies on memory performance and energy efficiency, focusing on sparse dataflow and adaptive strategies (like SPADA). The simulator allows researchers to study:

- Data locality
- Cache utilization
- Total memory traffic
- Energy-related impact of replacement decisions

---

## 🧩 Supported Replacement Policies

- ✅ **LRU (Least Recently Used)**
- ✅ **FIFO (First-In-First-Out)**
- ✅ **RidxLRU** (Register Index based LRU - NEW)
- ✅ **SRRIP** (Static Re-Reference Interval Prediction - NEW)

---

## 📁 Project Structure

```
NeuroCache_Simulator/
├── CacheSimulator/
│   ├── cacheSim.cpp            # Main driver
│   ├── main.h                  # Simulator header (modular definitions)
│   ├── Cache.cpp               # L1 Cache read/write logic
│   ├── RidxLRU.cpp             # New: RidxLRU replacement logic
│   ├── SRRIP.cpp               # New: SRRIP logic implementation
│   ├── TraceParser.cpp         # Parses traces, SPADA-style adaptive window
│   ├── headers/
│   │   ├── Cache.hpp           # Cache class definitions
│   │   ├── RidxLRU.hpp
│   │   ├── SRRIP.hpp
│   ├── utils/
│   │   ├── TraceGen.cpp        # Generates different trace scenarios
│   │   └── Helpers.hpp         # Misc helper functions
│   └── cacheSim                # Compiled binary (after build)
├── traces/
│   ├── gcc.trace
│   ├── spada_window.trace      # Window-based dataflow trace
│   ├── custom_dataflow/        # Varying sparsity traces
├── README.md
├── Makefile
└── LICENSE
```

---

## ✅ Features

- ✅ Fully parameterizable L1 Cache: size, associativity, block size
- ✅ Memory access modes: `read`, `write`, with `write-back` / `write-through`
- ✅ Configurable on:
  - Write Miss Policy: `write-allocate`, `no-write-allocate`
  - Replacement Policy: `lru`, `fifo`, `ridxlru`, `srrip`
- ✅ Trace generation for:
  - Dense memory patterns
  - Sparse/SPADA-style window flows
- ✅ Outputs:
  - Hit/miss statistics
  - Total traffic
  - Writebacks
  - (Upcoming) Energy metrics

---

## ⚙️ Build Instructions

### 1️⃣ Clone the Repository

```bash
git clone https://github.com/Namaniit297/NeuroCache_Simulator.git
cd NeuroCache_Simulator/CacheSimulator
```

### 2️⃣ Compile with `make`

```bash
make
```

> ✅ Requires: `g++` with C++17 support (e.g., `g++-9` or later)

---

## 🚀 Running the Simulator

### ✅ Syntax

```bash
./cacheSim <cache_size_kb> <associativity> <block_size> <write_miss_policy> <write_hit_policy> <replacement_policy> < input_trace
```

### ✅ Example

```bash
./cacheSim 32 2 16 write-allocate write-back ridxlru < ../traces/gcc.trace
```

---

## 🧪 Trace Format

Each trace file should contain memory accesses in this format:

```
r 0x0040a1f4
w 0x0040a1f8
r 0x00a0f234
```

- `r`: load (read)
- `w`: store (write)
- Address: Hexadecimal

---

## 📊 Sample Output

```
===== Simulator Configuration =====
L1_SIZE:                  32 KB
L1_ASSOC:                 2-way
BLOCKSIZE:                16 B
WRITE MISS POLICY:        write-allocate
WRITE HIT POLICY:         write-back
REPLACEMENT POLICY:       RidxLRU

===== Simulation Results =====
a. Number of reads:             15240
b. Read misses:                 3201
c. Number of writes:            11321
d. Write misses:                2877
e. L1 miss rate:                23.5%
f. Total memory traffic:        7365
```

---

## 📄 Experimental Traces

SPADA and dataflow-based traces are generated under `traces/custom_dataflow/`. These mimic:

- Row-wise block multiplication (like SPADA)
- Diagonal and random access windows
- Ridx and reuse-insensitive blocks

---

## 🔍 Research Focus

We evaluate how custom-designed replacement policies like **RidxLRU** and **SRRIP** perform on varying data reuse windows, specifically in:

- Sparse matrix-vector multiplies (SpMV)
- Sliding windows in CNN-like flows
- Compiler-generated access traces

---

## 📈 Planned Extensions

- [ ] Energy profiling (RAPL / estimation)
- [ ] Adaptive replacement (SPADA window aware)
- [ ] LLVM pass integration for trace generation
- [ ] Trace visualization tools
- [ ] ML-guided cache decisions

---

## 🧠 Inspiration

Based on:
> **SPADA: Accelerating Sparse Matrix Multiplication with Adaptive Dataflow**  
> Danial Moghaddam, Xin Chen, et al.

---

## 📜 License

Licensed under the MIT License.  
See [LICENSE](./LICENSE) for details.

---
