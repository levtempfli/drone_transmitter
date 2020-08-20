#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lin_tcpip.h"
#include "timer.h"


#define in_buffer_size 64000
#define  out_buffer_size 256
#define  slv_buffer_size 4098

static const int mainloop_wait = 50;//miliseconds
static const int connloop_wait = 10;
static const int conn_msg_timeout = 5000;
static const int msg_send_period = 100;
static const int msg_types_max = 3;
static int msg_types_curr = 1;
static int in_status = 0;
static int in_chd_1, in_chd_2;
static int slv_buff_i = 0;
static char in_ip[16];
static int in_port;
static char connected = 0;
static struct timer conn_msg_timer;
static struct timer msg_send_timer;
static int t_ser, t_han;

static char in_buffer[in_buffer_size];
static char out_buffer[out_buffer_size];
static char slv_buffer[slv_buffer_size];


static void main_loop();

static void connected_loop();

static void decode_message(int rec);

static int encode_message();

static void solve_message(int len, char correct);

static int create_message(int type);

static char calculate_checksum(const char *buff, int begin, int end);

void *thr_tcp_tele_main() {
    int r;
    r = makesocket(&t_ser);
    if (r != 0) {
        printf("makesocket error: %s", strerror(r));
        exit(-1);
    }

    r = bindsck(&t_ser, "0.0.0.0", 325);
    if (r != 0) {
        printf("bindsck error: %s", strerror(r));
        exit(-1);
    }

    r = listensck(&t_ser);
    if (r != 0) {
        printf("listensck error: %s", strerror(r));
        exit(-1);
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        main_loop();
        usleep(mainloop_wait * 1000);
    }
#pragma clang diagnostic pop
}


static void main_loop() {
    setsocketmode(&t_ser, 0);
    int r = acceptsck(&t_ser, &t_han, in_ip, &in_port);
    if (r == 0) {
        connected = 1;//TODO:MSG set
        setsocketmode(&t_han, 1);
        TimerStartCounter(&conn_msg_timer);
        TimerStartCounter(&msg_send_timer);
    }
    while (connected) {
        connected_loop();
        usleep(connloop_wait * 1000);
    }
}

static void connected_loop() {
    int ret_v, rec = 0;
    ret_v = receivesck(&t_han, in_buffer, in_buffer_size, &rec);
    if (ret_v != 11 && ret_v != 0) {
        connected = 0;//TODO:MSG set
        disconnect(&t_han);
        return;
    }
    if (rec != 0) {
        decode_message(rec);
        TimerStartCounter(&conn_msg_timer);
    }

    int sent = 0, to_st = encode_message();
    if (to_st != 0) {
        ret_v = sendsck(&t_han, out_buffer, to_st, &sent);
        if (ret_v != 11 && ret_v != 0) {
            connected = 0;//TODO:MSG set
            disconnect(&t_han);
            return;
        } else if (to_st == sent && ret_v != 11) {
            TimerStartCounter(&msg_send_timer);
            msg_types_curr++;
            if (msg_types_curr > msg_types_max) msg_types_curr = 1;
        }
    }


    if (TimerGetCounter(&conn_msg_timer) > conn_msg_timeout) {
        disconnect(&t_han);
        connected = 0;//TODO:MSG set
    }
}

static void decode_message(int rec) {
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
                    default:
                        in_status = 0;
                }
                break;
        }
    }
}


static int encode_message() {
    if (TimerGetCounter(&msg_send_timer) > msg_send_period) {
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
    } else return 0;
}

static void solve_message(int len, char correct) {
    printf("%d", correct);
    for (int i = 1; i <= len; i++) {
        printf("%c", slv_buffer[i]);///////DEBUG
    }
}

static int create_message(int type) {
    switch (type) {
        case 1:
            strcpy(out_buffer, "Rhello1Z");
            return 7;
        case 2:
            strcpy(out_buffer, "Rhello2Z");
            return 7;
        case 3:
            strcpy(out_buffer, "Rhello3Z");
            return 7;
        default:
            return 0;
    }
}

static char calculate_checksum(const char *buff, int begin, int end) {
    char chs = 0;
    for (int i = begin; i <= end; i++) {
        chs = chs ^ buff[i];
    }
    return chs;
}