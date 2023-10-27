#include "scheduler.h"
#include "DES.h"
#include "verbose.h"

extern bool doTrace;

// ========================|  FCFS Scheduler  |=========================
DES::PROCESS* FCFSScheduler::get_next_process() {
    if (!runQueue.empty()) {
        DES::PROCESS* nextProcess = runQueue.front();
        runQueue.pop();
        return nextProcess;
    }
    return nullptr;
}

void FCFSScheduler::add_process(DES::PROCESS* process) {
    runQueue.push(process);
}

int FCFSScheduler::get_next_event_time() {
    return 0;
}

void FCFSScheduler::printVec(){}

bool FCFSScheduler::testPreempt(DES::PROCESS* process, DES::PROCESS* currently_running_process, 
                                DES des, int curtime) {
    return false;
}

// =======================|  LCFS Scheduler  |========================
DES::PROCESS* LCFSScheduler::get_next_process() {
    if (!runQueue.empty()) {
        DES::PROCESS* nextProcess = runQueue.back();
        runQueue.pop_back();
        return nextProcess;
    }
    return nullptr;
}

void LCFSScheduler::add_process(DES::PROCESS* process) {
    runQueue.push_back(process);
}

int LCFSScheduler::get_next_event_time() {
    return 0;
}

void LCFSScheduler::printVec(){}

bool LCFSScheduler::testPreempt(DES::PROCESS* process, DES::PROCESS* currently_running_process,
                                DES des, int curtime) {
    return false;
}


// ========================|  SRTF Scheduler  |=====================
DES::PROCESS* SRTFScheduler::get_next_process() {
    if (!srtfQueue.empty()){
        DES::PROCESS* nextProcess = srtfQueue.front();
        srtfQueue.pop_front();
        return nextProcess;
    }
    return nullptr;
}


void SRTFScheduler::add_process(DES::PROCESS* process) {
    std::list<DES::PROCESS*>::iterator it = srtfQueue.begin(); 
    while (it != srtfQueue.end() && (*it)->total_compute <= process->total_compute) {
        ++it;
    }
    srtfQueue.insert(it, process);
}

int SRTFScheduler::get_next_event_time() {
    return 0;
}

void SRTFScheduler::printVec(){}

bool SRTFScheduler::testPreempt(DES::PROCESS* process, DES::PROCESS* currently_running_process,
                                DES des, int curtime) {
    return false;
}


// ========================|  RR Scheduler  |============================
DES::PROCESS* RRScheduler::get_next_process() {
    if (!runQueue.empty()) {
        DES::PROCESS* nextProcess = runQueue.front();
        runQueue.pop();
        return nextProcess;
    }
    return nullptr;
}


void RRScheduler::add_process(DES::PROCESS* process) {
    process->dynamic_priority = process->static_priority - 1;
    runQueue.push(process);
}

int RRScheduler::get_next_event_time() {
    return 0;
}

void RRScheduler::printVec() {

}

bool RRScheduler::testPreempt(DES::PROCESS* process, DES::PROCESS* currently_running_process,
                              DES des, int curtime) {
    return false;
}


// =======================|  PRIO Scheduler  |============================
DES::PROCESS* PRIOScheduler::get_next_process() {
    
    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it = queue_vector.rbegin(); 

    DES::PROCESS* next_process = nullptr;
    while (it != queue_vector.rend()) {

        if (!(*it).first.empty()) {
            next_process = (*it).first.front();
            (*it).first.pop_front();
            return next_process; 
        }
        it++;
    }

    swapVec();

    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it2 = queue_vector.rbegin(); 

    while (it2 != queue_vector.rend()) {

        if (!(*it2).first.empty()) {
            next_process = (*it2).first.front();
            (*it2).first.pop_front();
            return next_process; 
        }
        it2++;
    }


    return nullptr;
}


void PRIOScheduler::add_process(DES::PROCESS* process) {
    if (process->dynamic_priority == -1){
        process->dynamic_priority = process->static_priority - 1;
        queue_vector[process->dynamic_priority].second.push_back(process);
    }
    else {    
        queue_vector[process->dynamic_priority].first.push_back(process);
    }
}

int PRIOScheduler::get_next_event_time() {
    return 0;
}

void PRIOScheduler::printVec() {
    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it = queue_vector.rbegin();
    std::cout << "{ ";
    while (it != queue_vector.rend()) {
        std::cout << "[";
        for (auto innerIt = it->first.begin(); innerIt != it->first.end(); ++innerIt) {
            if (innerIt == it->first.begin()){
                std::cout << (*innerIt)->id;
            }
            else {
                std::cout << "," << (*innerIt)->id;
            }
        }
        std::cout << "]";
        it++;

    }

    std::cout << "} : { ";

    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it2 = queue_vector.rbegin();
    while (it2 != queue_vector.rend()) {
        std::cout << "[";
        for (auto innerIt2 = it2->second.begin(); innerIt2 != it2->second.end(); ++innerIt2) {
            if (innerIt2 == it2->second.begin()) {
                std::cout << (*innerIt2)->id;
            }
            else {
                std::cout << "," << (*innerIt2)->id;
            }
        }
        std::cout << "]";
        it2++;
    }
    std::cout << "} :" <<std::endl;

}

void PRIOScheduler::swapVec() {

    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it = queue_vector.rbegin();

    while (it != queue_vector.rend()) {
        std::swap(it->first, it->second);
        it++;
    }
    trace("switched queues\n");
}

bool PRIOScheduler::testPreempt(DES::PROCESS* process, DES::PROCESS* currently_running_process,
                                DES des, int curtime) {
    return false;
}


// =======================|  PREPRIO Scheduler  |============================
DES::PROCESS* PREPRIOScheduler::get_next_process() {
    
    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it = queue_vector.rbegin(); 

    DES::PROCESS* next_process = nullptr;
    while (it != queue_vector.rend()) {

        if (!(*it).first.empty()) {
            next_process = (*it).first.front();
            (*it).first.pop_front();
            return next_process; 
        }
        it++;
    }

    swapVec();

    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it2 = queue_vector.rbegin(); 

    while (it2 != queue_vector.rend()) {

        if (!(*it2).first.empty()) {
            next_process = (*it2).first.front();
            (*it2).first.pop_front();
            return next_process; 
        }
        it2++;
    }


    return nullptr;
}


void PREPRIOScheduler::add_process(DES::PROCESS* process) {

    if (process->dynamic_priority == -1){
        process->dynamic_priority = process->static_priority - 1;
        queue_vector[process->dynamic_priority].second.push_back(process);
    }
    else {    
        queue_vector[process->dynamic_priority].first.push_back(process);
    }
}

int PREPRIOScheduler::get_next_event_time() {
    return 0;
}

void PREPRIOScheduler::printVec() {
    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it = queue_vector.rbegin();

    std::cout << "{ ";
    while (it != queue_vector.rend()) {
        std::cout << "[";
        for (auto innerIt = it->first.begin(); innerIt != it->first.end(); ++innerIt) {
            if (innerIt == it->first.begin()) {
                std::cout << (*innerIt)->id;
            }
            else {
                std::cout << "," << (*innerIt)->id;
            }
        }
        std::cout << "]";
        it++;
    }

    std::cout << "} : { ";

    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it2 = queue_vector.rbegin();
    while (it2 != queue_vector.rend()) {
        std::cout << "[";
        for (auto innerIt = it2->second.begin(); innerIt != it2->second.end(); ++innerIt) {
            if (innerIt == it2->second.begin()) {
                std::cout << (*innerIt)->id;
            }
            else {
                std::cout << "," << (*innerIt)->id;
            }
        }
        std::cout << "]";
        it2++;
    }
    std::cout << "} :" <<std::endl;

}

void PREPRIOScheduler::swapVec() {

    std::vector<std::pair<std::deque<DES::PROCESS*>,
    std::deque<DES::PROCESS*>>>::reverse_iterator it = queue_vector.rbegin();

    while (it != queue_vector.rend()) {
        std::swap(it->first, it->second);
        it++;
    }
    trace("switched queues\n");
}

bool PREPRIOScheduler::testPreempt(DES::PROCESS* process, DES::PROCESS* currently_running_process,
                                   DES des, int curtime) {

    bool condition1 = false;
    bool condition2 = true;

    if (currently_running_process == nullptr) {
        return false;
    }
    else if (currently_running_process->dynamic_priority < process->dynamic_priority) {
        condition1 = true;
    }

    int timestamp_currently_running_process = 0;
    std::list<DES::EVENT*>::iterator it = des.eventQueue.begin();
    while (it != des.eventQueue.end()) {
        if ((*it)->process->id == currently_running_process->id){
            timestamp_currently_running_process = (*it)->timestamp;
        }
        it++;
    }
    if (timestamp_currently_running_process == curtime)
        condition2 = false;

    static const char *return_str[] = {"NO", "YES"};

    showDecision("  --> PrioPreempt Cond1=%d Cond2=%d (%d) --> %s\n",
                             condition1, condition2, timestamp_currently_running_process-curtime,
                return_str[condition1 && condition2]);

    return condition1 && condition2;
}

