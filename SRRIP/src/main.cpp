#include <iostream>
#include <string>
#include "../include/Cache.hpp"

int main(int argc, char **argv)
{
    if (argc != 7)
    {
        std::cerr << "Usage:\n"
                  << "./cacheSim <no_sets> <no_blocks_per_set> <block_size> "
                  << "<write_miss_policy> <write_hit_policy> <replacement_policy> < <trace_file>" 
                  << std::endl;
        return 1;
    }

    // Parse command line arguments
    int setNum = std::stoi(argv[1]);
    int blocksPerSet = std::stoi(argv[2]);
    int blockSize = std::stoi(argv[3]);
    std::string writeMissPolicy = argv[4];
    std::string writeHitPolicy = argv[5];
    std::string replacementPolicy = argv[6];

    // Validate replacement policy
    if (replacementPolicy != "LRU" && replacementPolicy != "SRRIP") {
        std::cerr << "Error: Unsupported replacement policy. Use 'LRU' or 'SRRIP'." << std::endl;
        return 1;
    }

    // Create the Cache
    Cache* cache = new Cache(setNum, blocksPerSet, blockSize, writeHitPolicy, writeMissPolicy, replacementPolicy);

    // Read trace lines and feed to cache
    std::string traceLine;
    while (std::getline(std::cin, traceLine)) {
        cache->parseTrace(traceLine);
    }

    // Perform all memory accesses
    cache->memoryAccess();

    // Print simulator configuration
    std::cout << "===== Simulator configuration =====" << std::endl;
    std::cout << "  L1_SIZE:\t\t" << setNum * blocksPerSet * blockSize << std::endl;
    std::cout << "  L1_ASSOC:\t\t" << blocksPerSet << std::endl;
    std::cout << "  L1_BLOCKSIZE:\t\t" << blockSize << std::endl;
    std::cout << "  VC_NUM_BLOCKS:\t0" << std::endl;
    std::cout << "  L2_SIZE:\t\t0" << std::endl;
    std::cout << "  L2_ASSOC:\t\t0" << std::endl;
    std::cout << "  trace_file:\t\t" << "gcc_trace.txt" << std::endl << std::endl;

    // Print final cache state and statistics
    cache->printL1Contents();
    cache->printStatistics();

    delete cache;
    return 0;
}
