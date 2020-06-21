#include <stdlib.h>
#include <stdio.h>


void *thr_serial_main(void *ptr) {
    printf("I am serial thread\n");
    return NULL;
}