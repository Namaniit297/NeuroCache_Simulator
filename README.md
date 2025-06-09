# 🧠 NeuroCache Simulator

A compiler-integrated **L1 Cache Simulator** for evaluating cache performance under various memory access patterns, aimed at reducing data movement between compute cores and external memory — a primary source of energy loss in modern systems.

> 📄 Based on the concepts from the paper:  
> **“SPADA: Accelerating Sparse Matrix Multiplication with Adaptive Dataflow”**

---

## 📌 Objective

This project investigates various cache configurations and memory allocation/replacement policies to evaluate **data locality, cache utilization, and energy efficiency**. In particular, we compare baseline cache policies with **SPADA-style adaptive strategies** that reduce unnecessary memory movement — a key contributor to overall energy consumption.

---

## 📁 Repository Structure

```

NeuroCache\_Simulator/
├── CacheSimulator/
│   ├── cacheSim.cpp         # Main simulator code
│   ├── headers/             # Header files (modularized)
│   ├── utils/               # Utility and helper functions
│   ├── cacheSim             # Compiled binary (after make)
│   └── ...
├── traces/
│   ├── gcc.trace            # Example trace file
│   └── other\_traces/
├── Makefile
└── README.md

````

---

## ✅ Features

- ✅ Parameterizable **L1 Cache** (Size, Associativity, Block Size)
- ✅ Accurate simulation of **read/write** memory accesses
- ✅ Multiple memory policies:
  - Write: `write-back`, `write-through`
  - Allocation: `write-allocate`, `no-write-allocate`
  - Replacement: `lru`, `fifo`, `spada` (coming soon)
- ✅ Text-based reporting for:
  - Read/write counts
  - Miss rates
  - Total memory traffic

---

## ⚙️ Build Instructions

### 1️⃣ Clone and Enter the Project

```bash
git clone https://github.com/Namaniit297/NeuroCache_Simulator.git
cd NeuroCache_Simulator/CacheSimulator
````

### 2️⃣ Compile with Make

```bash
make
```

> ✅ Requires C++17-compatible compiler (e.g., `g++-9` or later)

---

## 🚀 Running the Simulator

### ✅ Command Syntax

```bash
./cacheSim <cache_size_kb> <associativity> <block_size> <write_miss_policy> <write_hit_policy> <replacement_policy> < input_trace
```

### ✅ Example

```bash
./cacheSim 32 2 16 write-allocate write-back lru < ../traces/gcc.trace
```

---

## 📄 Input Trace Format

Trace files must follow this format:

```
r 0x0040a1f4
w 0x0040a1f8
...
```

* `r`: read (load)
* `w`: write (store)
* Hexadecimal memory addresses

---

## 📊 Sample Output

```
===== Simulator Configuration =====
L1_SIZE:                  32 KB
L1_ASSOC:                 2-way
BLOCKSIZE:                16 B
WRITE MISS POLICY:        write-allocate
WRITE HIT POLICY:         write-back
REPLACEMENT POLICY:       LRU

===== Simulation Results =====
a. Number of reads:             63640
b. Read misses:                 8322
c. Number of writes:            36360
d. Write misses:                3920
e. L1 miss rate:                12.3%
f. Total memory traffic:        10242
```

---

## 🔍 Research Focus

We explore how **data movement patterns and cache policies** impact energy efficiency. Our simulator serves as a testbed to evaluate techniques like:

* 🔄 SPADA: Adaptive dataflow-based replacement
* 🧠 Neuro-inspired and ML-driven replacement policies
* ⚡ Reducing compute–memory data traffic to save power

> 📉 **Goal**: Minimize energy lost due to external memory accesses by optimizing cache locality and trace-aware scheduling.

---

## 🧪 Future Extensions

* [ ] Implement SPADA (adaptive)
* [ ] Add energy profiling hooks
* [ ] LLVM compiler pass integration
* [ ] Machine learning-based cache replacement
* [ ] Support for compressed and encoded traces

---

## 📜 License

Licensed under the MIT License. See [LICENSE](./LICENSE) for details. 
