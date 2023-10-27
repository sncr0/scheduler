#include "DES.h"
#include "cstring"
#include "algorithm"
#include "sstream"
#include "iostream"
#include "cstdio"
#include "iostream"
#include "randomizer.h"

DES::DES(Randomizer& rand) : randomizer(rand) {}


extern bool doTrace;
#define extern trace(fmt...)      do { if (doTrace) { printf(fmt); fflush(stdout); } } while(0)
const char *state_str[] = {"CREATED", "READY", "RUNNG", "BLOCK", "PREEMPT", "Done"};
std::string DES::printQueue() {
    std::stringstream ss;
    std::list<DES::EVENT*> printQ = eventQueue;
    while (!printQ.empty()) {
        EVENT* e = printQ.front();
        printQ.pop_front();
        ss << e->timestamp << ":" 
            << e->process->id << ":" << state_str[e->newstate] << " ";
        //    std::cout << "Timestamp: " << e->timestamp << ", State: " << e->transition << std::endl;
    }
    return ss.str();
}

std::list<DES::PROCESS*> DES::readInput(const std::string& inputFile, int maxprio) {
    int inputCount = getInputCount(inputFile);

    int id = 0;
    int timestamp;
    int total_cpu;
    int cpu_burst;
    int io_burst;
    int static_priority;
    //PROCESS* process;
    EVENT* event;
    
    std::list<PROCESS*> process_list;

    std::ifstream file(inputFile);
    char line[256];
    while (file.getline(line, 256)) {
        timestamp = std::stoi(strtok(line, " "));
        total_cpu = std::stoi(strtok(nullptr, " "));
        cpu_burst = std::stoi(strtok(nullptr, " "));
        io_burst = std::stoi(strtok(nullptr, " "));
        static_priority = randomizer.myrandom(maxprio);
        PROCESS* process = new PROCESS(id, total_cpu, cpu_burst, io_burst, static_priority, timestamp);
        EVENT* event = new EVENT(timestamp, process, 0, 0, 1);
        //eventQueue.push(event);
        putEvent(event);
        process_list.push_back(process);
        id++;
    }

    // You can optionally print the queue after reading input
    //printQueue();
    return process_list;
}

int DES::getInputCount(const std::string& inputFile) {
    std::ifstream file(inputFile);
    std::string line;
    int inputCount = 0;
    while (std::getline(file, line)) {
        inputCount++;
    }
    return inputCount;
}

DES::EVENT* DES::getEvent(){
    EVENT* e = eventQueue.front();
    eventQueue.pop_front();
    return e;
}

DES::EVENT* DES::removeEvent(DES::PROCESS* process) {
    std::list<EVENT*>::iterator it = eventQueue.begin();
    for (it = eventQueue.begin(); it != eventQueue.end();) {
        EVENT* event = *it;
        if ((*it)->process == process) {
            //extract and return event, keep eventQueue with the event extracted out
            EVENT* extractedEvent = event;

            // Erase the event from the eventQueue
            it = eventQueue.erase(it);
            return extractedEvent;
        }
        it++;
    }
    return nullptr;
}

void DES::putEvent(DES::EVENT* event){
    if (eventQueue.empty() == true){
        eventQueue.push_back(event);
    }
    else {
        std::list<EVENT*>::iterator it = eventQueue.begin();
        while (it != eventQueue.end() && (*it)->timestamp <= event->timestamp) {
            ++it;
        }
        eventQueue.insert(it, event);
    }
}

int DES::getNextEventTime(){
    if (eventQueue.empty() == true){
        return -1;
    }
    else {
        int next_time = eventQueue.front()->timestamp;
        return next_time;
    }
}
