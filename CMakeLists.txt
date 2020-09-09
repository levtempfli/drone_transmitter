cmake_minimum_required(VERSION 3.13)
project(drone_transmitter C)

set(CMAKE_C_STANDARD 11)

add_executable(drone_transmitter main.c data_communication.c thread_serial.c thread_vid_stream.c thread_tcp_tele.c timer.c lin_tcpip.c)

target_link_libraries(drone_transmitter python3.7m pthread)

add_custom_command(TARGET drone_transmitter POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/video_streamer/video_stream.py $<TARGET_FILE_DIR:drone_transmitter>)