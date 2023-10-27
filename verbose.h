#include <iostream>

#define verbose(fmt...)        do { if (doVerbose) {printf(fmt); fflush(stdout); } } while(0)
#define showEvents(fmt...)      do { if (doShowEvents) { printf(fmt); fflush(stdout); } } while(0)
#define trace(fmt...)        do { if (doTrace) {printf(fmt); fflush(stdout); } } while(0)
#define showDecision(fmt...)        do { if (doShowDecision) {printf(fmt); fflush(stdout); } } while(0)

extern bool doVerbose;
extern bool doShowEvents;
extern bool doTrace;
extern bool doShowDecision;

