#ifndef DATA_TELEM_H
#define DATA_TELEM_H

#include <pthread.h>

#define MAX_MSG_SIZE 128
#define TELEM_MSG_NUM 7
#define CONTR_MSG_NUM 3

extern struct data_comm_strc data_comm;

extern int init_data_comm_struct();

extern int destroy_data_comm_struct();

#endif //DATA_TELEM_H
