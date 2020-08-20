#include "timer.h"
#include <time.h>

//Starts counter
void TimerStartCounter(struct timer *tim) {
    clock_gettime(CLOCK_REALTIME, &tim->start);
}

//Returns elapsed time in miliseconds
double TimerGetCounter(struct timer *tim) {
    clock_gettime(CLOCK_REALTIME, &tim->stop);
    return ((tim->stop.tv_sec - tim->start.tv_sec) * 1e3 + (tim->stop.tv_nsec - tim->start.tv_nsec) / 1e6);
}