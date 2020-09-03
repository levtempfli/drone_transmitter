#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lin_tcpip.h"
#include <python3.7/Python.h>

void *thr_vid_stream_main(void *ptr) {
    printf("video thread started\n");

    Py_Initialize();

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        printf("Starting video stream\n");
        FILE *PythonScriptFile = fopen("video_stream.py", "r");
        if (PythonScriptFile) {
            PyRun_SimpleFile(PythonScriptFile, "video_stream.py");
            fclose(PythonScriptFile);
        }
        printf("Video stream crashed\n");

        sleep(1);
    }
#pragma clang diagnostic pop

    Py_Finalize();

    return NULL;
}