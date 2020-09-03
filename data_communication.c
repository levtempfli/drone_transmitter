#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "data_communication.h"

struct data_comm_strc data_comm;

int init_data_comm_struct() {
    int i, r;
    for (i = 0; i < TELEM_MSG_NUM; i++) {
        r = pthread_mutex_init(&data_comm.telem_msgs[i].mtx, NULL);
        if (r) {
            printf("pthread_mutex_init error: %s", strerror(r));
            return 1;
        }
        data_comm.telem_msgs[i].ready = 0;
        data_comm.telem_msgs[i].msg_len = 0;
        memset(data_comm.telem_msgs[i].msg, 0, MAX_MSG_SIZE);
    }
    for (i = 0; i < CONTR_MSG_NUM; i++) {
        r = pthread_mutex_init(&data_comm.control_msgs[i].mtx, NULL);
        if (r) {
            printf("pthread_mutex_init error: %s", strerror(r));
            return 1;
        }
        data_comm.control_msgs[i].ready = 0;
        memset(data_comm.control_msgs[i].msg, 0, MAX_MSG_SIZE);
    }
    return 0;
};

int destroy_data_comm_struct() {
    int i;
    for (i = 0; i < TELEM_MSG_NUM; i++) {
        pthread_mutex_destroy(&data_comm.telem_msgs[i].mtx);
    }
    for (i = 0; i < CONTR_MSG_NUM; i++) {
        pthread_mutex_destroy(&data_comm.control_msgs[i].mtx);
    }
    return 0;
}
