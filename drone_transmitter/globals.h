#ifndef GLOBALS_H
#define GLOBALS_H
#include <pthread.h> 
#include "data/data_commands.h"
#include "data/data_telemetry.h"
#include "data/data_video.h"

extern pthread_mutex_t _dt_cmd_mtx;
extern pthread_mutex_t _dt_tele_mtx;
extern pthread_mutex_t _dt_vid_mtx;

extern data_commands _dt_cmd;
extern data_telemetry _dt_tele;
extern data_video _dt_vid;



#endif // !GLOBALS_H
