#include "Pi_Socket.h"

#define MAX_RECEIVE_STRING 5000

fd_set read_file_descriptor;
struct timeval timevalue;
char complete_receive_string[MAX_RECEIVE_STRING];

void Pi_Socket::PI_Receive( int sockFD, char* return_buffer )
{
    char recv_Buf[MAX_RECEIVE_STRING];
  	//struct timeval timevalue;
    timevalue.tv_sec = 0;
    timevalue.tv_usec = 10000; // 10ms
    FD_ZERO(&read_file_descriptor);
    FD_SET(sockFD, &read_file_descriptor);

    if ( !Pi_Socket::Check_FD_Ready(sockFD, &read_file_descriptor, &timevalue)	)
        return;

    StringH::Erase_String(complete_receive_string);
    StringH::Erase_String(return_buffer);
    // recv data if any is available on socket
    if (FD_ISSET(sockFD, &read_file_descriptor))
    {
        int count;
		ioctl( sockFD, FIONREAD, &count); // examine how many bytes are available to read from the socket, store in count
        if(count > MAX_RECEIVE_STRING)
            count = MAX_RECEIVE_STRING;

        Pi_Socket::Read_Socket_nBytes(sockFD, recv_Buf, count); // stores result in complete_receive_string

		int receive_Bytes = strlen(complete_receive_string);
		strncpy(return_buffer, (const char*)complete_receive_string, receive_Bytes);
        return_buffer[receive_Bytes] = '\0';
	}
    return;
}

void Pi_Socket::PI_Send(int sockFD, char* send_string)
{

    //printf("sending to pi: %s\n", send_string);
    timevalue.tv_sec = 0;
    timevalue.tv_usec = 10000; // 10ms
    FD_ZERO(&read_file_descriptor);
    FD_SET(sockFD, &read_file_descriptor);

    if ( !Pi_Socket::Check_FD_Ready(sockFD, &read_file_descriptor, &timevalue)	)
    {
        Network::report_error(ERR_FD_NOT_READY);
        return;
    }

    int send_Bytes = Network::sendData(sockFD , (char*)send_string, (int)strlen(send_string) );
    //printf("sent %d bytes\n", send_Bytes);
}


int Pi_Socket::Receive_Data(int sockFD, char* buffer)
{
		StringH::Erase_String((char*)buffer);
		int receive_Bytes = Network::recvData(sockFD , (char*)buffer, (int)sizeof(buffer));
		buffer[receive_Bytes] = '\0';
		return receive_Bytes;
}

void Pi_Socket::Read_Socket_nBytes(int sockFD, char* buff, int num_bytes)
{
		StringH::Erase_String(complete_receive_string);
		while(num_bytes>0)
		{
			int read_chars = Receive_Data(sockFD, buff); // stores result in complete_receive_string
			num_bytes -= read_chars;
			strncat(complete_receive_string, buff, read_chars);
			StringH::Erase_String(buff);
            strcpy(buff, complete_receive_string);
		}

}

bool Pi_Socket::Check_FD_Ready(int sockFD, fd_set* FD, struct timeval* tv)
{
		bool status = true;

		// argument order: (num_file descriptors+1, readFD, writeFD, fixed buffer, timeout value)
		if ( select(sockFD + 1, FD, NULL, NULL, tv) == -1 )	// determine if file descriptors of either socket are ready
		{
        printf("select failed dammit\n");
				status = false;
				perror("select failed\n");
				Network::disconnect_tcp(sockFD);
		}

		return status;
}

// Bytes available for reading on the socket
int Pi_Socket::Get_Bytes_Available(int fd)
{
    int count;
    ioctl( fd, FIONREAD, &count);
    return count;
}

int Pi_Socket::Connect_TCP(const char* serverIP, const char* serverPort)
{
    return Network::connect_tcp(serverIP, serverPort);
}

int Pi_Socket::Connect_to_PI( const char* serverIP, const char* serverPort )
{

    char recv_Buf[2000];
        printf("\nConnecting to Pi server " COLOR_BOLD_GREEN " %s " COLOR_RESET " on PORT " COLOR_BOLD_GREEN "%s\n\n" COLOR_RESET, serverIP, serverPort);

    int sockFD;
    sockFD = Network::connect_tcp(serverIP, serverPort);
    usleep(100);
    if (!sockFD )
            return -1;

    timevalue.tv_sec = 0;
    timevalue.tv_usec = 10000; // 10ms
    FD_ZERO(&read_file_descriptor);
    FD_SET(sockFD, &read_file_descriptor);

    if ( !Pi_Socket::Check_FD_Ready(sockFD, &read_file_descriptor, &timevalue)	)
            return -1;

    // recv data if any is sitting on socket
        if (FD_ISSET(sockFD, &read_file_descriptor))
        {
            int count;
            ioctl( sockFD, FIONREAD, &count); // examine how many bytes are available to read from the socket, store in count
            Pi_Socket::Read_Socket_nBytes(sockFD, recv_Buf, count);

            printf(COLOR_BROWN "%s\n" COLOR_RESET, complete_receive_string);
            int receive_Bytes = strlen(recv_Buf);
            strncpy(complete_receive_string, (const char*)recv_Buf, receive_Bytes);
        }

    // Now check to make sure the Blackbox server process is running:
    if( Pi_Socket::Host_Is_Up(sockFD) )
    {
        printf(COLOR_BOLD_GREEN "\tCONNECTED TO PI\n" COLOR_RESET);
        printf(COLOR_BOLD_GREEN "\tPI HOST IS UP\n\n" COLOR_RESET);
    }
    else
    {
        char REMOTE_HOST_IP[20];
        char REMOTE_HOST_MAC_ADDR[20];
        Net_Interface* local_interfaces = new Net_Interface();
        remote_pi** remote_pis = local_interfaces->Find_Connected_PIs();    // filters out pis found with MAC address of Calibration Box

        if(remote_pis != NULL)
        {
            remote_pi* connect_pi = remote_pis[0];  // select the first item in the list to connect to
            strncpy(REMOTE_HOST_IP, connect_pi->pi_ip_addr, 20);
            strncpy(REMOTE_HOST_MAC_ADDR, connect_pi->pi_mac_addr, 20);

        }
        Net_Interface::Free_Mem_Pi_List(remote_pis);
        delete local_interfaces;

        if(!(strlen(REMOTE_HOST_IP)) > 1)
        {   
            printf("Could not find Pi to connect to\n");
            printf("Exiting....\n");
            exit(-1);
        }

        // user and password are defined in "Cal_Parameters.h"
        SSH_Helper* ssh = new SSH_Helper(REMOTE_HOST_IP, REMOTE_HOST_MAC_ADDR, REMOTE_SSH_USER, REMOTE_SSH_PASSWORD);
        ssh->Log_In_To_Host();

        ssh->Restart_Remote_DAQ_Process();   // this func takes 30 seconds to return
        usleep(1000*1000);

        sockFD = Network::connect_tcp(serverIP, serverPort);
        usleep(100);
        timevalue.tv_sec = 0;
        timevalue.tv_usec = 10000; // 10ms
        FD_ZERO(&read_file_descriptor);
        FD_SET(sockFD, &read_file_descriptor);

        if ( !Pi_Socket::Check_FD_Ready(sockFD, &read_file_descriptor, &timevalue)	)
                return -1;

        // recv data if any is sitting on socket
        if (FD_ISSET(sockFD, &read_file_descriptor))
        {
            int count;
            ioctl( sockFD, FIONREAD, &count); // examine how many bytes are available to read from the socket, store in count
            Pi_Socket::Read_Socket_nBytes(sockFD, recv_Buf, count);

            printf(COLOR_BROWN "%s\n" COLOR_RESET, complete_receive_string);
            int receive_Bytes = strlen(recv_Buf);
            strncpy(complete_receive_string, (const char*)recv_Buf, receive_Bytes);
        }




        printf("checking again to see if host is up\n");
        bool is_running = ssh->Remote_DAQ_Process_Is_Running(false);   // check again, after restart. false == do not print status string from pi
        if( !is_running )
        {
            printf("\t--host is still down\n");
        }
        else
        {
            printf("\t--Remote DAQ Process is running now, retry the Calibration executable\n");
            exit(0);

        }


        // Now check to make sure the Blackbox server process is running:
        if( Pi_Socket::Host_Is_Up(sockFD) )
        {
            printf(COLOR_BOLD_GREEN "\tCONNECTED TO PI\n" COLOR_RESET);
            printf(COLOR_BOLD_GREEN "\tPI HOST IS UP\n\n" COLOR_RESET);
        }
        else
        {
            printf(COLOR_BOLD_RED "\tPI HOST IS DOWN.\n" COLOR_RESET);
            printf(COLOR_RED "\tCheck to make sure IP & socket number are correct, and server process is running on host.\n\n" COLOR_RESET);
            printf("Cannot continue, must exit. SSH into PI and verify application is running on the PI.\n");
            printf("Use 'top' or 'ps ax'.\n");
            exit(0);
        }
  }
  return sockFD;
}

bool Pi_Socket::Host_Is_Up(int sockFD)
{
    usleep(ONE_MILLISECOND*10);
    Pi_Socket::PI_Send( sockFD, (char*)"OK" );
    usleep(ONE_MILLISECOND*10);

    char receive_buff[SOCK_BUFF_SIZE];
    StringH::Erase_Num_Chars( receive_buff, SOCK_BUFF_SIZE );

    bool pi_received = false;
    char* received_string = NULL;
    time_t start, end;
    int seconds_waiting = 0;

    time(&start);
    while( !pi_received && seconds_waiting < 5 )
    {
        usleep(ONE_MILLISECOND);
        time(&end);
        seconds_waiting = (int)difftime(end, start);

        // if the PI server process is running, the correct response from the PI will be:
        // "Syntax Error: received "OK", missing syntax '>' or '?'"
        if( (receive_buff != NULL) && (StringH::String_Contains_Ignore_Case(receive_buff, "OK")) )
        {
            pi_received = true;
            return true;
        }
        else
            Pi_Socket::PI_Receive(sockFD, receive_buff);
    }

    if(pi_received == true)
        return true;
    else
        return false;
}
