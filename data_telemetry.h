#ifndef DATA_TELEM_H
#define DATA_TELEM_H

#include <pthread.h>

struct data_telem_str {
    char serial_ack;
    char tcp_ack;
};

extern pthread_mutex_t tele_mtx;
extern struct data_telem_str data_telem;

#endif //DATA_TELEM_H
