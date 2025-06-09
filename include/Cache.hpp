#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <map>

struct MemoryAccess {
    std::string loadStore;
    unsigned long long int address;
};

struct CacheConstruct {
    bool dirty, valid;
    unsigned long long int tag, lruPosition, fifoCount;
};

class Cache {
private:
    unsigned long long int setsNum;
    unsigned long long int blocksPerSet;
    unsigned long long int blockSize;
    std::string writeHitPolicy;
    std::string writeMissPolicy;
    std::string replacementPolicy;

    friend MemoryAccess stringToMemAccess(const std::string trace);

    std::vector<MemoryAccess> accessList;
    std::vector<std::vector<CacheConstruct>> cache;

    // === Simulation statistics ===
    unsigned long long numReads = 0;
    unsigned long long numReadMisses = 0;
    unsigned long long numWrites = 0;
    unsigned long long numWriteMisses = 0;
    unsigned long long numWriteBacks = 0;

public:
    // Constructor
    Cache(unsigned long long int sets_number,
          unsigned long long int blocks_per_set,
          unsigned long long int block_size,
          std::string write_hit_policy,
          std::string write_miss_policy,
          std::string replacement_policy);

    // Destructor
    ~Cache();

    // Load trace line into accessList
    void parseTrace(const std::string trace);

    // Process all memory accesses and print results
    void memoryAccess();

    // Core memory read/write logic
    bool read(MemoryAccess access, unsigned long long int indexMask,
              unsigned long long int instructionCount, long long int &totalCycles);

    bool write(MemoryAccess access, unsigned long long int indexMask,
               unsigned long long int instructionCount, long long int &totalCycles);

    // Replacement policy management
    void updateLRU(int index, int blockIndex);

    // Print L1 cache state after simulation
    void printL1Contents();

    // Print simulation statistics
    void printStatistics();
};
