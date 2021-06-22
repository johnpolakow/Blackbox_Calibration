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
#include <arpa/inet.h>
#include <net/if.h>


#include "relay.h"

static const int NEW_LINE  = 10;	       // ASCII integer code for new line
static const int CARRIAGE_RETURN = 13;	// ASCII integer code for carriage return
static const int QUIT      = 113;       // ASCII for 'q'
static const int ENTER_KEY = 10;        // ASCII character code


const static int CAL_COOLER_DCV = 1;
const static int CAL_COOLER_AC_MA = 2;
const static int CAL_REF100 = 3;
const static int CAL_LOAD = 4;
const static int CAL_DIODE = 5;
const static int CAL_EXIT = 6;
const static int SEL_ERROR = 7;
const static int CAL_AUTOMATIC = 11;
const static int CAL_MANUAL = 12;


#define MESSAGE_LENGTH 100

void Wait_Enter()
{
	int key_pressed;
	bool Enter_Pressed = false;
    printf("\n\t\t< to continue hit Enter >\n");
	while(!Enter_Pressed)
	{
			key_pressed = getchar();
			if((key_pressed == NEW_LINE) || (key_pressed == CARRIAGE_RETURN))
			{ Enter_Pressed = true; }
			if(key_pressed == QUIT)
			{ Enter_Pressed = true; }
	}
	return;
}

void Trim_WhiteSpace(char* trim_string)
{
    int start_index=0, new_index=0, old_index=0;

    // find index of first non-whitespace character
    while(trim_string[start_index] == ' ' || trim_string[start_index] == '\t' || trim_string[start_index] == '\n')
    { ++start_index;  }

    if(start_index != 0)
    {
        // copy all characters over to the left
        old_index = start_index;
        while( trim_string[ old_index ] != '\0' )
        {
            trim_string[ new_index ] = trim_string[ old_index ];
            ++new_index;
            ++old_index;
        }
        trim_string[new_index] = '\0'; // Make sure that string is NULL terminated
    }

   int stringLen = strlen(trim_string);
   while(stringLen > 0 && isspace(trim_string[stringLen -1]) )
   {
      stringLen--;
   }
   trim_string[stringLen] = '\0';
}

// Must free memory after this function
char* Get_Substring(char* full_string, int start_index, int end_index)
{
   int substring_length = end_index - start_index + 1;
   char* substring = (char*)malloc(sizeof(char*) * substring_length + 1);        // allocate memory for the substring

   for( int i = 0; i< (end_index - start_index + 1); ++i)
   {
       substring[i] = full_string[i + start_index];
   }

   substring[ substring_length ] = '\0';                                // add null terminating character
   Trim_WhiteSpace( substring );
   return substring;
}

int Parse_Int(char* integer_string)
{
   int first_dig_index = 0;
   for(int i = 0; i< strlen(integer_string); ++i)
   {
       if( isdigit(integer_string[i]) || integer_string[i] == '+' || integer_string[i] == '-' )
       {
           first_dig_index = i;
           break;
       }
   }
   char* int_substr = Get_Substring(integer_string, first_dig_index, strlen(integer_string));
   char* end_ptr;
   int parsed_int = strtol(int_substr, &end_ptr, 10);   // parse the substring to integer using stringtolong function
   free(int_substr);
   return parsed_int;
 }

void Read_Console_Line(char* usr_string, int size)
{
    size_t characters;
  	size_t bufsize = 128;
	char* keyboard_input_buffer = (char*)malloc(sizeof( char* ) * bufsize );
	char send_buffer[MESSAGE_LENGTH];

    characters = getline(&keyboard_input_buffer,&bufsize,stdin); // read line from the keyboard (doesnt return until enter key pushed)
    keyboard_input_buffer[characters-1] = '\0';	// null terminate the string
    sprintf(send_buffer, "%s", keyboard_input_buffer);  // copy text read from keyboard to send buffer
    send_buffer[strlen(send_buffer)] = '\0';

    usr_string[0] = '\0';
	strncpy(usr_string, send_buffer, size);     // copy line read from console to usr_string
}

void Prompt_User_Message(char* message)
{
    printf("%s\n", message);
    Wait_Enter();
}

void Clear_Console_Screen()
{
      	printf( "\033[0m\033[2J\033[H");  // clears the terminal and brings cursor to first line
      	printf("\n");
}

int Get_Cal_Selection()
{
    int ssize = 20;
    char selection[ ssize ];
    for(int i=0; i<ssize; ++i)
        selection[i] = '\0';
    Read_Console_Line(selection, ssize);
    Clear_Line();

    int cal_choice = Parse_Int(selection);
    if(cal_choice >= 0)
    {
        switch(cal_choice)
        {
            case 0:
                exit(1);
                break;
            case 1:
                K1_OFF(true);
                break;
            case 2:
                K1_ON(true);
                break;
            case 8:
                K8_OFF(true);
                break;
            case 9:
                K9_OFF(true);
                break;
            case 10:
                K10_OFF(true);
                break;
            case 11:
                K11_OFF(true);
                break;
            case 12:
                K12_OFF(true);
                break;
            case 13:
                K13_OFF(true);
                break;
            case 14:
                K14_OFF(true);
                break;
            case 15:
                K15_OFF(true);
                break;
            case 16:
                K16_OFF(true);
                break;
            case 17:
                K17_OFF(true);
                break;
            case 20:
                K8_K9_K10_K11_ON(true);
                break;
            case 21:
                K12_K13_K14_K15_ON(true);
                break;
            case 22:
                K16_K17_ON(true);
                break;
        }
    }
    else
        return SEL_ERROR;
}


void Print_Main_Menu()
{
    Clear_Console_Screen();
    printf(COLOR_YELLOW "\t--Relay MENU--\n" COLOR_RESET);
    printf(COLOR_YELLOW "Select Relay to operate:  \n" COLOR_RESET);
        
    printf(COLOR_YELLOW "  1. K1 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  2. K1 ON\n" COLOR_RESET);
    printf(COLOR_YELLOW "  8. K8 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  9. K9 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  10. K10 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  11. K11 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  20. K8, K9, K10, K11 ON\n" COLOR_RESET);

    printf(COLOR_YELLOW "  12. K12 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  13. K13 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  14. K14 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  15. K15 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  21. K12, K13, K14, K15 ON\n" COLOR_RESET);

    printf(COLOR_YELLOW "  16. K16 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  17. K17 OFF\n" COLOR_RESET);
    printf(COLOR_YELLOW "  22. K16, K17 ON\n" COLOR_RESET);
    printf(COLOR_YELLOW "  0. Exit\n" COLOR_RESET);
}


int main( int argc, char** argv )
{
    Initialize_Relays();
    Reset_All_Relays(false);
    while(true)
    {
        Clear_Console_Screen();
        Print_Main_Menu();
        Print_Relays();
        Get_Cal_Selection();
    }
}