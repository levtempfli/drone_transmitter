import socket
import time
import picamera
import threading
import termios
import fcntl
import os


class PiCamStreamer:
    class _ClearPipe:
        clear_pipe_stop = False
        clear_pipe_stop_lock = threading.Lock()

        def clear_pipe(self, pipe_out_sck):
            try:
                k = 1
                while k != 0:
                    k = len(os.read(pipe_out_sck, 16384))
                    self.clear_pipe_stop_lock.acquire()
                    if self.clear_pipe_stop:
                        break
                    self.clear_pipe_stop_lock.release()
            except:
                pass

    _clr_p = _ClearPipe()

    _pipe_out_sck, _pipe_in_pcam = None, None
    _camera = None

    def _start_capture_to_pipe(self, bitrate_set):
        self._pipe_out_sck, self._pipe_in_pcam = os.pipe()
        self._camera.start_recording(os.fdopen(self._pipe_in_pcam, 'wb'), format='h264', bitrate=bitrate_set)

    def _stop_capture_to_pipe(self):
        self._clr_p.clear_pipe_stop = False
        clear_thread = threading.Thread(target=self._clr_p.clear_pipe, args=(self._pipe_out_sck,))
        clear_thread.start()

        self._camera.stop_recording()

        self._clr_p.clear_pipe_stop_lock.acquire()
        self._clr_p.clear_pipe_stop = True
        self._clr_p.clear_pipe_stop_lock.release()
        clear_thread.join()

        os.close(self._pipe_out_sck)

    def stream(self, port, resolution=(1280, 720), fps=30, min_bitrate=50000, max_bitrate=1000000, bitrate_step=50000,
               dbg=False):

        bitrate_set = max_bitrate

        server_socket = socket.socket()
        try:
            server_socket.bind(('0.0.0.0', port))
            server_socket.listen(0)
        except Exception as e:
            print('Failed starting the server. Exception thrown:', e)
            exit(1)

        self._camera = picamera.PiCamera()
        self._camera.resolution = resolution
        self._camera.framerate = fps

        connection = None
        try:
            bytes_waiting_avg = 0
            last_bitrate_change = time.time()
            connected = False
            c = 0

            while True:
                if not connected:
                    connection = server_socket.accept()[0]
                    connection.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                    self._start_capture_to_pipe(bitrate_set)
                    connected = True
                    if dbg: print('Client connected')
                while connected:
                    time.sleep(5 / 1000)

                    numb = bytearray(b'\0\0\0\0')
                    fcntl.ioctl(self._pipe_out_sck, termios.FIONREAD, numb)
                    bytes_waiting = int.from_bytes(numb, byteorder='little', signed=False)
                    bytes_waiting_avg = bytes_waiting_avg * 0.9 + bytes_waiting * 0.1

                    try:
                        connection.send(os.read(self._pipe_out_sck, 16384))
                    except ConnectionError:
                        if dbg: print('Client disconnected')
                        self._stop_capture_to_pipe()
                        connection.close()
                        connected = False
                        continue

                    if time.time() - last_bitrate_change > 5 and bytes_waiting_avg > 10000 and bitrate_set > min_bitrate:
                        if dbg: print('Decreasing bitrate to:', bitrate_set - bitrate_step)

                        self._stop_capture_to_pipe()
                        bitrate_set -= bitrate_step
                        self._start_capture_to_pipe(bitrate_set)

                        last_bitrate_change = time.time()
                        bytes_waiting_avg = 0

                        if dbg: print('Decreased bitrate to:', bitrate_set)

                    elif time.time() - last_bitrate_change > 20 and bytes_waiting_avg < 300 and bitrate_set < max_bitrate:
                        if dbg: print('Increasing bitrate to:', bitrate_set + bitrate_step)

                        self._stop_capture_to_pipe()
                        bitrate_set += bitrate_step
                        self._start_capture_to_pipe(bitrate_set)

                        last_bitrate_change = time.time()
                        bytes_waiting_avg = 0

                        if dbg: print('Increased bitrate to:', bitrate_set)

                    c += 1
                    if c == 20:
                        c = 0
                        if dbg: print('Pipe Buffer Average:', bytes_waiting_avg)

        except KeyboardInterrupt:
            print('Exit.')
        finally:
            connection.close()
            server_socket.close()


if __name__ == '__main__':
    streamer = PiCamStreamer()
    streamer.stream(port=326)
