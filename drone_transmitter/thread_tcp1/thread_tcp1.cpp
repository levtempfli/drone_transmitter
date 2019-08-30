#include "thread_tcp1.h"

tcp_server thread_tcp1::t_ser;
tcp_handle thread_tcp1::t_han;
timer thread_tcp1::conn_msg_timer;
timer thread_tcp1::msg_send_timer;
const int thread_tcp1::mainloop_wait = 50;//miliseconds
const int thread_tcp1::connloop_wait = 10;
const int thread_tcp1::conn_msg_timeout = 5000;
const int thread_tcp1::msg_send_period = 100;
const int thread_tcp1::msg_types_max = 3;
int thread_tcp1::msg_types_curr = 1;
int thread_tcp1::in_chd_1;
int thread_tcp1::in_chd_2;
int thread_tcp1::in_status = 0;
int thread_tcp1::slv_buff_i = 0;
char thread_tcp1::in_ip[16];
int thread_tcp1::in_port;
bool thread_tcp1::connected = 0;
const int thread_tcp1::in_buffer_size = 64000;
const int thread_tcp1::out_buffer_size = 256;
const int thread_tcp1::slv_buffer_size = 4098;
char thread_tcp1::in_buffer[thread_tcp1::in_buffer_size];
char thread_tcp1::out_buffer[thread_tcp1::out_buffer_size];
char thread_tcp1::slv_buffer[thread_tcp1::slv_buffer_size];

void *thread_tcp1::main(void*ptr) {
	int r;
	r = t_ser.makesocket();
	if (r != 0) {
		error::errorMSGwithcode("makesocket", r);
		exit(-1);
	}

	r = t_ser.bindsck("0.0.0.0", 325);
	if (r != 0) {
		error::errorMSGwithcode("bindsck", r);
		exit(-1);
	}

	r = t_ser.listensck();
	if (r != 0) {
		error::errorMSGwithcode("listensck", r);
		exit(-1);
	}
	
	while (1) {
		main_loop();
		usleep(50 * 1000);
	}

	return NULL;
}

void thread_tcp1::main_loop() {
	t_ser.setsocketmode(0);
	int r = t_ser.acceptsck(t_han.sck, in_ip, in_port);
	if (r == 0) {
		connected = 1;
		pthread_mutex_lock(&_dt_cmd.mutex);
		_dt_cmd.connected = 1;
		pthread_mutex_unlock(&_dt_cmd.mutex);
		t_han.setsocketmode(1);
		conn_msg_timer.StartCounter();
		msg_send_timer.StartCounter();
	}
	while (connected) {
		connected_loop();
		usleep(connloop_wait * 1000);
	}
}

void thread_tcp1::connected_loop() {
	int ret_v, rec;
	ret_v = t_han.receivesck(in_buffer, in_buffer_size, rec);
	if (ret_v != 11 && ret_v != 0) {
		connected = 0;
		pthread_mutex_lock(&_dt_cmd.mutex);
		_dt_cmd.connected = 0;
		pthread_mutex_unlock(&_dt_cmd.mutex);
		t_han.disconnect();
		return;
	}
	if (rec != 0) {
		decode_message(rec);
		conn_msg_timer.StartCounter();
	}

	int sent, to_st = encode_message();
	if (to_st != 0) {
		ret_v = t_han.sendsck(out_buffer, to_st, sent);
		if (ret_v != 11 && ret_v != 0) {
			connected = 0;
			pthread_mutex_lock(&_dt_cmd.mutex);
			_dt_cmd.connected = 0;
			pthread_mutex_unlock(&_dt_cmd.mutex);
			t_han.disconnect();
			return;
		}
		else if (to_st == sent && ret_v != 11) {
			msg_send_timer.StartCounter();
			msg_types_curr++;
			if (msg_types_curr > msg_types_max) msg_types_curr = 1;
		}
	}
	

	if (conn_msg_timer.GetCounter() > conn_msg_timeout) {
		t_han.disconnect();
		connected = 0;
		pthread_mutex_lock(&_dt_cmd.mutex);
		_dt_cmd.connected = 0;
		pthread_mutex_unlock(&_dt_cmd.mutex);
	}
}

void thread_tcp1::decode_message(int rec) {
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


int thread_tcp1::encode_message() {
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

void thread_tcp1::solve_message(int len, bool correct) {
	printf("%d", correct);
	for (int i = 1; i <= len; i++) {
		printf("%c", slv_buffer[i]);///////DEBUG
	}
}

int thread_tcp1::create_message(int type) {
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

int thread_tcp1::calculate_checksum(char * buff, int begin, int end) {
	int chs = 0;
	for (int i = begin; i <= end; i++) {
		chs = chs ^ buff[i];
	}
	return chs;
}
