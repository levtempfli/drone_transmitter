#include "thread_serial.h"

int thread_serial::serial_filestream = -1;
speed_t thread_serial::baud_rate = B115200;
bool thread_serial::serial_ack2 = 0;
timer thread_serial::conn_msg_timer;
timer thread_serial::msg_send_timer;
const int thread_serial::mainloop_wait = 10;//miliseconds
const int thread_serial::conn_msg_timeout = 5000;
const int thread_serial::msg_send_period = 100;
const int thread_serial::msg_types_max = 3;
int thread_serial::msg_types_curr = 1;
int thread_serial::in_chd_1;
int thread_serial::in_chd_2;
int thread_serial::in_status = 0;
int thread_serial::slv_buff_i = 0;
const int thread_serial::in_buffer_size = 64000;
const int thread_serial::out_buffer_size = 256;
const int thread_serial::slv_buffer_size = 4098;
char thread_serial::in_buffer[thread_serial::in_buffer_size];
char thread_serial::out_buffer[thread_serial::out_buffer_size];
char thread_serial::slv_buffer[thread_serial::slv_buffer_size];

void *thread_serial::main(void*ptr) {
	serial_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (serial_filestream == -1) {
		error::errorMSGwithcode("open", errno);
		exit(-1);
	}
	struct termios options;
	tcgetattr(serial_filestream, &options);
	options.c_cflag = baud_rate | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(serial_filestream, TCIFLUSH);
	int r = tcsetattr(serial_filestream, TCSANOW, &options);
	if (r == -1) {
		error::errorMSGwithcode("tcsetattr", errno);
		exit(-1);
	}

	conn_msg_timer.StartCounter();
	msg_send_timer.StartCounter();

	while (1) {
		main_loop();
		usleep(mainloop_wait * 1000);
	}

	return NULL;
}

void thread_serial::main_loop() {
	int ret_v, rec;

	ret_v = ioctl(serial_filestream, FIONREAD, &rec);
	if (ret_v == -1) {
		error::errorMSGwithcode("ioctl", errno);
		rec = -1;
	}
	if (rec > 0) {
		ret_v = read(serial_filestream, &in_buffer, in_buffer_size);
		if (ret_v == -1) {
			error::errorMSGwithcode("read", errno);
			rec = -1;
		}
		else rec = ret_v;

		if (rec > 0) {
			decode_message(rec);
			conn_msg_timer.StartCounter();
		}
	}


	int sent, to_st = encode_message();
	if (to_st != 0) {
		ret_v = write(serial_filestream, &out_buffer, to_st);
		if (ret_v == -1) {
			error::errorMSGwithcode("read", errno);
			sent = -1;
		}
		else sent = ret_v;

		if (to_st == sent) {
			msg_send_timer.StartCounter();
			msg_types_curr++;
			if (msg_types_curr > msg_types_max) msg_types_curr = 1;
		}
	}

	if (conn_msg_timer.GetCounter() > conn_msg_timeout) {
		serial_ack2 = 0;
		pthread_mutex_lock(&_dt_tele.mutex);
		_dt_tele.serial_ack = 0;
		pthread_mutex_unlock(&_dt_tele.mutex);
	}
	else if (serial_ack2 == 0){
		serial_ack2 = 1;
		pthread_mutex_lock(&_dt_tele.mutex);
		_dt_tele.serial_ack = 1;
		pthread_mutex_unlock(&_dt_tele.mutex);
	}
}

void thread_serial::decode_message(int rec) {
	char char_rec;
	for (int i = 0; i < rec; i++) {
		char_rec = in_buffer[i];
		switch (char_rec) {
		case '@':
			in_status = 1;
			slv_buff_i = 0;
			break;
		case '~':
			if (in_status != 1) break;
			in_status = 2;
			break;
		default:
			switch (in_status) {
			case 1:
				if (slv_buff_i == slv_buffer_size - 1) {
					in_status = 0;
					break;
				}
				slv_buff_i++;
				slv_buffer[slv_buff_i] = char_rec;
				break;
			case 2:
				if (char_rec < 58) in_chd_1 = char_rec - 48;
				else in_chd_1 = char_rec - 55;
				in_status = 3;
				break;
			case 3:
				if (char_rec < 58) in_chd_2 = char_rec - 48;
				else in_chd_2 = char_rec - 55;
				in_chd_1 = in_chd_1 * 16 + in_chd_2;
				if (in_chd_1 == calculate_checksum(slv_buffer, 1, slv_buff_i)) solve_message(slv_buff_i, 1);
				else solve_message(slv_buff_i, 0);
				in_status = 0;
				break;
			}
			break;
		}
	}
}


int thread_serial::encode_message() {
	if (msg_send_timer.GetCounter() > msg_send_period) {
		int len = create_message(msg_types_curr);
		out_buffer[0] = '@';
		int chs = calculate_checksum(out_buffer, 1, len);
		len++;
		out_buffer[len] = '~';
		len++;
		if (chs / 16 < 10) out_buffer[len] = 48 + (chs / 16);
		else out_buffer[len] = (chs / 16) - 10 + 65;
		len++;
		if (chs % 16 < 10) out_buffer[len] = 48 + (chs % 16);
		else out_buffer[len] = (chs % 16) - 10 + 65;
		return len + 1;
	}
	else return 0;

	return 6;
}

void thread_serial::solve_message(int len, bool correct) {
	printf("%d", correct);
	for (int i = 1; i <= len; i++) {
		printf("%c", slv_buffer[i]);///////DEBUG
	}
}

int thread_serial::create_message(int type) {
	switch (type) {
	case 1:
		strcpy(out_buffer, "Rhello1Z");
		return 7;
		break;
	case 2:
		strcpy(out_buffer, "Rhello2Z");
		return 7;
		break;
	case 3:
		strcpy(out_buffer, "Rhello3Z");
		return 7;
		break;
	default:
		return 0;
		break;
	}
}

int thread_serial::calculate_checksum(char * buff, int begin, int end) {
	int chs = 0;
	for (int i = begin; i <= end; i++) {
		chs = chs ^ buff[i];
	}
	return chs;
}