#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "data_telemetry.h"
#include "data_video.h"
#include "thread_capture.h"
#include "thread_serial.h"
#include "thread_tcp_tele.h"
#include "thread_tcp_vid.h"

int main() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int r1, r2, r3, r4;
    r1 = pthread_mutex_init(&vid_mtx, NULL);
    r2 = pthread_mutex_init(&tele_mtx, NULL);
    if (r1 != 0 || r2 != 0) {
        printf("pthread_mutex_init error: %s", strerror(r1));
        exit(1);
    }

    pthread_t thr_hdl_cap;
    pthread_t thr_hdl_ser;
    pthread_t thr_hdl_tcp_tel;
    pthread_t thr_hdl_tcp_vid;
    r1 = pthread_create(&thr_hdl_cap, NULL, &thr_capture_main, NULL);
    r2 = pthread_create(&thr_hdl_ser, NULL, &thr_serial_main, NULL);
    r3 = pthread_create(&thr_hdl_tcp_tel, NULL, &thr_tcp_tele_main, NULL);
    r4 = pthread_create(&thr_hdl_tcp_vid, NULL, &thr_tcp_vid_main, NULL);
    if(r1!=0||r2!=0||r3!=0||r4!=0){
        printf("pthread_create error: %s", strerror(r1));
        exit(1);
    }


    pthread_join(thr_hdl_cap, NULL);
    pthread_join(thr_hdl_ser, NULL);
    pthread_join(thr_hdl_tcp_tel, NULL);
    pthread_join(thr_hdl_tcp_vid, NULL);


    pthread_mutex_destroy(&vid_mtx);
    pthread_mutex_destroy(&tele_mtx);

    return 0;
}
