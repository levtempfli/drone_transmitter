#ifndef GLOBALS_H
#define GLOBALS_H
#include <pthread.h> 
#include "data/data_commands.h"
#include "data/data_telemetry.h"
#include "data/data_video.h"
#include "error/error.h"

extern data_commands _dt_cmd;
extern data_telemetry _dt_tele;
extern data_video _dt_vid;



#endif // !GLOBALS_H
