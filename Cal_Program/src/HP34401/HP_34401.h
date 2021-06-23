

#ifndef _HP_34401A_
#define _HP_34401A_


#include <fcntl.h>  /* File Control Definitions          */
#include <dirent.h>
#include <sys/time.h>
#include <sys/ioctl.h>
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
#include "StringH.h"
#include "Struct_Defs.h"
#include "Data.h"


// this macro allows C++ compiler to use C files
#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG false
#define DEBUG_HEX false
#define DEBUG_FLOW false
#define DEBUG_METER false

// 34401A COMMANDS //

#define SET_REMOTE          "SYSTEM:REMOTE"
#define GET_ID              "*IDN?"
#define RESET               "*RST"
#define SET_AVERAGE         "CALC:FUNCTION AVERAGE"
#define FUNCTION_ON         "CALC:STATE ON"
#define FUNCTION_OFF        "CALC:STATE OFF"

#define GET_AVERAGE         "CALCULATE:AVERAGE:AVERAGE?"
#define GET_TRIGGER_SOURCE  "TRIGGER:SOURCE?"
#define GET_AVG_COUNT       "CALCULATE:AVERAGE:COUNT?"  // gets number of samples used for the average
#define GET_TRIGGER_COUNT   "TRIGGER:COUNT?"            // gets number of triggers to issue before returning to idle
#define GET_SAMPLE_COUNT    "SAMPLE:COUNT?"             // gets number of samples to take per trigger

#define SET_TRIGGER_IMM     "TRIGGER:SOURCE IMMEDIATE"
#define DISPLAY_ON          "DISPLAY ON"        // turn on the display
#define CLEAR_DISPLAY       "DISPLAY:TEXT:CLEAR"
#define INIT                "INIT"
#define FETCH               "FETCH?"
#define SET_VOLTS_DC        "CONFIGURE:VOLTAGE:DC"
#define SET_CURRENT_DC      "CONFIGURE:CURRENT:DC"
#define SET_AUTOZERO        "SENSE:ZERO:AUTO ON"



/*
DISPlay {OFF|ON}
DISPlay?
DISPlay
 :TEXT <quoted string>
 :TEXT?
 :TEXT:CLEar

 SYSTEM:BEEPER:STATE OFF
*/

// turn this into a function #define DISPLAY_TEXT(string)    DISPLAY:TEXT 'string'
// function SET_VOLTS_DC_INTEGRATION            VOLTAGE:DC:NPLCYCLES (x)
// order sequence:
// configure the measurement parameters
//set trigger SOURCE// INIT
// FETCH?

typedef enum meas_t
{
    VOLTS_DC,
    CURRENT_DC,
} DC_MEAS;

typedef struct meter_config_t
{
    int sample_count;
    int num_triggers;
    DC_MEAS meas_type;
}Meter_Configuration;



typedef struct meter_t
{
    int file_descriptor;
    char device_path[255];  // "/dev/ttyUSBXX"
    char meter_ID[25];      // "0,2-1-1" or "0,11-5-2"
    char meter_string[10];  // "HP34401A" or "HP34401B"
    char manufacturer[30];  // HEWLETT-PACKARD
    char model[20];          
    char text[15];
    int sample_count;
    int trigger_count;
    int NPLC;
    Meter_Configuration configuration;
}Meter;

// Function Declarations
int HP34401_open_serial_port(const char* device);
int HP34401_setup_serial_port(int port_FD);
int HP34401_Send_Command(Meter dmm, const char* command_string);
int HP34401_Get_Num_Chars_Waiting(int port_FD);
void HP34401_Read_Response(Meter* dmm, char* response_string);
void Print_Characters_Hex(char* string);
void Print_Response(char* response_string);
void HP34401_Print_Modem_Bits(int status_bits);
void HP34401_Enable_DTR(int port_FD);
void HP34401_Enable_DSR(int port_FD);
void HP34401_Disable_DTR(int port_FD);
void HP34401_Disable_DSR(int port_FD);
void HP34401_Enable_RTS(int port_FD);
void HP34401_Disable_RTS(int port_FD);
void Erase_Num_Chars(char* string, int length);
void Erase_String_Contents(char* string);
int parse_USB_Number(char* path);
float HP34401_Measure_Volts_DC(Meter dmm);
double Parse_Scientific_Notation( char* notation_string);
int Index_of_E( char* string );
char* Get_Substring(char* full_string, int start_index, int end_index);
void Trim_Leading_WhiteSpace(char* str);
void Trim_Trailing_WhiteSpace(char* str);
double Parse_Double(char* double_string);
void HP34401_Set_Remote(Meter* dmm);
float HP34401_Measure_Current_DC(Meter dmm);
void Setup_Trigger_10_samples(Meter* dmm);
void Set_Volts_DC_NPLC(Meter* dmm, int num_cycles);
void Set_Current_DC_NPLC(Meter* dmm, int num_cycles);
void Configure_Volts_DC(Meter* dmm);
void Configure_Current_DC(Meter* dmm);
void HP34401_Reset(Meter* dmm);
void HP34401_Verify_Buffer_Is_Empty(Meter* dmm);
void HP34401_Clear_Error(Meter dmm);
char* HP34401_Get_Meter_ID(Meter* dmm);
void HP34401_Identify_Meter(Meter* dmm);
Data_Group* HP34401_Average_10_DC_V_Samples(Meter* dmm);
Data_Group* HP34401_Average_10_DC_mA_Samples(Meter* dmm);
Data_Group* Parse_HP_Line(char* line_string);

void HP34401_Display_Text(Meter* dmm, const char* text);

void HP34401_Display_Text(Meter* dmm, const char* text)
{
    char text_command[80];
    StringH::Erase_Num_Chars(text_command, 80);

    if( strlen(text)<15)
    {
        sprintf(text_command, "DISPLAY:TEXT '%s'", text);
    }
    else
    {
        char shortened_string[15];
        StringH::Erase_Num_Chars(shortened_string, 15);
        strncpy(shortened_string, text, 15);
        shortened_string[15] = '\0';

        sprintf(text_command, "DISPLAY:TEXT '%s'", shortened_string);
    }

    HP34401_Send_Command(*dmm, text_command);
    usleep(ONE_MILLISECOND*100);
}


void HP34401_Reset(Meter* dmm)
{
    HP34401_Send_Command(*dmm, RESET);
    usleep(ONE_MILLISECOND*1000);
}

// Displays the ID string on the display of each meter
void HP34401_Identify_Meter(Meter* dmm)
{
  char* ID1 = HP34401_Get_Meter_ID(dmm);
  strcpy(dmm->meter_ID, ID1);
  free(ID1);

  HP34401_Display_Text(dmm, dmm->meter_ID);
}

void Set_Volts_DC_NPLC(Meter* dmm, int num_cycles)
{
    // Note, max NPLC is 100
    if( num_cycles > 100)
        num_cycles = 100;

    if( num_cycles < 1)
        num_cycles = 1;

    char command[50];

    sprintf(command, "VOLTAGE:DC:NPLCYCLES %d", num_cycles);
    HP34401_Send_Command(*dmm, command);

    dmm->NPLC = num_cycles;
}

void Set_Current_DC_NPLC(Meter* dmm, int num_cycles)
{
    // Note, max NPLC is 100
    if( num_cycles > 100)
        num_cycles = 100;

    if( num_cycles < 1)
        num_cycles = 1;

    char command[50];

    sprintf(command, "CURRENT:DC:NPLCYCLES %d", num_cycles);
    HP34401_Send_Command(*dmm, command);

    dmm->NPLC = num_cycles;
}

void Configure_Current_DC(Meter* dmm)
{
    printf("\tconfiguring " COLOR_MAGENTA "%s" COLOR_RESET " current measurements...\n", dmm->meter_string);

    dmm->configuration.meas_type = CURRENT_DC;

    HP34401_Send_Command(*dmm, SET_CURRENT_DC );
    usleep(ONE_MILLISECOND*50);

    Set_Current_DC_NPLC(dmm, 10);
    usleep(ONE_MILLISECOND*50);
}

void Configure_Volts_DC(Meter* dmm)
{
    printf("\tconfiguring " COLOR_MAGENTA "%s" COLOR_RESET " voltage measurements...\n", dmm->meter_string);

    dmm->configuration.meas_type = VOLTS_DC;

    HP34401_Send_Command(*dmm, SET_VOLTS_DC );
    usleep(ONE_MILLISECOND*50);

    Set_Volts_DC_NPLC(dmm, 10);
    usleep(ONE_MILLISECOND*50);
}

void Setup_Trigger_10_samples(Meter* dmm)
{
    HP34401_Send_Command(*dmm, SET_TRIGGER_IMM );    // sets trigger to immediate. Only have to send the "INIT" command
    usleep(ONE_MILLISECOND*50);

    int samples_per_trigger = 2;
    char command[50];
    sprintf(command, "SAMPLE:COUNT %d", samples_per_trigger);     // take 3 samples per trigger
    HP34401_Send_Command(*dmm, command );                        // send command
    usleep(ONE_MILLISECOND*50);
    dmm->sample_count = samples_per_trigger;
    dmm->configuration.sample_count = 2;

    int trigger_count = 5;
    sprintf(command, "TRIGGER:COUNT %d", trigger_count );     // set to 5 triggers total before returning to idle
    HP34401_Send_Command(*dmm, command );                     // send command
    usleep(ONE_MILLISECOND*50);
    dmm->trigger_count = trigger_count;
    dmm->configuration.num_triggers = 5;
}

Data_Group* HP34401_Average_10_DC_V_Samples(Meter* dmm)
{
    HP34401_Send_Command(*dmm, INIT );        // send command
    usleep(ONE_MILLISECOND*50);

    // for 10 samples, get reliable reads with a 2900 ms time delay
    usleep(ONE_MILLISECOND *3050);
    HP34401_Send_Command(*dmm, FETCH );  // send command

    char buffer[500];
    StringH::Erase_Num_Chars(buffer, 500);
    usleep(ONE_MILLISECOND*200);

    HP34401_Read_Response(dmm, buffer);
    //printf("%s\n", buffer);

    StringH::Trim_Leading_WhiteSpace(buffer);
    Data_Group* HP_V_measurements = Parse_HP_Line(buffer);

    strcpy(HP_V_measurements ->units, "V");
    HP_V_measurements ->average = Data::Calc_Average(HP_V_measurements );
    HP_V_measurements ->std_dev = Data::Calc_Std_Dev(HP_V_measurements );

    return HP_V_measurements;
}

Data_Group* HP34401_Average_10_DC_mA_Samples(Meter* dmm)
{
    HP34401_Send_Command(*dmm, INIT );        // send command
    usleep(ONE_MILLISECOND*10);

    // for 10 samples, get reliable reads with a 3000 ms time delay
    // this accounts for the time for meter to sample the data and then send
    usleep(ONE_MILLISECOND *3050);
    HP34401_Send_Command(*dmm, FETCH );  // send command

    char buffer[500];
    StringH::Erase_Num_Chars(buffer, 500);
    usleep(ONE_MILLISECOND*200);

    HP34401_Read_Response(dmm, buffer);

    StringH::Trim_Leading_WhiteSpace(buffer);
    Data_Group* measurements = Parse_HP_Line(buffer);
    Data::Unit_Convert(measurements->data_points, 1000, measurements->num_points);
    strcpy(measurements ->units, "mA");
    measurements->average = Data::Calc_Average(measurements);
    measurements->std_dev = Data::Calc_Std_Dev(measurements);

    return measurements;
}

Data_Group* Parse_HP_Line(char* line_string)
{
    double data_points[10];
    int data_index = 0;

    char* line = strdup(line_string);

    while( StringH::String_Contains(line, ",") )
    {
        //printf("line: %s \n", line);
        // remove first iterm from the list, store in buffer, up to 10 items
        int comma_index = StringH::Index_First_Comma(line);
        char* first_item_in_list  = StringH::Get_Substring(line, 0, comma_index-1);
        if(DEBUG) printf("\t%s  ....\t", first_item_in_list);
        data_points[ data_index ] = StringH::Parse_Scientific_Notation( first_item_in_list );
        if(DEBUG) printf("   %f \n", data_points[ data_index ]);
        ++data_index;
        free(first_item_in_list);

        char* updated_list = StringH::Get_Substring( line, comma_index+1, strlen(line)-1);   // this step updates the "line" string, creating new string w/out first token
        free(line);
        line = updated_list;
    }
    data_points[ data_index ] = StringH::Parse_Scientific_Notation( line );  // parse the last token when no commas remain
    free( line );

    Data_Group* datagroup = (Data_Group*)malloc(sizeof(Data_Group));
    datagroup->data_points = (float*)malloc( sizeof(float) * 12);

    int num_points = 0;
    for(int i = 0; i<= data_index; ++i)
    {
        datagroup->data_points[i] = data_points[i];
        ++num_points;
    }
    datagroup->num_points = num_points;

    return datagroup;
}

int HP34401_open_serial_port(const char* device)
{
    int port;
    port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    /*  O_RDWR POSIX read write
        O_NOCTTY: this program does not want to be the "controlling terminal"
        O_NDELAY": Ignore DCD signal state. If you dont set this, program will sleep until DCD is at space voltage
   */

  printf(" opened device " COLOR_MAGENTA "%s\n" COLOR_RESET, device);
  if(port == -1)
    { perror(" - Unable to open ttyUSB[x] - \n"); return false; }
  else
    fcntl(port, F_SETFL, 0);
        // fnctl - performs operation on file descriptor specified, "port"
        // F_SETFL  - set file descriptor flags. Can only change these flags: O_APPEND, O_ASYNC, O_DIRECT, O_NOATIME, and O_NONBLOCK
        // 0 - clear all flags

  HP34401_setup_serial_port(port);

  return port;
}

int HP34401_setup_serial_port(int port_FD)
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
  SerialPortSettings.c_cc[VTIME] = 2;       // amount of time to wait in tenths of seconds before returning
                                            // If VTIME is set to 0 (the default), reads will block (wait) indefinitely unless the NDELAY option
                                            // is set on the port with open or fcntl. If VTIME > 0, then read blocks until nBytes have been received,
                                            // or the timer expires. See: http://www.unixwiz.net/techtips/termios-vmin-vtime.html

  tcflush(port_FD, TCIOFLUSH); // clear out the input and output buffer

  int result = tcsetattr(port_FD, TCSANOW, &SerialPortSettings); // Set the port settings. TCSANOW == make changes now
  if (result < 0)
        printf("failed to set attr: %d, %s\n", port_FD, strerror(errno));

  HP34401_Enable_DTR(port_FD);     // Enable output flow control pins
  HP34401_Enable_DSR(port_FD);     // Enable output flow control pins
  HP34401_Enable_RTS(port_FD);     // Enable output flow control pins

  if(DEBUG_FLOW) HP34401_Print_Modem_Bits(port_FD);

  return result;
}

void HP34401_Set_Remote(Meter* dmm)
{
    HP34401_Send_Command(*dmm, SET_REMOTE);
    usleep(ONE_MILLISECOND*50);

    HP34401_Send_Command(*dmm, GET_ID);
    usleep(ONE_MILLISECOND*50); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    HP34401_Read_Response(dmm, response);

    bool correct = StringH::String_Contains(response, "HEWLETT-PACKARD,34401A,");

    return;
}

void HP34401_Print_Modem_Bits(int port_FD)
{
    int status_bits;
    ioctl(port_FD, TIOCMGET, &status_bits);   // gets the status of the modem bits: TIOCM_DTR, TIOCM_RTS, TIOCM_CTS, TIOCM_DSR, ...

    int DTR_flag, CTS_flag, RTS_flag, DSR_flag;

    DTR_flag = status_bits & TIOCM_DTR; // Mask out the specific bit
    DSR_flag = status_bits & TIOCM_DSR;
    CTS_flag = status_bits & TIOCM_CTS;
    RTS_flag = status_bits & TIOCM_RTS;

    if( DEBUG_FLOW )
    {
        printf("--Modem Status Bits--\n");

        if(DTR_flag)    printf("\tDTR bit: SET\n");
        else            printf("\tDTR bit: OFF\n");

        if(DSR_flag)    printf("\tDSR bit: SET\n");
        else            printf("\tDSR bit: OFF\n");

        if(CTS_flag)    printf("\tCTS bit: SET\n");
        else            printf("\tCTS bit: OFF\n");

        if(RTS_flag)    printf("\tRTS bit: SET\n\n");
        else            printf("\tRTS bit: OFF\n\n");
    }

    return;
}

// returns HP34401A ID string. Memory for the string must be freed afterwards
char* HP34401_Get_Meter_ID(Meter* dmm)
{
    HP34401_Send_Command(*dmm, GET_ID);
    usleep(ONE_MILLISECOND*75); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    HP34401_Read_Response(dmm, response);
    // Example response:    HEWLETT-PACKARD,34401A,0,2-1-1

    char delims[] = ",";    // delimiters to be used in parsing out tokens
    char* token = strtok(response, delims);

    if( token != NULL && strlen(token) > 0)
    {
        bool correct = StringH::String_Contains(token, "HEWLETT-PACKARD");
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "HEWLETT-PACKARD");

        token = strtok(NULL, delims);
        correct = StringH::String_Contains(token, "34401A");
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "34401A");

        char* ID = (char*)malloc( sizeof(char) * 15); // ID actually should only be 7 characters long
        StringH::Erase_Num_Chars(ID, 10);

        token = strtok(NULL, delims);
        strncpy(ID, token, 2);
        char comma[] = ",";
        strcat(ID, comma);

        token = strtok(NULL, delims);
        strcat(ID, token);

        int meter_number = parse_USB_Number(dmm->device_path);
        printf("\tHP34401 ID%d: " COLOR_MAGENTA "\t%s" COLOR_RESET, meter_number, ID);

        return ID;
    }
    else
        printf("Could not read response from HP meter.\n");
}

void HP34401_Clear_Error(Meter dmm)
{
    char err_str[100];
    StringH::Erase_Num_Chars(err_str, 100);
    sprintf(err_str, "\tresetting %s to clear error condition.\n", dmm.meter_string);
    fputs(err_str, stderr);
    HP34401_Reset(&dmm);
    usleep(1000);
    HP34401_Identify_Meter(&dmm);

    if( dmm.configuration.meas_type == VOLTS_DC)
    {
        Configure_Volts_DC(&dmm); 
        usleep(10000);
        Setup_Trigger_10_samples(&dmm);
        usleep(10000);
    }

    if( dmm.configuration.meas_type == CURRENT_DC)
    {
        Configure_Current_DC(&dmm); 
        usleep(10000);
        Setup_Trigger_10_samples(&dmm);
        usleep(10000);
    }
}

int HP34401_Send_Command(Meter dmm, const char* command_string)
{
    int port_FD = dmm.file_descriptor;
    int num_written = 0;

    char write_string[255];
    StringH::Erase_Num_Chars(write_string, 255);
    strcpy(write_string, command_string);
    strcat(write_string, "\r\n");       // add carriage return and newline

    num_written = write(port_FD, write_string, strlen(write_string) );
    if (num_written < 0)
    {
        fputs("write() to HP34401 failed!\t in ::HP34401A_Send_Command\n", stderr);
        HP34401_Clear_Error(dmm);
    }

    char dev_path[50];
    strcpy(dev_path, dmm.device_path);
    int meter_number = parse_USB_Number(dev_path);

    if( DEBUG_METER )
    {
        printf("CMD to " COLOR_BRIGHT_MAGENTA "Meter%d: \t" COLOR_RESET COLOR_BRIGHT_YELLOW "%s\n" COLOR_RESET, meter_number, command_string );
    }
    if( DEBUG_HEX )
    {
        Print_Characters_Hex(write_string);      // used to debug line termination
        printf("CMD to " COLOR_BRIGHT_MAGENTA "Meter%d: \t" COLOR_RESET COLOR_BRIGHT_YELLOW "%s\n" COLOR_RESET, meter_number, command_string );
    }

    usleep(ONE_MILLISECOND*550); //Wait after sending to let HP34401 process.

    return num_written;
}

int parse_USB_Number(char* path)
{
    char devpath[strlen(path)];
    strcpy(devpath, path);

    char delimiter[] = "/";    // delimiters to be used in parsing out tokens
    char* token = strtok(devpath, delimiter);


    while( token != NULL)
    {
        if( StringH::String_Contains(token, "ttyUSB") )
        {
            char* position = strrchr(token, 'B');   // gets pointer to character 'B' in "ttyUSB"
            position++; // increment pointer to character after ttyUSB, which should be a digit
            char ch_digit = *position;
            int digit = ch_digit - '0'; // convert ASCII value to digit by subtracting ASCII value for 0
            return digit;
        }
        token = strtok(NULL, delimiter);

    }
}

float HP34401_Measure_Volts_DC(Meter dmm)
{

    char buffer[512];
    StringH::Erase_Num_Chars(buffer, 512);

    HP34401_Send_Command(dmm, "MEAS:VOLT:DC?");
    usleep(ONE_MILLISECOND*100); //Wait after sending

    HP34401_Read_Response(&dmm, buffer);
    usleep(ONE_MILLISECOND*50); //Wait after sending

    float volts = StringH::Parse_Scientific_Notation(buffer);

    char dev_path[50];
    strcpy(dev_path, dmm.device_path);
    int meter_number = parse_USB_Number(dev_path);

    printf("Meter%d DCV: " COLOR_BOLD_RED "%.6f\n" COLOR_RESET, meter_number, volts);
    if(DEBUG) printf("\tdev path: %s\n\n", dmm.device_path);
    return volts;
}

float HP34401_Measure_Current_DC(Meter dmm)
{

    char buffer[255];
    StringH::Erase_Num_Chars(buffer, 255);

    HP34401_Send_Command(dmm, "MEAS:CURRENT:DC?");
    usleep(ONE_MILLISECOND*100); //Wait after sending

    HP34401_Read_Response(&dmm, buffer);
    usleep(ONE_MILLISECOND*50); //Wait after sending

    float volts = StringH::Parse_Scientific_Notation(buffer);

    char dev_path[50];
    strcpy(dev_path, dmm.device_path);
    int meter_number = parse_USB_Number(dev_path);

    printf("Meter%d DC Amps: " COLOR_BOLD_RED "%.6f\n" COLOR_RESET, meter_number, volts);
    return volts;
}

void Print_Characters_Hex(char* string)
{
    printf("\t\tASCII VALUES of STRING\n");
    printf(" dec. \t HEX \t char\n");
    printf("__________________________________\n");

    for(int i=0; i<strlen(string); ++i)
    {
        char cur_char;
        cur_char = string[i];
        int ascii_val = (int)cur_char;

        printf(" %d \t 0x%02X \t ", ascii_val, ascii_val);

        if(ascii_val >= 33)
            printf("%c\n", cur_char);
        else
        {
            switch(ascii_val)
            {
                case(8):
                    printf("backspace\n");
                    break;
                case(9):
                    printf("horizontal tab\n");
                    break;
                case(10):
                    printf("line feed\n");
                    break;
                case(12):
                    printf("form feed\n");
                    break;
                case(13):
                    printf("carriage return\n");
                    break;
                case(27):
                    printf("escape\n");
                    break;
                case(32):
                    printf("space\n");
                    break;
                case(0):
                    printf("NULL\n");
                    break;
            }
        }
    }
    printf("\n");
}

void HP34401_Enable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIS, &DTR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void HP34401_Disable_DTR(int port_FD)
{
    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    ioctl(port_FD,TIOCMBIC, &DTR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void HP34401_Enable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIS, &DSR_flag);   //Set DTR pin w/ "TIOCMBIS" command
}

void HP34401_Disable_DSR(int port_FD)
{
    int DSR_flag;
    DSR_flag = TIOCM_DSR;
    ioctl(port_FD,TIOCMBIC, &DSR_flag);   //Clear DTR pin w/ "TIOCMBIC" command
}

void HP34401_Enable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIS, &RTS_flag);  //Set RTS pin w/ "TIOCMBIS" command
}

void HP34401_Disable_RTS(int port_FD)
{
    int RTS_flag;
    RTS_flag = TIOCM_RTS;
    ioctl(port_FD,TIOCMBIC,&RTS_flag);   //Clear RTS pin w/ "TIOCMBIC" command
}

int HP34401_Get_Num_Chars_Waiting(int port_FD)
{
    int bytes_ready = 0;

    ioctl(port_FD, FIONREAD, &bytes_ready);
    if( DEBUG )
        printf("%d bytes in input buffer\n", bytes_ready);

    return bytes_ready;
}

void HP34401_Verify_Buffer_Is_Empty(Meter* dmm)
{
    char buff[255];
    usleep(1*ONE_MILLISECOND);
    int bytes_ready = HP34401_Get_Num_Chars_Waiting(dmm->file_descriptor);
    if( bytes_ready > 0 )
    {
        read(dmm->file_descriptor, buff, 255);
    }
}


void HP34401_Read_Response(Meter* dmm, char* response_string)
{
    char buff[512];
    for(int i = 0; i< 512; ++i)
        buff[i] = '\0';     // have to do this otherwise characters from other loop iterations are printed

    int bytes_ready = HP34401_Get_Num_Chars_Waiting(dmm->file_descriptor);
    if( !(bytes_ready > 0) )
    {
        usleep(ONE_MILLISECOND*500);
        if( !(HP34401_Get_Num_Chars_Waiting(dmm->file_descriptor) > 0) )
        {
            HP34401_Send_Command(*dmm, FETCH );  // send command
            usleep(ONE_MILLISECOND*50);
        }
        int num_timeouts = 0; 
        while( !(HP34401_Get_Num_Chars_Waiting(dmm->file_descriptor) > 0) )
        {
            printf("waiting another 500 ms for response from HP34401\n");
            usleep(ONE_MILLISECOND*500);
            ++num_timeouts;
            if( num_timeouts >= 2 && !(HP34401_Get_Num_Chars_Waiting(dmm->file_descriptor) > 0))
            {
                HP34401_Clear_Error(*dmm);
                response_string[0] = '\0';
                response_string[1] = '\0';
                return;
            }
        }
    }

    int num_read = read(dmm->file_descriptor, buff, 512);
    if( num_read > 0 )
    {
        if( DEBUG )
        {
            printf("\tresponse:\t" COLOR_BOLD_CYAN "%s\n" COLOR_RESET, buff);
        }
    }
    else
        printf(COLOR_BOLD_RED "\tno response from HP34401, even after extended wait\n\n" COLOR_RESET);

    StringH::Erase_String(response_string);
    strcpy(response_string, buff);

    return;
}

#ifdef __cplusplus
}
#endif

#endif
