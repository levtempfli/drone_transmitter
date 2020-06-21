#ifndef LIN_TCPIP_H
#define LIN_TCPIP_H

//Creates TCP/IP socket
//Returns:0 - If no error occurs, otherwise returns the error code
extern int makesocket(int *sck);

//Sets socket mode(blocking/nonblocking)
//0-blocking, 1-nonblocking
//Returns:0 - If no error occurs, otherwise returns the error code
extern int setsocketmode(const int *sck, unsigned long mode);

//Gives error status of the last failed WinSock operation
//Returns:error code
extern int socketlasterror();

//Closes the socket
//Returns:0 - If no error occurs, otherwise returns the error code
extern int closesck(const int *sck);

//Disconnects
//Returns:0 - If no error occurs, otherwise returns the error code
extern int disconnect(int *sck);

//Connects to specified ip and port
//Returns:0 - If no error occurs, otherwise returns the error code
extern int connectsck(const int *sck, const char *destip, unsigned short destport);

//Sends data over socket
//Returns:0 - If no error occurs, otherwise returns the error code, 11 - nothing sent(only in non-blocking), 32- connection lost
extern int sendsck(const int *sck, char *buff, int bufflen, int *sent);

//Receives data over socket
//Returns:0 - If no error occurs, otherwise returns the error code, -1 - closed connection, 11 - nothing received(only in non-blocking)
extern int receivesck(const int *sck, char *buff, int bufflen, int *received);

//Associates a local address with a socket
//Returns:0 - If no error occurs, otherwise returns the error code
extern int bindsck(const int *sck, const char *ip, unsigned short port);

//Places the socket in a state in which it is listening for an incoming connection
//Returns:0 - If no error occurs, otherwise returns the error code
extern int listensck(const int *sck);

//Permits an incoming connection attempt on a socket.
//Returns:0 - If no error occurs, otherwise returns the error code, 11 - no incoming connection(only in non-blocking)
extern int acceptsck(const int *sck, int *newsocket, char *in_ip, int *in_port);

#endif //LIN_TCPIP_H
