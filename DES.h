#ifndef DES_H
#define DES_H

#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <list>
#include "randomizer.h"


typedef enum {RUNNG, READY, BLOCK, CREATED, Done} state_t;
class Randomizer;

class DES {
public:
    struct PROCESS {
        int id; 
        int total_compute;
        int orig_total_compute;
        int cpu_burst;
        int io_burst;
        int state_ts;
        int static_priority;
        int dynamic_priority;
        int finish_time;
        int turnaround_time;
        int io_time;
        int cpu_wait_time;
        int arrival_time;
        int current_burst;
        PROCESS(int id_input, int tc, int cb, int io, int statprio, int at) : 
            id(id_input), 
            total_compute(tc),
            orig_total_compute(tc),
            cpu_burst(cb), 
            io_burst(io), 
            state_ts(at),
            static_priority(statprio),
            dynamic_priority(statprio-1),
            arrival_time(at) {}
    };
    struct EVENT {
        int timestamp;
        PROCESS* process;
        int transition;
        int oldstate;
        int newstate;
        EVENT() : timestamp(0), process(nullptr), transition(0), oldstate(CREATED), newstate(READY) {}
        EVENT(int t, PROCESS* p, int s, int os, int ns) : 
            timestamp(t), process(p), transition(s), oldstate(os), newstate(ns) {}
    };


    Randomizer& randomizer;  // Reference to Randomizer

    DES(Randomizer& rand);

    std::string printQueue();
    std::list<PROCESS*> readInput(const std::string& inputFile, int maxprio);
    EVENT* getEvent();
    void putEvent(EVENT*);
    int getNextEventTime();
    int getInputCount(const std::string& inputFile);
    EVENT* removeEvent(DES::PROCESS*);

    std::list<EVENT*> eventQueue;
};

#endif // DES_H

