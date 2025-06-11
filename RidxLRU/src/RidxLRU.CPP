#include "../include/Cache.hpp"
#include "../include/RidxLRU.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

// === Utility: Convert trace string to MemoryAccess ===
MemoryAccess stringToMemAccess(const std::string trace)
{
    MemoryAccess access;
    char type = trace[0];  // 'r' or 'w'
    access.loadStore = (type == 'r') ? "l" : "s";
    access.address = std::stoull(trace.substr(2), nullptr, 16);
    return access;
}

// === Constructor ===
Cache::Cache(unsigned long long int sets_number,
             unsigned long long int blocks_per_set,
             unsigned long long int block_size,
             std::string write_hit_policy,
             std::string write_miss_policy,
             std::string replacement_policy)
    : setsNum(sets_number),
      blocksPerSet(blocks_per_set),
      blockSize(block_size),
      writeHitPolicy(std::move(write_hit_policy)),
      writeMissPolicy(std::move(write_miss_policy)),
      replacementPolicy(std::move(replacement_policy))
{
    cache.resize(setsNum, std::vector<CacheConstruct>(blocksPerSet));
    if (replacementPolicy == "ridxlru") {
        ridxLRU = new RidxLRU(setsNum, blocksPerSet);
        ridxLRU->loadMap("traces/addr_to_arow.map");
    }
}

// === Destructor ===
Cache::~Cache()
{
    if (ridxLRU) delete ridxLRU;
}

// === Trace Parser ===
void Cache::parseTrace(const std::string trace)
{
    accessList.push_back(stringToMemAccess(trace));
}

// === READ ===
bool Cache::read(MemoryAccess access, unsigned long long int indexMask,
                 unsigned long long int instructionCount, long long int & /*totalCycles*/)
{
    unsigned long long blockNumber = access.address / blockSize;
    unsigned long long index = blockNumber & indexMask;
    unsigned long long tag = access.address / (blockSize * (indexMask + 1));

    for (unsigned long long i = 0; i < blocksPerSet; ++i) {
        if (cache[index][i].valid && cache[index][i].tag == tag) {
            ridxLRU->touch(index, i, blockNumber);
            return true;
        }
    }

    for (unsigned long long i = 0; i < blocksPerSet; ++i) {
        if (!cache[index][i].valid) {
            cache[index][i] = {false, true, tag, 0, instructionCount};
            ridxLRU->touch(index, i, blockNumber);
            return false;
        }
    }

    unsigned long long victim = ridxLRU->getVictim(index);

    if (cache[index][victim].dirty) numWriteBacks++;

    cache[index][victim] = {false, true, tag, 0, instructionCount};
    ridxLRU->touch(index, victim, blockNumber);
    return false;
}

// === WRITE ===
bool Cache::write(MemoryAccess access, unsigned long long int indexMask,
                  unsigned long long int instructionCount, long long int & /*totalCycles*/)
{
    unsigned long long blockNumber = access.address / blockSize;
    unsigned long long index = blockNumber & indexMask;
    unsigned long long tag = access.address / (blockSize * (indexMask + 1));

    for (unsigned long long i = 0; i < blocksPerSet; ++i) {
        if (cache[index][i].valid && cache[index][i].tag == tag) {
            cache[index][i].dirty = (writeHitPolicy == "write-back");
            ridxLRU->touch(index, i, blockNumber);
            return true;
        }
    }

    if (writeMissPolicy == "write-allocate") {
        for (unsigned long long i = 0; i < blocksPerSet; ++i) {
            if (!cache[index][i].valid) {
                cache[index][i] = {
                    (writeHitPolicy == "write-back"), true, tag, 0, instructionCount
                };
                ridxLRU->touch(index, i, blockNumber);
                return false;
            }
        }

        unsigned long long victim = ridxLRU->getVictim(index);
        if (cache[index][victim].dirty) numWriteBacks++;

        cache[index][victim] = {
            (writeHitPolicy == "write-back"), true, tag, 0, instructionCount
        };
        ridxLRU->touch(index, victim, blockNumber);
        return false;
    }

    return false;
}

// === MEMORY ACCESS LOOP ===
void Cache::memoryAccess()
{
    unsigned long long indexMask = setsNum - 1;

    for (const auto &access : accessList) {
        long long int ignored = 0;
        if (access.loadStore == "l") {
            numReads++;
            if (!read(access, indexMask, numReads + numWrites, ignored))
                numReadMisses++;
        } else {
            numWrites++;
            if (!write(access, indexMask, numReads + numWrites, ignored))
                numWriteMisses++;
        }
    }
}

// === UNUSED: LRU Logic ===
void Cache::updateLRU(int, int) {
    // Not used in RidxLRU
}

// === PRINT L1 CONTENT ===
void Cache::printL1Contents()
{
    std::cout << "===== L1 contents =====\n";
    for (unsigned long long set = 0; set < setsNum; ++set) {
        std::cout << "  set " << set << ":\t";
        for (unsigned long long way = 0; way < blocksPerSet; ++way) {
            if (cache[set][way].valid) {
                std::ostringstream oss;
                oss << std::hex << std::nouppercase << cache[set][way].tag;
                std::cout << oss.str() << (cache[set][way].dirty ? " D" : "  ") << "\t";
            } else {
                std::cout << "   \t";
            }
        }
        std::cout << std::endl;
    }
}

// === PRINT STATS ===
void Cache::printStatistics()
{
    double totalAccesses = numReads + numWrites;
    double totalMisses = numReadMisses + numWriteMisses;
    double missRate = totalAccesses > 0 ? totalMisses / totalAccesses : 0.0;
    unsigned long long totalTraffic = numReadMisses + numWriteMisses + numWriteBacks;

    std::cout << "===== Simulation results (raw) =====\n";
    std::cout << "  a. number of L1 reads:\t\t" << numReads << "\n";
    std::cout << "  b. number of L1 read misses:\t\t" << numReadMisses << "\n";
    std::cout << "  c. number of L1 writes:\t\t" << numWrites << "\n";
    std::cout << "  d. number of L1 write misses:\t\t" << numWriteMisses << "\n";
    std::cout << "  e. number of swap requests:\t\t0\n";
    std::cout << "  f. swap request rate:\t\t\t0.0000\n";
    std::cout << "  g. number of swaps:\t\t\t0\n";
    std::cout << "  h. combined L1+VC miss rate:\t\t" << std::fixed << std::setprecision(4) << missRate << "\n";
    std::cout << "  i. number writebacks from L1/VC:\t" << numWriteBacks << "\n";
    std::cout << "  j. number of L2 reads:\t\t0\n";
    std::cout << "  k. number of L2 read misses:\t\t0\n";
    std::cout << "  l. number of L2 writes:\t\t0\n";
    std::cout << "  m. number of L2 write misses:\t\t0\n";
    std::cout << "  n. L2 miss rate:\t\t\t0.0000\n";
    std::cout << "  o. number of writebacks from L2:\t0\n";
    std::cout << "  p. total memory traffic:\t\t" << totalTraffic << "\n";
}
