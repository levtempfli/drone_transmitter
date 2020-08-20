#ifndef TIMER_H
#define TIMER_H

#include <time.h>

struct timer {
    struct timespec start, stop;
};

void TimerStartCounter(struct timer *tim);

double TimerGetCounter(struct timer *tim);


#endif // !TIMER_H
