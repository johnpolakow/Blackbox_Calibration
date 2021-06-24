#ifndef _NETWORK_H
#define _NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>

#include "../Util/colors.h"

#define ERR_CLIENT_SOCKET_CREATE 	1
#define ERR_ETH_CONN_FAILED			2	// couldnt connect over the network
#define ERR_HOST_NOT_UP				3	// server process "exec" on the PI is not running, or IP address/ port mismatch
#define ERR_DISCONNECT				4	// server disconnected
#define ERR_SOCK_RECEIVE			5	// socket receive error
#define ERR_SEND_BYTES				6	// error sending bytes ( none written)
#define ERR_FD_NOT_READY			7


#define BACKLOG	5

// a convenience struct that contains the socket and remote address for a connection
typedef struct {
	int sockfd;
	struct sockaddr_in theiraddr;
} conn_t;


class Network
{
public:
		static int connect_tcp(const char* serverIP, const char* serverPort);
		static void disconnect_tcp(int sockFD);
		static int recv_tcp(int sockFD, char* buffer, unsigned int bufSize);
		static int send_tcp(int sockFD, char* buffer, unsigned int bytesToSend);
		static int recvData(int sockFD, char* buffer, unsigned int bufSize);
		static int sendData(int sockFD, char* buffer, unsigned int bytesToSend);
		static void report_error(int error_code);
};






#endif
