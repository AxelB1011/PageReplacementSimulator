#ifndef POLICIES_HPP
#define POLICIES_HPP

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <list>
#include <unordered_map>

class FIFO {
    std::vector<std::pair<unsigned, char> > eventVector;        //intializes fixed array size with nFrames
public:
    void place(std::string, int, std::string sysChoice);
    size_t read, write;
};

class LRUCache {
    std::unordered_map<unsigned, char> dirty;
    std::list<std::pair<unsigned, char> > recent; // Recently used list
    std::unordered_map<unsigned, std::list<std::pair<unsigned, char> >::iterator> cache;
    long long unsigned int cacheSize;
    std::list<std::pair<unsigned, char> >::iterator itr;
public:
    LRUCache(int);
    void page(std::pair<unsigned, char>, std::string sysChoice);
    void display();
    int read, write; // Read/Write counters
}; 

class SFIFO {
    std::unordered_map<unsigned, char> disk;
    std::list<std::pair<unsigned, char> > recent; // Recently used list
    std::unordered_map<unsigned, std::list<std::pair<unsigned, char> >::iterator> sMem;
    size_t pSize; //primary buffer size
    size_t sSize; //secondary buffer size
    std::vector<std::pair<unsigned, char> > eventVector;
    std::list<std::pair<unsigned, char> >::iterator itL; //iterator to lru list
    std::pair<unsigned, char> temp; //temp var to store front elem of fifo
public:
    void simulate(std::string, int, int, std::string sysChoice);
    void setSecondarySize(size_t x){
        sSize = x;
    }
    size_t getSecondarySize(){
        return sSize;
    }
    void setPrimarySize(size_t x){
        pSize = x;
    }
    size_t getPrimarySize(){
        return pSize;
    }
    size_t read, write; // Read/Write counters
};
#endif