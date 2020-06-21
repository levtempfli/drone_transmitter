#include <pthread.h>
#include "data_telemetry.h"

pthread_mutex_t tele_mtx;
struct data_telem_str data_telem = {.tcp_ack=0, .serial_ack=0};
