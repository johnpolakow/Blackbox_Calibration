
#ifndef PI_SOCK_H
#define PI_SOCK_H

#define SOCK_BUFF_SIZE 2000
#define ONE_MILLISECOND 1000


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>     //UNIX Standard function definitions
#include <fcntl.h>      //File control
#include <errno.h>      //Error number def
#include <termios.h>    //POSIX terminal control
#include <termio.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#include "StringH.h"
#include "Network.h"
#include "Net_Interface.h"
#include "SSH_Helper.h"


extern bool SEND_FLAG;
extern bool RECEIVE_FLAG;
extern char send_string[];

class Pi_Socket
{
public:
  Pi_Socket();
  static void PI_Receive(int sockFD, char* return_buffer);
  static int Receive_Data(int sockFD, char* buffer);
  static bool Check_FD_Ready(int sockFD, fd_set* FD, struct timeval* tv);
  static void Read_Socket_nBytes(int sockFD, char* buff, int num_bytes);
  void Get_String(char* usr_string);
  static void PI_Send(int sockFD, char* send_string);
  static int Connect_to_PI( const char* serverIP, const char* serverPort);
  static int Connect_TCP(const char* serverIP, const char* serverPort);
  static int Get_Bytes_Available(int fd);
  static bool Host_Is_Up(int sockFD);

  char receive_buffer[SOCK_BUFF_SIZE];
};


#endif
