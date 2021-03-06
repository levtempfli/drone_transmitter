#ifndef DATA_TELEM_H
#define DATA_TELEM_H

#include <pthread.h>

#define MAX_MSG_SIZE 128
#define TELEM_MSG_NUM 7
#define CONTR_MSG_NUM 2

struct data_comm_strc {
    struct msg_mtx_ready_strc {
        char msg[MAX_MSG_SIZE];
        int msg_len;
        pthread_mutex_t mtx;
        char ready;
    } telem_msgs[TELEM_MSG_NUM], control_msgs[CONTR_MSG_NUM];
    int telem_msgs_first_k_conti;//The first k message is continous, the others can be skipped
    int control_msgs_first_k_conti;
};

extern struct data_comm_strc data_comm;

extern int init_data_comm_struct();

extern int destroy_data_comm_struct();

#endif //DATA_TELEM_H
