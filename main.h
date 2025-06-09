#include <iostream>
#include <string>
#include "../include/Cache.hpp"

int main(int argc, char **argv)
{
    if (argc != 7)
    {
        std::cerr << "Usage: \n./cacheSim <no_sets> <no_blocks_per_set> <block_size> <write_policy> <write_strategy> <replacement_policy> < <trace_file>" << std::endl;
        return 1;
    }

    int setNum = std::stoi(argv[1]);
    int blocksPerSet = std::stoi(argv[2]);
    int blockSize = std::stoi(argv[3]);
    std::string writeMissPolicy = argv[4];
    std::string writeHitPolicy = argv[5];
    std::string replacementPolicy = argv[6];

    Cache* cache = new Cache(setNum, blocksPerSet, blockSize, writeHitPolicy, writeMissPolicy, replacementPolicy);

    std::string traceLine;
    while (std::getline(std::cin, traceLine)) {
        cache->parseTrace(traceLine);
    }

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

    // Print L1 contents
    cache->printL1Contents();

    // Print L1 statistics
    cache->printStatistics();

    delete cache;
    return 0;
}
