

#include "USB_Helper.h"

#define DEBUG_TTYUSB false



#define USB_DEV_SEARCH_COMMAND   "ls /dev | grep ttyUSB"

// searches the /dev/ device tree location for devices with name ttyUSB*
// issues the shell command "ls /dev | grep ttyUSB", and returns a list of all results from the command
// for each result in the list, the port is opened, and the device is queried with *IDN? SCPI command
// this queries the device for its specific ID, from which we can identify the device.
// each device is queried, and a list of type USB_Device is returned
USB_Device** USB::Search_ttyUSB_Devices()
{
    char search_command[] = USB_DEV_SEARCH_COMMAND;
    char** response = USB::Exec_Shell_Command(search_command);
    if( response[0] == NULL )
    {
        printf("no response\n");
        return NULL;
    }

    usleep(ONE_MILLISECOND*1000);

    USB_Device** device_list = (USB_Device**)malloc( sizeof(USB_Device*)*5);
    for( int i=0; i<5; ++i)
        device_list[i] = NULL;

    char* path;
    int i = 0;
    while(response[i]!= NULL)
    {
        path = response[i];
        USB_Device* device = (USB_Device*)malloc( sizeof(USB_Device) );    // allocate memory for USB device struct
        StringH::Erase_Num_Chars(device->device_path, 255);                 // clear the memory for path string
        strcpy(device->device_path, "/dev/");
        strcat(device->device_path, path);                                 // copy the struct into the device path string
        //printf("%s\n", device->device_path);
        device->file_descriptor = USB::ttyUSB_open_serial_port(device->device_path);            // open the device path
        //printf("FD: %d\n", device->file_descriptor);
        USB::Get_USB_Device_ID(device);

        device_list[i] = device;
        ++i;
    }

    StringH::Free_String_Array_Memory(response);
    response = NULL;

    return device_list;
}

int Get_USB_Num_Chars_Waiting(int port_FD)
{
    int bytes_ready = 0;

    ioctl(port_FD, FIONREAD, &bytes_ready);
    if( DEBUG_TTYUSB )
        printf("%d bytes in input buffer\n", bytes_ready);

    return bytes_ready;
}

// Queries a USB attached device for its specific ID number
// as part of the response string from the device, three parameters are returned, and stored
// the device manufacturer, the device model number, and the specific ID string
void USB::Get_USB_Device_ID(USB_Device* device)
{
    USB::ttyUSB_Send_Command(*device, "*IDN?");
    usleep(ONE_MILLISECOND*100); //Wait after sending
    char* response;

    // example responses:
    response = USB::ttyUSB_Read_Response(device->file_descriptor);
    // Agilent Technologies,E3648A,0,1.7-5.0-1.0
    // HEWLETT-PACKARD,34401A,0,11-5-27 

    char delims[] = ",";    // dresponse string is delimited by comma
    char* token = strtok(response, delims); // parse the response string, delimiter is comma

    if( token != NULL && strlen(token) > 0)
    {
        strcpy(device->manufacturer, token);    // first token is the manufacturer
        token = strtok(NULL, delims);       // get next token
        strcpy(device->model, token);       // second token is the model
        token = strtok(NULL, delims);       // get next token, should be a single digit and a comma
        strncpy(device->ID_string, token, 2);   // add the read character to the ID string
        char comma[] = ",";
        strcat(device->ID_string, comma);   // add comma to ID string

        token = strtok(NULL, delims);   // get last token
        strcat(device->ID_string, token);   // copy parsed ID to device ID string

        USB::Print_Device(device);
    }
    else
        printf("Could not read response from HP meter.\n");
}

// prints parameters of a USB_Device object
void USB::Print_Device(USB_Device* device)
{
    printf(COLOR_BOLD_MAGENTA "%s\n" COLOR_RESET, device->device_path);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, device->manufacturer);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, device->model);
    printf(COLOR_LIGHT_BLUE "\t%s" COLOR_RESET, device->ID_string);

    if( StringH::String_Contains_Ignore_Case(device->ID_string, HP_A_ID) )
        printf(COLOR_LIGHT_BLUE "\t(HP METER A)\n" COLOR_RESET);
    else if( StringH::String_Contains_Ignore_Case(device->ID_string, HP_B_ID) )
        printf(COLOR_LIGHT_BLUE "\t(HP METER B)\n" COLOR_RESET);
    else if( StringH::String_Contains_Ignore_Case(device->ID_string, AGILENT_PS_ID) )
        printf(COLOR_LIGHT_BLUE "\t(AGILENT POWER SUPPLY)\n" COLOR_RESET);
}

// Frees memory dynamically allocated for USB_Device tyoe struct list
void USB::Free_Mem_Device_List(USB_Device** list)
{
    int i =0;
    while( list[i] != NULL )
    {
        if( list[i] != NULL)
            free( list[i] );
        ++i;
    }
    free(list);
}

/*
|--------------------------------------------------------------------------
| Exec_Shell_Command
|--------------------------------------------------------------------------
|
| Execute a command at the terminal prompt. The command is passed as argument.
| A string of the shell output response is returned. The string captures
| stdout and stderr.
|
| @param:  command    the shell command to execute
| @return:            a string of the shell response is returned
*/
char** USB::Exec_Shell_Command(const char* command)
{
    int MAX_LINES = 100;
    int MAX_LINE_LENGTH = 150;
    char read_line[250];



    FILE* command_shell = popen(command, POPEN_READ);                 //  The popen() function opens a process by creating a pipe, forking and invoking the shell.
    if (command_shell == NULL)
    {
        printf("Error opening pipe for shell command!!\n" );
        exit(1);
    }

    int line_count = 0;
    char**  string_array = (char**)malloc(sizeof(char*) * MAX_LINES);        // allocate memory for pointer to strings
    while (fgets(read_line, sizeof(read_line), command_shell) != NULL)     /* Read the output a line at a time - output it. */
    {
        StringH::Trim_Trailing_WhiteSpace(read_line);
        //printf("LINE:\t%s\n", read_line);
		string_array[line_count] = (char*)malloc(sizeof(char*) * ( strlen(read_line) +50 ) ); // allocate memory for the
        strcpy(string_array[line_count], read_line);
        ++line_count;
    }
    string_array[line_count] = NULL;   // Add a marker for the last string in the array
    int exit_code = pclose(command_shell);     // close file pointer to STDOUT
    return string_array;
}

// opens the serial port and returns a file descriptor
int USB::ttyUSB_open_serial_port(const char* device)
{
    int port;
    port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    /*  O_RDWR POSIX read write
        O_NOCTTY: this program does not want to be the "controlling terminal"
        O_NDELAY": Ignore DCD signal state. If you dont set this, program will sleep until DCD is at space voltage
   */

  if(port == -1)
    { perror(" - Unable to open ttyUSBX - \n"); return false; }
  else
    fcntl(port, F_SETFL, 0);
        // fnctl - performs operation on file descriptor specified, "port"
        // F_SETFL  - set file descriptor flags. Can only change these flags: O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, and O_NONBLOCK
        // 0 - clear all flags

  USB::ttyUSB_setup_serial_port(port);
  int num_waiting = USB::ttyUSB_Get_Num_Chars_Waiting(port);

  if(num_waiting > 0 )
  {
      char* temp = USB::ttyUSB_Read_Response(port);
      free(temp);
  }

  return port;
}

// sets up the serial port for HP/Agilent RS232 coms
// following parameters:
//  8 bits, 2 stop bits, no parity bit
//  9600 baud 
//  raw mode, software flow control off, but DTR, RTS, and DSR pins are enabled for RS232 flow control
int USB::ttyUSB_setup_serial_port(int port_FD)
{
  struct termios SerialPortSettings;                            // declare object to hold the port settings
  memset(&SerialPortSettings, 0, sizeof(SerialPortSettings));  // clear the new struct

  cfsetispeed(&SerialPortSettings, B9600); // set input speed
  cfsetospeed(&SerialPortSettings, B9600); // set output speed
  cfmakeraw(&SerialPortSettings);           // make port raw

  // Control Mode Flags
  SerialPortSettings.c_cflag |= (CLOCAL|CREAD);     // CLOCAL - do not change "owner" of the port.  CREAD - enable the receiver
  SerialPortSettings.c_cflag &= ~CSIZE;             // bit mask for character size. Resets the bit to 0 so we can set it
  SerialPortSettings.c_cflag |=  CS8;               // set the bit for 8 data bits
  SerialPortSettings.c_cflag |=  CSTOPB;            // enable this bit, sets for 2 stop bits to be used. If bit was cleared there would be 1 stop bit
  SerialPortSettings.c_cflag &= ~PARENB;            // clear the parity bit
  //SerialPortSettings.c_cflag |=  CRTSCTS;  //Turn on flow control

  // Local Mode Flags
  SerialPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //Make sure that Canonical input is off (raw data mode).
  // Clears all these flags:
        // ICANON - enable Canonical input (else raw)
        // ECHO - enable echoing of input Characters
        // ECHOE - echo erase characters (backspace, space)
        // ISG - Enable SIGINTR, SIGSUSP, SIGDSUSP, and SIGQUIT signals

  // Input Mode Flags
  SerialPortSettings.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                                | INLCR | IGNCR | ICRNL | IXON);    //Turn off software flow control

  // Output Mode Flags, filters output
  SerialPortSettings.c_oflag &= ~OPOST; // Clear post processing flag. (use raw output)
  // note, when OPOST is disabled, all other flags are ignored

  // Control Characters
  SerialPortSettings.c_cc[VMIN]  = 0;       // minimum number of characters to read
  SerialPortSettings.c_cc[VTIME] = 4;       // amount of time to wait in tenths of seconds before returning
                                            // If VTIME is set to 0 (the default), reads will block (wait) indefinitely unless the NDELAY option
                                            // is set on the port with open or fcntl. If VTIME > 0, then read blocks until nBytes have been received,
                                            // or the timer expires. See: http://www.unixwiz.net/techtips/termios-vmin-vtime.html

  tcflush(port_FD, TCIOFLUSH); // clear out the input and output buffer

  int result = tcsetattr(port_FD, TCSANOW, &SerialPortSettings); // Set the port settings. TCSANOW == make changes now
  if (result < 0)
        printf("failed to set attr: %d, %s\n", port_FD, strerror(errno));

  ttyUSB_Enable_DTR(port_FD);     // Enable output flow control pins
  ttyUSB_Enable_DSR(port_FD);     // Enable output flow control pins
  ttyUSB_Enable_RTS(port_FD);     // Enable output flow control pins

  return result;
}


// writes the command string to the serial port file descriptor
int USB::ttyUSB_Send_Command(USB_Device dev, const char* command_string)
{
    int port_FD = dev.file_descriptor;
    int num_written = 0;

    char write_string[255];
    StringH::Erase_Num_Chars(write_string, 255);
    strcpy(write_string, command_string);
    strcat(write_string, "\r\n");       // add carriage return and newline

    num_written = write(port_FD, write_string, strlen(write_string) );
    if (num_written < 0)
        fputs("USB write() failed!\n", stderr);

    usleep(ONE_MILLISECOND*550); //Wait after sending to let device process.

    return num_written;
}


void USB::ttyUSB_Enable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIS, &DTR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void USB::ttyUSB_Disable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIC, &DTR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void USB::ttyUSB_Enable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIS, &DSR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void USB::ttyUSB_Disable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIC, &DSR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void USB::ttyUSB_Enable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIS, &RTS_flag);  //Set RTS pin w/ "TIOCMBIS" command
}

void USB::ttyUSB_Disable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIC,&RTS_flag);   //Clear RTS pin w/ "TIOCMBIC" command
}

int USB::ttyUSB_Get_Num_Chars_Waiting(int port_FD)
{
    int bytes_ready = 0;

    ioctl(port_FD, FIONREAD, &bytes_ready);

    return bytes_ready;
}

// must free memory allocated for the returned string after finished with it
char* USB::ttyUSB_Read_Response(int file_descriptor)
{
    char buff[255];
    StringH::Erase_Num_Chars(buff, 255);

    int bytes_ready = USB::ttyUSB_Get_Num_Chars_Waiting(file_descriptor);
    if( !(bytes_ready > 0) )
    {
        usleep(ONE_MILLISECOND*500);
        while( !(ttyUSB_Get_Num_Chars_Waiting(file_descriptor) > 0) )
        {
            printf("waiting another 500 ms for response from ttyUSB device\n");
            usleep(ONE_MILLISECOND*500);
        }
    }

    int num_read = read(file_descriptor, buff, 255);
    if( num_read > 0 )
    {
        if( DEBUG_TTYUSB )
        {
            printf("\tresponse:\t" COLOR_BOLD_CYAN "%s\n" COLOR_RESET, buff);
        }
    }
    else
        printf(COLOR_BOLD_RED "\tno response from USB Device, even after extended wait\n\n" COLOR_RESET);

    return strdup(buff);
}