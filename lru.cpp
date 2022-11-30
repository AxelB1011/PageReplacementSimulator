#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <list>
#include <unordered_map>
#include "policies.hpp"

LRUCache::LRUCache(int n) {
    cacheSize = n;
}

void LRUCache::page(std::pair<unsigned, char> x, std::string sysChoice) { // pages x into the table
    if(cache.find(x.first) == cache.end()){ // page not found in cache
        if(sysChoice == "debug"){
            std::cout << "Page " << x.first << " not found in cache" << std::endl;
        }
        if(recent.size() == cacheSize){ // cache is full, so delete least recently used page
            if(sysChoice == "debug"){
                std::cout << "Cache Full" << std::endl;
            }
            std::pair<unsigned, char> least = recent.back(); // least = least recently used
            if(sysChoice == "debug"){
                std::cout << "Deleting Page " << least.first << std::endl;
            }
            if(dirty[least.first] == 'W'){ // If least = W while removing, increment w
                write++;
                dirty[least.first] = 'R';
            }
            recent.pop_back();
            cache.erase(least.first); // delete the page of the least recently used
        }
    } else { // page is present in cache
        if(sysChoice == "debug"){
            std::cout << "Page " << x.first << " is present in cache with " << x.second << std::endl;
        }
        for(itr = recent.begin(); itr != recent.end(); itr++){ // dirty bit section
            if(x.first == (*itr).first){ // if address matches the paged event
                if(x.second == 'W' && (*itr).second == 'R'){ // if the paged event has a W while the existing page has an R
                    if(sysChoice == "debug"){
                        std::cout << "Clean page at: " << (*itr).first  << " " << (*itr).second << std::endl;
                    }
                    (*itr).second = x.second; // overwrite the R with the W
                    if(sysChoice == "debug"){
                        std::cout << "Dirty page at: " << (*itr).first  << " " << (*itr).second << std::endl;
                    }
                }
            }
        }
        recent.erase(cache[x.first]); // remove the page from wherever it is in the linked list
    }
    
    if((x.second == 'R' || x.second == 'W') && cache.find(x.first) == cache.end()){ // If the paged event is not in the cache, add to read
        read++;
    }

    dirty.insert(x); // add the page to the dirty bit map
    recent.push_front(x); // update recency of the referenced page

    if(sysChoice == "debug"){
        std::cout << "Most Recent Page: " << x.first << std::endl;
    }

    if(x.second == 'W' && dirty[x.first] == 'R'){ // dirty bit verification
        dirty[x.first] = 'W';
    }

    cache[x.first] = recent.begin(); // inside the cache map, set the value of page address as a pointer to the linked list
}

void LRUCache::display() {
    for(itr = recent.begin(); itr != recent.end(); itr++){
        std::cout << (*itr).first << " " << (*itr).second << std::endl; // Outputs every page and read/write
    }
}

void lru(std::string fileChoice, int nFrames, std::string sysChoice) {
    LRUCache lruCache(nFrames); // new LRUCache
    lruCache.read = 0;
    lruCache.write = 0;
    unsigned addr;
    char rw;
    int traceCount = 0;

    std::pair<unsigned, char> event;

    FILE* file;

    if(fileChoice == "bzip.trace") {
        file = fopen("bzip.trace", "r");
    } else if(fileChoice == "sixpack.trace") {
        file = fopen("sixpack.trace", "r");
    } else {
        throw std::runtime_error("ERROR Incorrect filename");
    }

    while (!feof(file)) { 

        if(traceCount == 1000000) { // trace count does not exceed 1,000,000
            break;
        }

        fscanf(file, "%x %c", &addr, &rw); // scan address and r/w character
        addr = addr>>12; // shift address left to fit page table
        event.first = addr;
        event.second = rw;

        lruCache.page(event, sysChoice); // page the event

        traceCount++;
    }
    
    fclose(file);

    std::cout << "Total Memory Frames: " << nFrames << std::endl;
    std::cout << "Trace Count: " << traceCount << std::endl;
    std::cout << "Total Disk Reads: " << lruCache.read << std::endl;
    std::cout << "Total Disk Writes: " << lruCache.write << std::endl;

    return;
}//g++ -std=c++17 -Wall lru.cpp