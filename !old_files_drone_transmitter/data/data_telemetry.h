#ifndef DATA_TELEMETRY_H
#define DATA_TELEMETRY_H

struct data_telemetry {
	pthread_mutex_t mutex;
	bool serial_ack = 0;
};

#endif // !DATA_TELEMETRY_H

