#include <pthread.h> 
#include "thread_capture/thread_capture.h"
#include "thread_serial/thread_serial.h"
#include "thread_tcp1/thread_tcp1.h"
#include "thread_tcp2/thread_tcp2.h"
#include "data/data_commands.h"
#include "data/data_telemetry.h"
#include "data/data_video.h"
#include "globals.h"

data_commands _dt_cmd;
data_telemetry _dt_tele;
data_video _dt_vid;

thread_capture _th_cap;
thread_serial _th_ser;
thread_tcp1 _th_tcp1;
thread_tcp2 _th_tcp2;

int main(){
	error::init();
	int r1, r2, r3, r4;
	r1 = pthread_mutex_init(&_dt_cmd.mutex, NULL);
	r2 = pthread_mutex_init(&_dt_tele.mutex, NULL);
	r3 = pthread_mutex_init(&_dt_vid.mutex, NULL);
	if (r1 != 0 || r2 != 0 || r3 != 0) {
		error::errorMSGwithcode("pthread_mutex_init", r1);
		exit(0);
	}

	pthread_t _th_hdl_cap;
	pthread_t _th_hdl_ser;
	pthread_t _th_hdl_tcp1;
	pthread_t _th_hdl_tcp2;
	r1 = pthread_create(&_th_hdl_cap, NULL, &_th_cap.main, NULL);
	r2 = pthread_create(&_th_hdl_ser, NULL, &_th_ser.main, NULL);
	r3 = pthread_create(&_th_hdl_tcp1, NULL, &_th_tcp1.main, NULL);
	r4 = pthread_create(&_th_hdl_tcp2, NULL, &_th_tcp2.main, NULL);
	if (r1 != 0 || r2 != 0 || r3 != 0 || r4 != 0) {
		error::errorMSGwithcode("pthread_create", r1);
		exit(0);
	}

	pthread_join(_th_hdl_cap, NULL);
	pthread_join(_th_hdl_ser, NULL);
	pthread_join(_th_hdl_tcp1, NULL);
	pthread_join(_th_hdl_tcp2, NULL);
	
	
	pthread_mutex_destroy(&_dt_cmd.mutex);
	pthread_mutex_destroy(&_dt_tele.mutex);
	pthread_mutex_destroy(&_dt_vid.mutex);

    return 0;
}