#include <pthread.h> 
#include "thread_capture/thread_capture.h"
#include "thread_serial/thread_serial.h"
#include "thread_tcp1/thread_tcp1.h"
#include "thread_tcp2/thread_tcp2.h"
#include "data/data_commands.h"
#include "data/data_telemetry.h"
#include "data/data_video.h"

pthread_mutex_t _dt_cmd_mtx;
pthread_mutex_t _dt_tele_mtx;
pthread_mutex_t _dt_vid_mtx;

data_commands _dt_cmd;
data_telemetry _dt_tele;
data_video _dt_vid;


int main(){
    return 0;
}