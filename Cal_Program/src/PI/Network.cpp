

#include "Network.h"


// Connect to IP:port, allocate any necessary resources
int Network::connect_tcp(const char* server_IP, const char* server_Port) {
	struct addrinfo hints;
	struct addrinfo *result = NULL;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(server_IP, server_Port, &hints, &result);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return -1;
	}

	int sockFD = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if(sockFD < 0)
	{
		Network::report_error(ERR_CLIENT_SOCKET_CREATE);
		return -1;
	}
	status = connect(sockFD, result->ai_addr, result->ai_addrlen);
	if(status < 0)
	{
		Network::report_error(ERR_ETH_CONN_FAILED);
		return -1;
	}
	free(result);		// free the dynamically allocated memory
	return sockFD;
}


void Network::report_error(int error_code)
{
  static const char* error_strings[] = {
    NULL,                                          							// ERR_OK          			0
    "Client socket creation error...",             							// ERR_CLIENT_SOCKET_CREATE	1
    "Ethernet Connection failed.(server process on PI may not be running)", // ERR_ETH_CONN_FAILED  	2
    "Server process on PI not running, or wrong IP/PORT params.",           // ERR_HOST_NOT_UP   		3
	"Server Disconnected.",													// ERR_DISCONNECT			4
	"Error in socket receive",												// ERR_SOCK_RECEIVE			5
	"Could not write to socket",											// ERR_SEND_BYTES			6
	"File Descriptor not ready",											// ERR_FD_NOT_READY			7
  };

  printf(COLOR_BRIGHT_RED "\tERROR:\t" COLOR_RESET "%s\n", error_strings[error_code]);
  fprintf(stderr, COLOR_BRIGHT_RED "\t\t<error> %s\n" COLOR_RESET, strerror( errno ));
}

// Disconnect socket, free any allocated resources
void Network::disconnect_tcp(int sockFD)
{
	close(sockFD);
}

// receive data on the socket
int Network::recv_tcp(int sockFD, char* buffer, unsigned int bufSize)
{
	int retVal = recv(sockFD, buffer, bufSize, 0);
	if(retVal <= 0) {
		if(retVal == 0)
			Network::report_error(ERR_DISCONNECT);
		else
			Network::report_error(ERR_SOCK_RECEIVE);
	}
	return retVal;
}

// send data on the socket
int Network::send_tcp(int sockFD, char* buffer, unsigned int bytesToSend) {
	int ret;
	int retVal = 0;

	if( bytesToSend == 0 )
			return 0;

	// all data in the buffer may not be sent on a single call to send(),
	// so loop until all data is sent
	while (retVal < (int)bytesToSend) {
		ret = send(sockFD, (char*)(buffer+retVal), bytesToSend-retVal, 0);
		if( ret <= 0 ) {
			Network::report_error(ERR_SEND_BYTES);
			retVal = ret;
			break;
		}
		retVal += ret;
	}
	return retVal;
}

// Convenience function: Receive data into a provided buffer on the given connection
int Network::recvData(int sockFD, char* buffer, unsigned int bufSize) {
	int retVal = Network::recv_tcp(sockFD, (char*)buffer, bufSize);
	if (retVal <= 0) {
		printf("\n\t\t\t...disconnecting\n");
		Network::disconnect_tcp(sockFD);
	}
	return retVal;
}

// Convenience function: Send data in a provided buffer on the given connection
int Network::sendData(int sockFD, char* buffer, unsigned int bytesToSend) {
	int retVal = Network::send_tcp(sockFD, (char*)buffer, bytesToSend);
	if (retVal <= 0) {
				printf("\n\t\t\t...disconnecting\n");
		Network::disconnect_tcp(sockFD);
	}
	return retVal;
}
