#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "DES.h"
#include <queue>

class Scheduler {
public:
    Scheduler(const std::string& schedulerName,
              int mp,
              int q) : 
        name(schedulerName),
        max_priority(mp),
        quantum(q) {}
    virtual DES::PROCESS* get_next_process() = 0;
    virtual void add_process(DES::PROCESS* process) = 0;
    virtual int get_next_event_time() = 0;
    std::string name;
    int quantum;
    int max_priority;
    virtual void printVec() = 0;
    virtual bool testPreempt(DES::PROCESS*, DES::PROCESS*, DES, int) = 0;
};

class FCFSScheduler : public Scheduler {
public:
    FCFSScheduler(int mp, int q) : Scheduler("FCFS", mp, q) {} 
    std::queue<DES::PROCESS*> runQueue;
    DES::PROCESS* get_next_process() override;
    void add_process(DES::PROCESS* process) override;
    int get_next_event_time() override;
    void printVec() override;
    bool testPreempt(DES::PROCESS*, DES::PROCESS*, DES, int) override;
};

class LCFSScheduler : public Scheduler {
public:
    LCFSScheduler(int mp, int q) : Scheduler("LCFS", mp, q) {} 
    std::deque<DES::PROCESS*> runQueue;
    DES::PROCESS* get_next_process() override;
    void add_process(DES::PROCESS* process) override;
    int get_next_event_time() override;
    void printVec() override;    
    bool testPreempt(DES::PROCESS*, DES::PROCESS*, DES, int) override;
};

class SRTFScheduler : public Scheduler {
    public:
    SRTFScheduler(int mp, int q) : Scheduler("SRTF", mp, q) {}
    std::list<DES::PROCESS*> srtfQueue;
    DES::PROCESS* get_next_process() override;
    void add_process(DES::PROCESS* process) override;
    int get_next_event_time() override;
    void printVec() override;
    bool testPreempt(DES::PROCESS*, DES::PROCESS*, DES, int) override;
};

class RRScheduler : public Scheduler {
    public:
    RRScheduler(int mp, int q) : Scheduler("RR", mp, q) {}
    std::queue<DES::PROCESS*> runQueue;
    DES::PROCESS* get_next_process() override;
    void add_process(DES::PROCESS* process) override;
    int get_next_event_time() override;
    void printVec() override;
    bool testPreempt(DES::PROCESS*, DES::PROCESS*, DES, int) override;
};

class PRIOScheduler : public Scheduler {
public:
    PRIOScheduler(int mp, int q) : 
        Scheduler("PRIO", mp, q),
        queue_vector(mp, std::make_pair(std::deque<DES::PROCESS*>(), std::deque<DES::PROCESS*>())) {}
    DES::PROCESS* get_next_process() override;
    void add_process(DES::PROCESS* process) override;
    int get_next_event_time() override;
    void printVec() override;
    std::vector<std::pair<std::deque<DES::PROCESS*>, std::deque<DES::PROCESS*>>> queue_vector;
    void swapVec();
    bool testPreempt(DES::PROCESS*, DES::PROCESS*, DES, int) override;
};


class PREPRIOScheduler : public Scheduler {
public:
    PREPRIOScheduler(int mp, int q) : 
        Scheduler("PREPRIO", mp, q),
        queue_vector(mp, std::make_pair(std::deque<DES::PROCESS*>(), std::deque<DES::PROCESS*>())) {}
    DES::PROCESS* get_next_process() override;
    void add_process(DES::PROCESS* process) override;
    int get_next_event_time() override;
    void printVec() override;
    std::vector<std::pair<std::deque<DES::PROCESS*>, std::deque<DES::PROCESS*>>> queue_vector;
    void swapVec();
    bool testPreempt(DES::PROCESS*, DES::PROCESS*, DES, int) override;
};

#endif // SCHEDULER_H

