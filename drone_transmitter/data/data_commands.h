#ifndef DATA_COMMANDS_H
#define DATA_COMMANDS_H

struct data_commands {
	pthread_mutex_t mutex;
	bool connected = 0;
};

#endif // !DATA_COMMANDS_H
