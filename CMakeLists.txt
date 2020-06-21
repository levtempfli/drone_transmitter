cmake_minimum_required(VERSION 3.13)
project(drone_transmitter C)

set(CMAKE_C_STANDARD 11)

add_executable(drone_transmitter main.c data_telemetry.c data_telemetry.h data_video.c data_video.h thread_capture.c thread_capture.h thread_serial.c thread_serial.h thread_tcp_vid.c thread_tcp_tele.c thread_tcp_vid.h thread_tcp_tele.h lin_tcpip.c lin_tcpip.h)

target_link_libraries(drone_transmitter pthread)