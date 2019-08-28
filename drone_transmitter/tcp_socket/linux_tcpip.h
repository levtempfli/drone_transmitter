#ifndef TCPIP_H
#define TCPIP_H
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <ifaddrs.h>
#include <list>
#include <string>
#include <cstring>
#include <vector>

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1



//TCP socket class
class tcp_socket {
public:
	int sck; //Socket

	//Creates TCP/IP socket
	//Returns:0 - If no error occurs, otherwise returns the error code
	int makesocket();

	//Sets socket mode(blocking/nonblocking)
	//0-blocking, 1-nonblocking
	//Returns:0 - If no error occurs, otherwise returns the error code
	int setsocketmode(u_long mode);

	//Gives error status of the last failed WinSock operation
	//Returns:error code
	int socketlasterror();

	//Disconnects
	//Returns:0 - If no error occurs, otherwise returns the error code
	int disconnect();

	//Closes the socket
	//Returns:0 - If no error occurs, otherwise returns the error code
	int closesck();

};

//TCP client without connect
class tcp_handle :public tcp_socket {
public:
	//Sends data over socket
	//Returns:0 - If no error occurs, otherwise returns the error code, 11 - nothing sent(only in non-blocking), 32- connection lost
	int sendsck(char *buff, int bufflen, int &sent);

	//Receives data over socket
//Returns:0 - If no error occurs, otherwise returns the error code, -1 - closed connection, 11 - nothing received(only in non-blocking)
	int receivesck(char *buff, int bufflen, int &received);
};

//TCP client
class tcp_client :public tcp_handle {
public:
	//Connects to specified ip and port
	//Returns:0 - If no error occurs, otherwise returns the error code
	int connectsck(const char *destip, const unsigned short destport);
};

//TCP server
class tcp_server :public tcp_socket {
public:
	//Associates a local address with a socket
	//Returns:0 - If no error occurs, otherwise returns the error code
	int bindsck(const char *ip, const unsigned short port);

	//Places the socket in a state in which it is listening for an incoming connection
	//Returns:0 - If no error occurs, otherwise returns the error code
	int listensck();

	//Permits an incoming connection attempt on a socket.
//Returns:0 - If no error occurs, otherwise returns the error code, 11 - no incoming connection(only in non-blocking)
	int acceptsck(int &newsocket, char *in_ip, int &in_port);
};

//TCPIP utility
class tcp_utility {
public:
	//Lists the available network ip addresses
	//Returns:0 - If no error occurs, otherwise returns the error code
	static int getiptable(std::vector<std::string> &ips);

	//Get ip adress from hostname
	//Returns:0 - If no error occurs, otherwise returns the error code
	static int getipaddr(const char *addr, std::vector<std::string> &ips);
};


#endif // !TCPIP_H
