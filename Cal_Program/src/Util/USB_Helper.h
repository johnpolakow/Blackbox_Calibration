#ifndef _USB_H_
#define _USB_H_

#include <fcntl.h>  /* File Control Definitions          */
#include <dirent.h>
#include <sys/time.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>      //Error number def
#include <termios.h>    //POSIX terminal control
#include <termio.h>
#include <sys/ioctl.h>

#include "colors.h"
#include "TimeH.h"
#include "StringH.h"
#include "Struct_Defs.h"
#include "Cal_Parameters.h"
//#include "Equipment_Defs.h"

#define HP34401A_ID  "0,11-5-2"
#define HP34401B_ID  "0,2-1-1"
#define E3648A_ID    "0,1.7-5.0-1.0"



typedef struct usb_device_t
{
    int file_descriptor;
    char device_path[255];
    char ID_string[20];
    char manufacturer[30];
    char model[20];
}USB_Device;


class USB
{
public:
    USB();
    ~USB();
    static int ttyUSB_open_serial_port(const char* device);
    static int ttyUSB_setup_serial_port(int port_FD);
    static int ttyUSB_Send_Command(USB_Device dev, const char* command_string);
    static int parse_USB_Number(char* path);

    static void ttyUSB_Enable_DTR(int port_FD);
    static void ttyUSB_Disable_DTR(int port_FD);
    static void ttyUSB_Enable_DSR(int port_FD);
    static void ttyUSB_Disable_DSR(int port_FD);
    static void ttyUSB_Enable_RTS(int port_FD);
    static void ttyUSB_Disable_RTS(int port_FD);
    static int ttyUSB_Get_Num_Chars_Waiting(int port_FD);
    static char* ttyUSB_Read_Response(int file_descriptor);
    static void Free_Mem_Device_List(USB_Device** list);

    static USB_Device** Search_ttyUSB_Devices();
    static char** Exec_Shell_Command(const char* command);

    static void Get_USB_Device_ID(USB_Device* device);
    static void Print_Device(USB_Device* device);
};




#endif
