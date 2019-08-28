#ifndef TIMER_H
#define TIMER_H
#include <time.h>

class timer {
public:
	void StartCounter();
	double GetCounter();
private:
	struct timespec start, stop;
};

#endif // !TIMER_H
