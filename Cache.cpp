#include "../include/Cache.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm> // Needed for std::sort

MemoryAccess stringToMemAccess(const std::string trace)
{
    MemoryAccess access;
    char type = trace[0];  // 'r' or 'w'
    access.loadStore = (type == 'r') ? "l" : "s";
    access.address = std::stoull(trace.substr(2), nullptr, 16);
    return access;
}

void Cache::updateLRU(int index, int blockIndex)
{
    unsigned long long int maxLRU = 0;
    for (unsigned long long i = 0; i < blocksPerSet; i++)
    {
        if (i == (unsigned long long)blockIndex) continue;
        if (cache[index][i].lruPosition > maxLRU)
        {
            maxLRU = cache[index][i].lruPosition;
        }
    }
    cache[index][blockIndex].lruPosition = maxLRU + 1;
}

bool Cache::read(MemoryAccess access, unsigned long long int indexMask,
                 unsigned long long int instructionCount, long long int & /*totalCycles*/)
{
    unsigned long long int blockNumber = access.address / blockSize;
    unsigned long long int index = blockNumber & indexMask;
    unsigned long long int tag = access.address / (blockSize * (indexMask + 1));

    for (unsigned long long int i = 0; i < blocksPerSet; i++)
    {
        if (cache[index][i].valid && cache[index][i].tag == tag)
        {
            updateLRU(index, i);
            return true;
        }
    }

    for (unsigned long long int i = 0; i < blocksPerSet; i++)
    {
        if (!cache[index][i].valid)
        {
            cache[index][i].valid = true;
            cache[index][i].dirty = false;
            cache[index][i].tag = tag;
            cache[index][i].fifoCount = instructionCount;
            cache[index][i].lruPosition = 0;
            updateLRU(index, i);
            return false;
        }
    }

    unsigned long long int victim = 0;
    if (replacementPolicy == "lru")
    {
        unsigned long long int minLRU = cache[index][0].lruPosition;
        for (unsigned long long int i = 1; i < blocksPerSet; i++)
        {
            if (cache[index][i].lruPosition < minLRU)
            {
                minLRU = cache[index][i].lruPosition;
                victim = i;
            }
        }
    }
    else
    {
        unsigned long long int minFIFO = cache[index][0].fifoCount;
        for (unsigned long long int i = 1; i < blocksPerSet; i++)
        {
            if (cache[index][i].fifoCount < minFIFO)
            {
                minFIFO = cache[index][i].fifoCount;
                victim = i;
            }
        }
    }

    if (cache[index][victim].dirty)
    {
        numWriteBacks++;
    }

    cache[index][victim].valid = true;
    cache[index][victim].dirty = false;
    cache[index][victim].tag = tag;
    cache[index][victim].fifoCount = instructionCount;
    cache[index][victim].lruPosition = 0;
    updateLRU(index, victim);

    return false;
}

bool Cache::write(MemoryAccess access, unsigned long long int indexMask,
                  unsigned long long int instructionCount, long long int & /*totalCycles*/)
{
    unsigned long long int blockNumber = access.address / blockSize;
    unsigned long long int index = blockNumber & indexMask;
    unsigned long long int tag = access.address / (blockSize * (indexMask + 1));

    for (unsigned long long int i = 0; i < blocksPerSet; i++)
    {
        if (cache[index][i].valid && cache[index][i].tag == tag)
        {
            if (writeHitPolicy == "write-back")
                cache[index][i].dirty = true;
            else
                cache[index][i].dirty = false;

            updateLRU(index, i);
            return true;
        }
    }

    if (writeMissPolicy == "write-allocate")
    {
        for (unsigned long long int i = 0; i < blocksPerSet; i++)
        {
            if (!cache[index][i].valid)
            {
                cache[index][i].valid = true;
                cache[index][i].dirty = (writeHitPolicy == "write-back");
                cache[index][i].tag = tag;
                cache[index][i].fifoCount = instructionCount;
                cache[index][i].lruPosition = 0;
                updateLRU(index, i);
                return false;
            }
        }

        unsigned long long int victim = 0;
        if (replacementPolicy == "lru")
        {
            unsigned long long int minLRU = cache[index][0].lruPosition;
            for (unsigned long long int i = 1; i < blocksPerSet; i++)
            {
                if (cache[index][i].lruPosition < minLRU)
                {
                    minLRU = cache[index][i].lruPosition;
                    victim = i;
                }
            }
        }
        else
        {
            unsigned long long int minFIFO = cache[index][0].fifoCount;
            for (unsigned long long int i = 1; i < blocksPerSet; i++)
            {
                if (cache[index][i].fifoCount < minFIFO)
                {
                    minFIFO = cache[index][i].fifoCount;
                    victim = i;
                }
            }
        }

        if (cache[index][victim].dirty)
        {
            numWriteBacks++;
        }

        cache[index][victim].valid = true;
        cache[index][victim].dirty = (writeHitPolicy == "write-back");
        cache[index][victim].tag = tag;
        cache[index][victim].fifoCount = instructionCount;
        cache[index][victim].lruPosition = 0;
        updateLRU(index, victim);

        return false;
    }

    return false;
}

void Cache::memoryAccess()
{
    unsigned long long int indexMask = setsNum - 1;

    for (const MemoryAccess &access : accessList)
    {
        long long int ignoredCycles = 0;
        if (access.loadStore == "l")
        {
            numReads++;
            if (!read(access, indexMask, numReads + numWrites, ignoredCycles))
                numReadMisses++;
        }
        else
        {
            numWrites++;
            if (!write(access, indexMask, numReads + numWrites, ignoredCycles))
                numWriteMisses++;
        }
    }
}

void Cache::printL1Contents()
{
    std::cout << "===== L1 contents =====" << std::endl;
    for (unsigned long long int setIdx = 0; setIdx < setsNum; setIdx++)
    {
        std::cout << "  set\t" << setIdx << ":\t";

        std::vector<CacheConstruct> tempSet = cache[setIdx];
        std::sort(tempSet.begin(), tempSet.end(),
                  [](const CacheConstruct &a, const CacheConstruct &b) {
                      return a.lruPosition > b.lruPosition;
                  });

        for (const auto &entry : tempSet)
        {
            if (entry.valid)
            {
                std::ostringstream oss;
                oss << std::hex << std::nouppercase << entry.tag;
                std::cout << oss.str();
                std::cout << " " << (entry.dirty ? "D" : " ") << "\t";
            }
            else
            {
                std::cout << "  \t";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void Cache::printStatistics()
{
    double totalAccesses = double(numReads + numWrites);
    double totalMisses = double(numReadMisses + numWriteMisses);
    double combinedMissRate = (totalAccesses > 0.0) ? (totalMisses / totalAccesses) : 0.0;
    unsigned long long int totalTraffic = numReadMisses + numWriteMisses + numWriteBacks;

    std::cout << "===== Simulation results (raw) =====" << std::endl;
    std::cout << "  a. number of L1 reads:\t\t" << numReads << std::endl;
    std::cout << "  b. number of L1 read misses:\t\t" << numReadMisses << std::endl;
    std::cout << "  c. number of L1 writes:\t\t" << numWrites << std::endl;
    std::cout << "  d. number of L1 write misses:\t\t" << numWriteMisses << std::endl;
    std::cout << "  e. number of swap requests:\t\t" << 0 << std::endl;
    std::cout << "  f. swap request rate:\t\t\t" << std::fixed << std::setprecision(4) << 0.0 << std::endl;
    std::cout << "  g. number of swaps:\t\t\t" << 0 << std::endl;
    std::cout << "  h. combined L1+VC miss rate:\t\t" << std::fixed << std::setprecision(4) << combinedMissRate << std::endl;
    std::cout << "  i. number writebacks from L1/VC:\t" << numWriteBacks << std::endl;
    std::cout << "  j. number of L2 reads:\t\t" << 0 << std::endl;
    std::cout << "  k. number of L2 read misses:\t\t" << 0 << std::endl;
    std::cout << "  l. number of L2 writes:\t\t" << 0 << std::endl;
    std::cout << "  m. number of L2 write misses:\t\t" << 0 << std::endl;
    std::cout << "  n. L2 miss rate:\t\t\t" << std::fixed << std::setprecision(4) << 0.0 << std::endl;
    std::cout << "  o. number of writebacks from L2:\t" << 0 << std::endl;
    std::cout << "  p. total memory traffic:\t\t" << totalTraffic << std::endl;
}

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
    cache.resize(setsNum);
    for (unsigned long long int i = 0; i < setsNum; i++)
    {
        cache[i].resize(blocksPerSet);
        for (unsigned long long int w = 0; w < blocksPerSet; w++)
        {
            cache[i][w].valid = false;
            cache[i][w].dirty = false;
            cache[i][w].tag = 0;
            cache[i][w].lruPosition = 0;
            cache[i][w].fifoCount = 0;
        }
    }
}

Cache::~Cache() {}

void Cache::parseTrace(const std::string trace)
{
    MemoryAccess access = stringToMemAccess(trace);
    accessList.push_back(access);
}
