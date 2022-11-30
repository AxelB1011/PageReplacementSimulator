#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdio.h>
#include <string>
#include <list>
#include <unordered_map>
#include "fifo.cpp"
#include "lru.cpp"

void SFIFO::simulate(std::string fileChoice, int nFrames, int par, std::string sysChoice){
    SFIFO vmsMemory;
    vmsMemory.read = 0;
    vmsMemory.write = 0;
    unsigned addr;
    char rw;
    int traceCount = 0;
    std::pair<unsigned, char> event;                    //variable to store one trace during loop
    FILE* file;
    if(fileChoice == "bzip.trace") {             //To let the user choose which zip file they want to use
        file = fopen ("bzip.trace", "r");
    } else if(fileChoice == "sixpack.trace") {
        file = fopen ("sixpack.trace", "r");
    } else {
        throw std::runtime_error("ERROR Incorrect filename");
    }
    int end;     //stores value for iterating through files
    bool emptyCount = false, test = false; 
    int c1=0, c2=0, c3=0, c4=0, c5=0;
    int cacheIn = 0, cacheDelete = 0, cacheOpen = 0;
    int emptyIndex = 0;    //stores amount of traces being looped through and the index of the empty frame
    //formula in slides
    vmsMemory.setSecondarySize(((nFrames*par)/100));
    vmsMemory.setPrimarySize(nFrames - vmsMemory.getSecondarySize());
    if(vmsMemory.getSecondarySize() == 0){
        fifo(fileChoice, nFrames, sysChoice);
        return;
    }
    else if(vmsMemory.getPrimarySize() == 0){
        lru(fileChoice, nFrames, sysChoice);
        return;
    }
    eventVector.resize(int(vmsMemory.getPrimarySize()));
    while ((end = fgetc(file)) != EOF) {
        if(traceCount == 1000000) {             //if all traces are read then break
            break;
        }
        fscanf(file, "%x %c", &addr, &rw);       //gathers trace then shifts address left by 12 bits
        addr = addr>>12;
        if(addr == 0) {
            addr = 1215752192;           //set all page numbers that are equal to 0 to 2 so the loop can find empty frames properly
        }
        event.first = addr;
        event.second = rw;

        //1
        for(size_t i = 0;i < vmsMemory.getPrimarySize();i++) {
            if(event.first == eventVector[i].first) {       //checks if address is already inside cache
                if(sysChoice == "debug") {
                    std::cout << "CASE 1" << std::endl;
                    std::cout << "inside cache " << event.first << std::endl;
                    c1++;
                }
                if(event.second == 'W' && eventVector[i].second == 'R') {
                    eventVector[i].second = 'W';                        //handling dirty bit
                }
                test = true;   //set to true if is inside cache
                cacheIn++;
            }
            if(eventVector[i].first == 0 && !emptyCount){
                emptyIndex = i;       //gets the index of empty space
                emptyCount = true;   //set to true if there was an empty space found
            }
        }
        //if address was not found inside fifo then we can move on to the rest of the cases
        if (!test){
            //2
            if(emptyCount){
                if(sysChoice == "debug") {
                    std::cout << "CASE 2" << std::endl;
                    std::cout << "Open space in FIFO cache " << event.first << std::endl;
                    c2++;
                }
                eventVector[emptyIndex] = event;
                if (event.second == 'W') { //if r in disk then update that r to w
                    disk[event.first] = 'W';
                }
                vmsMemory.read++;                           //increments read if a read was encountered in new page
                cacheOpen++;
            } 
            
            //3
            else if (sMem.find(event.first)==sMem.end() && recent.size()<vmsMemory.getSecondarySize()){
                //eject from front of fifo
                temp = eventVector.front();
                if(sysChoice == "debug") {
                    std::cout << "CASE 3" << std::endl;
                    std::cout << "fifo cache full " << event.first << std::endl;
                    c3++;
                }
                cacheDelete++;
                vmsMemory.read++;                      //increments read if a read was encountered in new page
                for (size_t i = 0; i < vmsMemory.getPrimarySize(); ++i) {
                    eventVector[i] = eventVector[i + 1];          //shifts entire array up one
                    if(i == vmsMemory.getPrimarySize() - 1) {
                        eventVector[i].first = 0;
                        eventVector[i].second = ' ';
                    } 
                }
                //eviction from fifo done, now make it the newest node of lru
                recent.push_front(temp); // update reference
                sMem[temp.first] = recent.begin();  
                eventVector[vmsMemory.getPrimarySize() - 1] =  event;            //loads new page to the end of fifo cache
                if (temp.second == 'W') { //if r in disk then update that r to w
                    disk[temp.first] = 'W';
                }
            }
            //4
            else if (sMem.find(event.first)!=sMem.end()){
                //eject from front of fifo
                temp = eventVector.front();
                if(sysChoice == "debug") {
                    std::cout << "CASE 4" << std::endl;
                    std::cout << "present in lru " <<event.first <<  std::endl;
                    c4++;
                }
                cacheDelete++; 
                for (size_t i = 0; i < vmsMemory.getPrimarySize(); ++i) {
                    eventVector[i] = eventVector[i + 1];          //shifts entire array up one
                    if(i == vmsMemory.getPrimarySize() - 1) {
                        eventVector[i].first = 0;
                        eventVector[i].second = ' ';
                    } 
                }
                for(itL = recent.begin(); itL != recent.end(); itL++){
                    if(event.first == (*itL).first){
                        if(event.second == 'W' && (*itL).second == 'R'){
                            // (*itr).second = event.second;
                            disk[event.first] = event.second;
                            cacheIn++;
                        }
                    }
                }
                cacheOpen++;
                recent.erase(sMem[event.first]);
                recent.push_front(temp); // update reference  

                eventVector[vmsMemory.getPrimarySize() - 1] =  event;            //loads new page to the end of fifo cache
                if (temp.second == 'W') { //if r in disk then update that r to w
                    disk[temp.first] = 'W';
                }
                sMem[temp.first] = recent.begin();     
            }
            //5
            else if (sMem.find(event.first)==sMem.end()){
                //evict from lru
                vmsMemory.read++;
                cacheDelete++;
                std::pair<unsigned, char> least = recent.back(); // least = least recently used
                if (disk[least.first] == 'W') {
                    vmsMemory.write++;
                    disk[least.first] = 'R'; //reset dirty bit
                }
                recent.pop_back();
                sMem.erase(least.first); // delete the page of the least recently used
                //eject from front of fifo
                temp = eventVector.front();
                if(sysChoice == "debug") {
                    std::cout << "CASE 5" << std::endl;
                    std::cout << "fifo and lru cache full " << event.first << std::endl;
                    c5++;
                }
                cacheDelete++;
                for (size_t i = 0; i < vmsMemory.getPrimarySize(); ++i) {
                    eventVector[i] = eventVector[i + 1];          //shifts entire array up one
                    if(i == vmsMemory.getPrimarySize() - 1) {
                        eventVector[i].first = 0;
                        eventVector[i].second = ' ';
                    } 
                }
                //eviction from fifo done, now make it the newest node of lru
                recent.push_front(temp); // update reference

                eventVector[vmsMemory.getPrimarySize() - 1] =  event;            //loads new page to the end of fifo cache
                if (temp.second == 'W') { //if r in disk then update that r to w
                    disk[temp.first] = 'W';
                }
                sMem[temp.first] = recent.begin();
            }
        }
        /*
        1. In fifo - 1
        -Update W
        2. Not in fifo, fifo not full - 5 write++?
        -Add to back of fifo
        3. Not in fifo, fifo full, lru not full - 4
        -Eject from front of fifo, move to lru making it newest lru page
        4. In lru, fifo full - 2 write++? update read++ as an else statement after updating write
        -Update W, move page at front of fifo to lru, then move this to back of fifo, 
        5. Not in fifo or lru, both full - 3 at the start read++
        -Evict from lru, move page at front of fifo to lru, add this to back of fifo
        */
        test = false;
        emptyCount = false;     //resets bool values for next iteration of loop

        traceCount++;
    }   
    fclose(file);
    std::cout << "Memory Frames: " << nFrames << std::endl;
    std::cout << "Trace count: " << traceCount << std::endl;
    std::cout << "Read Count: " << vmsMemory.read << std::endl;
    std::cout << "Write Count: " << vmsMemory.write << std::endl;
    if(sysChoice == "debug") {
        std::cout << "Cache ins: " << cacheIn << std::endl;
        std::cout << "Cache opens: " << cacheOpen << std::endl;
        std::cout << "Cache deletes: " << cacheDelete << std::endl;
        std::cout << "Cases" << c1 << " " << c2 << " " << c3 << " " << c4 << " " << c5 << std::endl;
    }
    std::cout << '\n';
}

void vms(std::string fileChoice, int nFrames, int par, std::string sysChoice){
    SFIFO vmsMemory1;
    vmsMemory1.read = 0;
    vmsMemory1.write = 0;
    vmsMemory1.simulate(fileChoice, nFrames, par, sysChoice);
}