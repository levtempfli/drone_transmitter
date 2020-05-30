#ifndef THREAD_TCP1_H
#define THREAD_TCP1_H

#include <cstddef>
#include "../globals.h"
#include "../tcp_socket/linux_tcpip.h"
#include "../timer/timer.h"


class thread_tcp1 {
public:
	static void *main(void*ptr);
private:
	static const int mainloop_wait;
	static const int connloop_wait;
	static const int conn_msg_timeout;
	static const int msg_send_period;
	static const int msg_types_max;
	static int msg_types_curr;
	static int in_status;
	static int in_chd_1;
	static int in_chd_2;
	static int slv_buff_i;
	static char in_ip[16];
	static int in_port;
	static bool connected;
	static const int in_buffer_size;
	static const int out_buffer_size;
	static const int slv_buffer_size;
	static char in_buffer[];
	static char out_buffer[];
	static char slv_buffer[];
	static timer conn_msg_timer;
	static timer msg_send_timer;
	static tcp_server t_ser;
	static tcp_handle t_han;
	static void main_loop();
	static void connected_loop();
	static void decode_message(int rec);
	static int encode_message();
	static void solve_message(int len, bool correct);
	static int create_message(int type);
	static int calculate_checksum(char *buff, int begin, int end);
};
#endif // !THREAD_TCP1_H
