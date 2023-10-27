#include <cmath>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <map>
#include <cstdio>
#include <sstream>
#include <utility>
#include <queue>
#include "DES.h"
#include "scheduler.h"
#include "randomizer.h"
#include "getopt.h"
#include "verbose.h"

// ===============================|  TRACING & VERBOSE OUTPUT  |================================ 

bool doVerbose = false;
bool doTrace = false;
bool doShowEvents = false;
bool doShowDecision = false;

// ==============================|   CALCULATOR STRUCT    |=====================================
struct Calculator {
    int cpu_time;
    int io_busy_time;
    int io_multiplicity;
    int finish_time;
    Calculator() : cpu_time(0), io_busy_time(0), io_multiplicity(0), finish_time(0) {}
};

// ==============================| MAIN DES CALL  |==============================================
void Simulation(DES des, Scheduler* scheduler, Randomizer randomizer, Calculator& calculator, int quantum) { 

    /*
     * Enumerations for optimization & clarity pursposes
     */
    typedef enum {TRANS_TO_READY, TRANS_TO_PREEMPT, TRANS_TO_RUN, TRANS_TO_BLOCK} transition_t; 
    typedef enum {CREATED, READY, RUNNG, BLOCK, PREEMPT, Done} state_t;
    static const char *state_str[] = {"CREATED", "READY", "RUNNG", "BLOCK", "PREEMPT", "Done"};

    /*
     *
     */


    /*
     * Pre-initialization for main event loop
     */
    DES::EVENT* evt;
    DES::PROCESS* currently_running_process = nullptr;
    int current_time;
    int previous_time;


    /*
     * Main event loop
     */
    while(!des.eventQueue.empty()) {
        // Obtain new event and depack it
        evt = des.getEvent();
        previous_time = current_time;
        current_time = evt->timestamp;       
        DES::PROCESS* process = evt->process;
        int time_in_prev_state = current_time - process->state_ts;
        int transition = evt->transition;
        int oldstate = evt->oldstate;
        int newstate = evt->newstate;
        process->state_ts = current_time;
        int nextstate;
        int io_burst;
        bool doPreempt = false;
        // Backwards accounting for the io-wait time from previous step
        if (calculator.io_multiplicity > 0) {
            calculator.io_busy_time = calculator.io_busy_time + (evt->timestamp - previous_time);
        }
        // Clear event for next call
        delete evt; evt = nullptr;

        // Switch statement processing the transition
        bool CALL_SCHEDULER;
        switch(transition) {
            //int burst;
            DES::EVENT* event;

            // Either from BLOCKED or CREATED -> add to sched runqueue, call sched,
            //                                   no event created (yet)
            case TRANS_TO_READY:
                // Sent to ready, which will then go into running at one point
                nextstate = RUNNG;
                // Backwards accounting for io time of process and updating io multiplicity
                if (oldstate == BLOCK) {
                    process->io_time = process->io_time + time_in_prev_state;
                    calculator.io_multiplicity = calculator.io_multiplicity - 1;
                }
                // Verbose output with state switch: BLOCK -> READY or CREATED -> READY
                verbose("%d %d %d: %s -> %s\n", current_time, process->id, time_in_prev_state, 
                      state_str[oldstate], state_str[newstate]);
                // Add process to the scheduler
                scheduler->add_process(process);
                doPreempt = scheduler->testPreempt(process, currently_running_process, des, current_time);
                // Call scheduler for what to run now
                CALL_SCHEDULER = true; 

                // Check if we need to preempt


                break;


            case TRANS_TO_PREEMPT: // Worldwide to be seen in all studios soon: this implementation
                
                process->total_compute = process->total_compute - time_in_prev_state;
                calculator.cpu_time = calculator.cpu_time + time_in_prev_state;
                process->current_burst -= time_in_prev_state;
                nextstate = RUNNG;
                verbose("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n", current_time, process->id, time_in_prev_state, 
                      state_str[oldstate], "READY", process->current_burst, process->total_compute,
                    process->dynamic_priority);


                process->dynamic_priority -= 1;

                scheduler->add_process(process);

                
                // must come from RUNNING (preemption)
                // add to runqueue (no event is generated)
                currently_running_process = nullptr;
                CALL_SCHEDULER = true;
                break;

            // Call from scheduler to run now
            case TRANS_TO_RUN: { 
                if (process->current_burst > 0) {
                    process->cpu_wait_time = process->cpu_wait_time + time_in_prev_state;
                }
                else {
                    process->current_burst = randomizer.myrandom(process->cpu_burst);
                    if (process->total_compute < process->current_burst) {
                        process->current_burst = process->total_compute;
                    }
                    process->cpu_wait_time = process->cpu_wait_time + time_in_prev_state;
                }
                //if process->

                if (process->current_burst <= quantum) {
                    nextstate = BLOCK;
                    verbose("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n",
                          current_time, process->id, time_in_prev_state, 
                          state_str[oldstate], state_str[newstate], process->current_burst, 
                          process->total_compute, process->dynamic_priority);
                    
                    int time_to_run = process->current_burst;
                    //process->current_burst = 0;

                    DES::EVENT* event = new DES::EVENT(current_time + time_to_run, 
                                                       process, TRANS_TO_BLOCK, 
                                                       newstate, nextstate);
                    showEvents(" AddEvent(%d:%d:%s): ", current_time + time_to_run, 
                          process->id, state_str[nextstate]);
                    showEvents("%s==> ", des.printQueue().c_str());
                    des.putEvent(event);
                    showEvents("%s\n", des.printQueue().c_str());
                }
                else {
                    nextstate = PREEMPT;
                    verbose("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n",
                          current_time, process->id, time_in_prev_state, 
                          state_str[oldstate], state_str[newstate], process->current_burst, 
                          process->total_compute, process->dynamic_priority);

                    //process->current_burst -= quantum;

                    DES::EVENT* event = new DES::EVENT(current_time + quantum, 
                                                       process, TRANS_TO_PREEMPT, 
                                                       newstate, nextstate);
                    showEvents(" AddEvent(%d:%d:%s): ", current_time + quantum, 
                          process->id, state_str[nextstate]);
                    showEvents("%s==> ", des.printQueue().c_str());
                    des.putEvent(event);
                    showEvents("%s\n", des.printQueue().c_str());
                }

                currently_running_process = process;
                // create event for either preemption or blocking
                break;
                }

            case TRANS_TO_BLOCK: {
                process->total_compute = process->total_compute - time_in_prev_state;
                calculator.cpu_time = calculator.cpu_time + time_in_prev_state;
                process->current_burst -= time_in_prev_state;
                process->dynamic_priority = process->static_priority - 1;
                if (process->total_compute == 0) {
                    nextstate = Done;
                    process->finish_time = current_time;
                    process->turnaround_time = current_time - process->arrival_time;
                    verbose("%d %d %d: %s\n", 
                          current_time, process->id, time_in_prev_state, 
                          state_str[nextstate]);
                }
                else {
                    calculator.io_multiplicity = calculator.io_multiplicity + 1;
                    io_burst = randomizer.myrandom(process->io_burst);

                    verbose("%d %d %d: %s -> %s ib=%d rem=%d\n", 
                          current_time, process->id, time_in_prev_state, 
                          state_str[oldstate], state_str[newstate],
                          io_burst, process->total_compute);

                    nextstate = READY;
                    DES::EVENT* event = new DES::EVENT(current_time + io_burst, 
                                                       process, TRANS_TO_READY,
                                                       newstate, nextstate);


                    showEvents(" AddEvent(%d:%d:%s): ", current_time + io_burst, 
                          process->id, state_str[nextstate]);
                    showEvents("%s==> ", des.printQueue().c_str());
                    des.putEvent(event);
                    showEvents("%s\n", des.printQueue().c_str());
                }
                currently_running_process = nullptr;
                CALL_SCHEDULER = true;
                break;
            }
        }

        if(CALL_SCHEDULER) {
            if (doPreempt == true && scheduler->name == "PREPRIO") {
                DES::EVENT* kicked_event = des.removeEvent(currently_running_process);
                showEvents("RemoveEvent(%d): %d:%d\n", currently_running_process->id, kicked_event->timestamp,
                           kicked_event->process->id);
                nextstate = PREEMPT;
                DES::EVENT* event = new DES::EVENT(current_time, currently_running_process,
                                               TRANS_TO_PREEMPT, RUNNG, nextstate);
                showEvents(" AddEvent(%d:%d:%s): ", current_time, currently_running_process->id, 
                      state_str[nextstate]);

                showEvents("%s==> ", des.printQueue().c_str());
                des.putEvent(event);
                showEvents("%s\n", des.printQueue().c_str());

                nextstate = RUNNG;
                DES::EVENT* prio_event = new DES::EVENT(current_time, process,
                                                   TRANS_TO_RUN, READY, nextstate);
            continue;
            }

            if (des.getNextEventTime() == current_time) {
                continue; //process next event from Event queue
            }
            CALL_SCHEDULER = false; // reset global flag
            if (currently_running_process == nullptr) {
                if (doTrace == true) {
                    scheduler->printVec();
                }
                currently_running_process = scheduler->get_next_process();
                if (currently_running_process == nullptr) 
                    continue;
                nextstate = RUNNG;
                DES::EVENT* event = new DES::EVENT(current_time, currently_running_process,
                                                   TRANS_TO_RUN, READY, nextstate);
                showEvents(" AddEvent(%d:%d:%s): ", current_time, currently_running_process->id, state_str[nextstate]);

                showEvents("%s==> ", des.printQueue().c_str());
                des.putEvent(event);
                showEvents("%s\n", des.printQueue().c_str());
            }
        } 
    }
    calculator.finish_time = current_time;
}

void printOutput(std::list<DES::PROCESS*> process_list, Calculator& calculator) {

    float process_count = static_cast<float>(process_list.size());
    float total_turnaround_time = 0;
    float total_cpu_wait_time = 0;
    
    //int total_time, io_time, cpu_wait_time;
    for (const auto& process : process_list) {
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
               process->id, process->arrival_time, 
               process->orig_total_compute,
               process->cpu_burst, process->io_burst,
               process->static_priority, process->finish_time,
               process->turnaround_time, 
               process->io_time, process->cpu_wait_time);
        total_turnaround_time += static_cast<float>(process->turnaround_time);
        total_cpu_wait_time += static_cast<float>(process->cpu_wait_time);
    }
    float average_turnaround_time = total_turnaround_time/process_count;
    float average_cpu_wait_time = total_cpu_wait_time/process_count;

    float througput = (process_count/calculator.finish_time)*100;
    float io_busy_time = static_cast<float>(calculator.io_busy_time);
    float cpu_time = static_cast<float>(calculator.cpu_time);
    float finish_time = static_cast<float>(calculator.finish_time);
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", calculator.finish_time, 100*cpu_time/finish_time,  
           100*io_busy_time/finish_time, average_turnaround_time, average_cpu_wait_time, througput);
}


int main(int argc, char **argv) {
    int c;
    char scheduler_type = 'F';
    int maxprio = 4;
    int quantum = 10000;
    while ((c = getopt(argc,argv,"vetps:i:r:")) != -1 ){
        switch(c) {
            case 'v': 
                doVerbose = true;
                break;
            case 'e': 
                doShowEvents = true;
                break;
            case 't':
                doTrace = true;
                break;
            case 'p':
                doShowDecision = true;
                break;
            case 's':

                if (optarg && *optarg) {                
                    sscanf(optarg, "%c%d:%d", &scheduler_type, &quantum, &maxprio);
                } 
                else {
                    std::cerr << "Missing argument for -s option." << std::endl;
                    return 1;
                }
                break;  
        }
    }

    Scheduler* scheduler = nullptr;
    if (scheduler_type == 'F') {
        scheduler = new FCFSScheduler(maxprio, quantum);
    } 
    else if (scheduler_type == 'S') {
        scheduler = new SRTFScheduler(maxprio, quantum);
    } 
    else if (scheduler_type == 'L') {
        scheduler = new LCFSScheduler(maxprio, quantum);
    }
    else if (scheduler_type == 'R') {
        scheduler = new RRScheduler(maxprio, quantum);
    }
    else if (scheduler_type == 'P') {
        scheduler = new PRIOScheduler(maxprio, quantum);
    }
    else if (scheduler_type == 'E') {
        scheduler = new PREPRIOScheduler(maxprio, quantum);
    }
    else {
        std::cerr << "Invalid argument for -s option: " << scheduler_type << std::endl;
        return 1;
    }

    std::string inputFile = "inputA";
    std::string rfile = "rfile";


    if ((argc - optind) == 2) {
        inputFile = argv[optind];
        rfile = argv[optind+1];
    }
    else if ((argc - optind) == 1) {
        inputFile = argv[optind];
    }


    Randomizer randomizer(rfile);
    DES des(randomizer);
    
    std::list<DES::PROCESS*> process_list;
    process_list = des.readInput(inputFile, maxprio);

    Calculator calculator;
    Simulation(des, scheduler, randomizer, calculator, quantum);
    
    std::cout << scheduler->name;
    if (scheduler->name == "RR" || scheduler->name == "PRIO" || scheduler->name == "PREPRIO") {
        std::cout << " " << quantum << std::endl;
    }
    else {
        std::cout << std::endl;
    }

    printOutput(process_list, calculator);

    return 0;
    exit(0);
}
