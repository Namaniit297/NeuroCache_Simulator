#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include "../include/RidxLRU.hpp"

struct CacheBlock {
    bool valid = false;
    bool dirty = false;
    unsigned long long tag = 0;
};

int main(int argc, char** argv) {
    if (argc != 5) {
        std::cerr << "Usage: ./ridxSim <num_sets> <ways> <block_size> <addr_to_arow.map> < trace.txt\n";
        return 1;
    }

    const unsigned long long numSets = std::stoull(argv[1]);
    const unsigned long long numWays = std::stoull(argv[2]);
    const unsigned long long blockSize = std::stoull(argv[3]);
    const std::string mapFile = argv[4];

    std::vector<std::vector<CacheBlock>> cache(numSets, std::vector<CacheBlock>(numWays));
    RidxLRU policy(numSets, numWays);
    policy.loadMap(mapFile);

    // Statistics
    unsigned long long readCount = 0, writeCount = 0;
    unsigned long long readMiss = 0, writeMiss = 0, writeBacks = 0;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        char op;
        std::string addrStr;
        iss >> op >> addrStr;

        unsigned long long addr = std::stoull(addrStr, nullptr, 16);
        unsigned long long blockAddr = addr / blockSize;
        unsigned long long setIdx = blockAddr % numSets;
        unsigned long long tag = blockAddr / numSets;

        bool hit = false;
        int hitWay = -1;

        for (int w = 0; w < numWays; ++w) {
            if (cache[setIdx][w].valid && cache[setIdx][w].tag == tag) {
                hit = true;
                hitWay = w;
                break;
            }
        }

        if (op == 'r') readCount++;
        else           writeCount++;

        if (hit) {
            if (op == 'w') cache[setIdx][hitWay].dirty = true;
            policy.touch(setIdx, hitWay, blockAddr);
        } else {
            if (op == 'r') readMiss++;
            else           writeMiss++;

            int victim = policy.getVictim(setIdx);
            if (cache[setIdx][victim].valid && cache[setIdx][victim].dirty)
                writeBacks++;

            cache[setIdx][victim].valid = true;
            cache[setIdx][victim].dirty = (op == 'w');
            cache[setIdx][victim].tag = tag;
            policy.touch(setIdx, victim, blockAddr);
        }
    }

    // Print statistics
    std::cout << "===== RidxLRU Simulation Results =====\n";
    std::cout << "Reads:           " << readCount << "\n";
    std::cout << "Read Misses:     " << readMiss << "\n";
    std::cout << "Writes:          " << writeCount << "\n";
    std::cout << "Write Misses:    " << writeMiss << "\n";
    std::cout << "Writebacks:      " << writeBacks << "\n";
    double missRate = (readMiss + writeMiss) / static_cast<double>(readCount + writeCount);
    std::cout << "Combined Miss %: " << std::fixed << std::setprecision(4) << missRate * 100 << "%\n";

    return 0;
}

