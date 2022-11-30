#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdio.h>
#include <unordered_map>
#include "policies.hpp"

void FIFO::place(std::string fileChoice, int nFrames, std::string sysChoice) {
    unsigned addr;
    char rw;
    int traceCount = 0, emptyIndex = 0;    //stores amount of traces being looped through and the index of the empty frame

    bool emptyCount = false, test = false;   //stores bool values for when the array has an empty frame and when the page is inside tables

    int cacheIn = 0, cacheDelete = 0, cacheOpen = 0;

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
    eventVector.resize(nFrames);
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

        for(int i = 0;i < nFrames;i++) {
            if(event.first == eventVector[i].first) {       //checks if address is already inside cache
                if(sysChoice == "debug") {
                    std::cout << "inside cache " << event.first << std::endl;
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

        if(!test) {  //page not inside cache
            if(emptyCount){
                if(sysChoice == "debug") {
                    std::cout << "Open space in cache" << std::endl;
                }
              
                eventVector[emptyIndex] = event;
                read++;                           //increments read if a read was encountered in new page
                cacheOpen++;
            } else {
                if(sysChoice == "debug") {
                    std::cout << "cache full" << std::endl;
                }
                

                cacheDelete++;

                if(eventVector[0].second == 'W') {  //increments write if a write was encountered in oldest page
                    write++;
                }

                read++;                      //increments read if a read was encountered in new page

                for (int i = 0; i < nFrames; ++i) {
                    eventVector[i] = eventVector[i + 1];          //shifts entire array up one
                    if(i == nFrames - 1) {
                        eventVector[i].first = 0;
                        eventVector[i].second = ' ';
                    } 
                }
                

                eventVector[nFrames - 1] =  event;            //loads new page to the end of cache
            }
        }        

        test = false;
        emptyCount = false;     //resets bool values for next iteration of loop
        traceCount++;

        /*
        if(traceCount == 1000) {
            break;                  //used for debugging fixed number of traces
        }*/
    }
    
    fclose(file);

    std::cout << "Memory Frames: " << nFrames << std::endl;
    std::cout << "Trace count: " << traceCount << std::endl;
    std::cout << "Read Count: " << read << std::endl;
    std::cout << "Write Count: " << write << std::endl;
    //std::cout << "Cache ins: " << cacheIn << std::endl;
    //std::cout << "Cache opens: " << cacheOpen << std::endl;
    //std::cout << "Cache deletes: " << cacheDelete << std::endl;

    if(sysChoice == "debug") {
        for (int i = 0;i < nFrames;i++) {
            std::cout << eventVector[i].first << " " << eventVector[i].second << " ";   //prints out page table for debugging
        }
    }
    std::cout << '\n';
}

void fifo(std::string fileChoice, int nFrames, std::string sysChoice){
    FIFO fifoMemory;
    fifoMemory.read = 0;
    fifoMemory.write = 0;
    fifoMemory.place(fileChoice, nFrames, sysChoice);
}