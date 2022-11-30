#include "vms.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <list>
#include <unordered_map>

int main(int argc, char** argv) {
    if (strcmp(argv[3], "fifo") == 0) { // if FIFO
        fifo(argv[1], std::stoi(argv[2]), argv[4]); // ./memsim bzip.trace 64 fifo quiet
    }
    else if (strcmp(argv[3], "lru") == 0) { // if LRU
        lru(argv[1], std::stoi(argv[2]), argv[4]); // ./memsim bzip.trace 64 lru quiet
    }
    else if (strcmp(argv[3], "vms") == 0) { // if VMS
        vms(argv[1], std::stoi(argv[2]), std::stoi(argv[4]), argv[5]); // ./memsim bzip.trace 64 vms 50 quiet
    }
    return 0;
}
