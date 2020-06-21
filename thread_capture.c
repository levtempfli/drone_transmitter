#include <stdlib.h>
#include <stdio.h>


void *thr_capture_main(void *ptr) {
    printf("I am capture thread\n");
    return NULL;
}