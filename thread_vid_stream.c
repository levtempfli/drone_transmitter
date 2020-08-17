#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lin_tcpip.h"
#include <python3.7/Python.h>

void *thr_vid_stream_main(void *ptr) {

    Py_Initialize();

    while (1) {
        printf("Starting video stream\n");
        FILE *PythonScriptFile = fopen("video_stream.py", "r");
        if (PythonScriptFile) {
            PyRun_SimpleFile(PythonScriptFile, "video_stream.py");
            fclose(PythonScriptFile);
        }
        printf("Video stream crashed\n");//TODO:DEBUG_MSG
        sleep(1);
    }

    Py_Finalize();

    return NULL;
}