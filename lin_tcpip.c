#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

//Creates TCP/IP socket
//Returns:0 - If no error occurs, otherwise returns the error code
int makesocket(int *sck) {
    *sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*sck == INVALID_SOCKET) return errno;
    else return 0;
}

//Sets socket mode(blocking/nonblocking)
//0-blocking, 1-nonblocking
//Returns:0 - If no error occurs, otherwise returns the error code
int setsocketmode(const int *sck, unsigned long mode) {
    int r = ioctl(*sck, FIONBIO, &mode);
    if (r == SOCKET_ERROR) return errno;
    else return 0;
}

//Gives error status of the last failed WinSock operation
//Returns:error code
int socketlasterror() {
    return errno;
}

//Closes the socket
//Returns:0 - If no error occurs, otherwise returns the error code
int closesck(const int *sck) {
    int r = close(*sck);
    if (r == SOCKET_ERROR) return errno;
    else return 0;
}

//Disconnects
//Returns:0 - If no error occurs, otherwise returns the error code
int disconnect(int *sck) {
    int r = closesck(sck);
    if (r != 0) return r;
    r = makesocket(sck);
    if (r != 0) return r;
    return 0;
    /*int r = shutdown(sck, SHUT_RDWR);
    if (r == SOCKET_ERROR) return errno;*/
}


//Connects to specified ip and port
//Returns:0 - If no error occurs, otherwise returns the error code
int connectsck(const int *sck, const char *destip, const unsigned short destport) {
    struct sockaddr_in dest; //Remote endpoint specifier
    dest.sin_family = AF_INET;//IPv4
    dest.sin_port = htons(destport);//convert port to TCP/IP network byte order
    inet_pton(AF_INET, destip, &dest.sin_addr.s_addr);//convert to numeric binary form
    int r = connect(*sck, (struct sockaddr *) &dest, sizeof(dest));
    if (r == SOCKET_ERROR) return errno;
    else return 0;
}

//Sends data over socket
//Returns:0 - If no error occurs, otherwise returns the error code, 11 - nothing sent(only in non-blocking), 32- connection lost
int sendsck(const int *sck, char *buff, int bufflen, int *sent) {
    int r = send(*sck, buff, bufflen, MSG_NOSIGNAL);
    if (r == SOCKET_ERROR) return errno;
    else {
        *sent = r;
        return 0;
    }

}

//Receives data over socket
//Returns:0 - If no error occurs, otherwise returns the error code, -1 - closed connection, 11 - nothing received(only in non-blocking)
int receivesck(const int *sck, char *buff, int bufflen, int *received) {
    int r = recv(*sck, buff, bufflen, 0);
    if (r == SOCKET_ERROR) {
        *received = 0;
        return errno;
    } else if (r == 0) {
        return -1;
    } else {
        *received = r;
        return 0;
    }
}

//Associates a local address with a socket
//Returns:0 - If no error occurs, otherwise returns the error code
int bindsck(const int *sck, const char *ip, const unsigned short port) {
    struct sockaddr_in server; //Local endpoint specifier
    server.sin_family = AF_INET; //IPv4
    server.sin_port = htons(port);//convert port to TCP/IP network byte order
    inet_pton(AF_INET, ip, &server.sin_addr.s_addr);//convert to numeric binary form
    int r = bind(*sck, (struct sockaddr *) &server, sizeof(server));
    if (r == SOCKET_ERROR) return errno;
    else return 0;
}

//Places the socket in a state in which it is listening for an incoming connection
//Returns:0 - If no error occurs, otherwise returns the error code
int listensck(const int *sck) {
    int r = listen(*sck, SOMAXCONN);
    if (r == SOCKET_ERROR) return errno;
    else return 0;
}

//Permits an incoming connection attempt on a socket.
//Returns:0 - If no error occurs, otherwise returns the error code, 11 - no incoming connection(only in non-blocking)
int acceptsck(const int *sck, int *newsocket, char *in_ip, int *in_port) {
    struct sockaddr_in client; // Remote endpoint specifier
    int clen = sizeof(client);//Size of sockaddr_in
    *newsocket = accept(*sck, (struct sockaddr *) &client, (socklen_t *) &clen); // Accept connection
    if (*newsocket == INVALID_SOCKET) {
        *newsocket = 0;
        return errno;
    } else {
        inet_ntop(AF_INET, &(client.sin_addr), in_ip, INET_ADDRSTRLEN);//convert numeric binary form to ascii characters
        *in_port = ntohs(client.sin_port);//convert port to int
        return 0;
    }
}
