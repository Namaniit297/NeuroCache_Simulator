#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>

/**
 * RidxLRU Replacement Policy
 * Implements row-index-aware LRU eviction used in SPADA.
 * For each cache set, tracks which A-row most recently accessed each way.
 */
class RidxLRU {
public:
    // Constructor: specify number of sets and ways (associativity)
    RidxLRU(unsigned long long sets_, unsigned long long ways_)
        : sets(sets_), ways(ways_), lastARow(sets_, std::vector<int>(ways_, -1)) {}

    /**
     * Loads a mapping file: each line "0xADDR A_ROW"
     * This tells the policy which A-row accessed which memory address.
     */
    void loadMap(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open addr_to_arow.map: " + filepath);
        }
        std::string line;
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string addrStr;
            int arow;
            if (!(iss >> addrStr >> arow)) continue;
            unsigned long long addr = std::stoull(addrStr, nullptr, 16);
            addrToARow[addr] = arow;
        }
    }

    /**
     * Selects a victim block from the set: evict the block
     * last used by the smallest (oldest) A-row.
     */
    int getVictim(unsigned long long setIndex) const {
        int minArow = std::numeric_limits<int>::max();
        int victim = 0;
        for (unsigned long long way = 0; way < ways; ++way) {
            if (lastARow[setIndex][way] < minArow) {
                minArow = lastARow[setIndex][way];
                victim = way;
            }
        }
        return victim;
    }

    /**
     * Updates the policy after a block is accessed or replaced.
     * Uses addr → arow map to find which A-row caused this access.
     */
    void touch(unsigned long long setIndex, unsigned long long wayIndex, unsigned long long blockAddr) {
        int arow = -1;
        auto it = addrToARow.find(blockAddr);
        if (it != addrToARow.end()) {
            arow = it->second;
        }
        lastARow[setIndex][wayIndex] = arow;
    }

private:
    unsigned long long sets, ways;
    std::vector<std::vector<int>> lastARow;  // [set][way] → a_row

    // Global map of blockAddr → A-row
    std::unordered_map<unsigned long long, int> addrToARow;
};

