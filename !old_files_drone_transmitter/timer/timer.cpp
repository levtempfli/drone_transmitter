#include "timer.h"

void timer::StartCounter() {
	clock_gettime(CLOCK_REALTIME, &start);
}

double timer::GetCounter() {
	clock_gettime(CLOCK_REALTIME, &stop);
	return ((stop.tv_sec - start.tv_sec)*1e3 + (stop.tv_nsec - start.tv_nsec) / 1e6);
}