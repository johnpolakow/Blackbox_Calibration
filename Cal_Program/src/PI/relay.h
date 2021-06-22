
#ifndef _RELAY_H_
#define _RELAY_H_

#ifdef __arm__
    #include <bcm2835.h>
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>  // UNIX Standard function definitions
#include <fcntl.h>   // File control
#include <errno.h>   // Error number def
#include <termios.h> // POSIX terminal control
#include <termio.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>


#include "Pin_Defs.h"
#include "Init_BCM2835.h"
#include "Struct_Defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_RELAY false
#define PULSE_ON	70*ONE_MILLISECOND

#define COLOR_RED           "\x1b[31m"
#define COLOR_GREEN         "\x1b[32m"
#define COLOR_YELLOW        "\x1b[33m"
#define COLOR_BLUE          "\x1b[34m"
#define COLOR_MAGENTA       "\x1b[35m"
#define COLOR_CYAN          "\x1b[36m"
#define COLOR_BROWN         "\033[22;33m"
#define COLOR_GRAY          "\033[22;37m"
#define COLOR_LIGHT_RED     "\033[01;31m"
#define COLOR_LIGHT_GREEN   "\033[01;32m"
#define COLOR_LIGHT_CYAN    "\033[01;36m"
#define COLOR_LIGHT_MAGENTA "\033[01;35m"
#define COLOR_LIGHT_BLUE    "\033[01;34m"
#define COLOR_RESET         "\x1b[0m"
#define COLOR_BRIGHT_GREEN  "\033[0;92m"
#define COLOR_BRIGHT_YELLOW "\033[0;93m"
#define COLOR_BRIGHT_BLUE   "\033[0;94m"
#define COLOR_BRIGHT_MAGENTA "\033[0;95m"
#define COLOR_BRIGHT_CYAN   "\033[0;96m"
#define COLOR_BRIGHT_WHITE  "\033[0;97m"
#define COLOR_BOLD_RED      "\033[1;91m"
#define COLOR_BOLD_GREEN    "\033[1;92m"
#define COLOR_BOLD_YELLOW   "\033[1;93m"
#define COLOR_BOLD_BLUE     "\033[1;94m"
#define COLOR_BOLD_MAGENTA  "\033[1;95m"
#define COLOR_BOLD_CYAN     "\033[1;96m"
#define COLOR_BOLD_WHITE    "\033[1;97m"

static const int ASCII_NEW_LINE  = 10;	       // ASCII integer code for new line
static const int ASCII_CARRIAGE_RETURN = 13;	// ASCII integer code for carriage return
static const int ASCII_QUIT      = 113;       // ASCII for 'q'
static const int ASCII_ENTER_KEY = 10;        // ASCII character code

void Serial_Console_Main_Menu();
void Get_User_Input_String(char* usr_string);
void Relay_Menu(RELAY* selected_relay);


void Initialize_Relays();
RELAY* Get_Relay_Object(int relay_number);
void Reset_All_Relays(bool print);
void Clear_Line();

void K1_Off(bool print);
void K2_K3_K4_K5_K6_K7_Off(bool print);
void K8_K9_K10_K11_Off(bool print);
void K12_K13_K14_K15_Off(bool print);
void K16_K17_Off(bool print);

void Set_Relay_REF100();
void Set_Relay_Read_Load_I();
void Set_Relay_Load_V();
void Set_Relay_Read_VRef();

void Set_Relays_Cal_Resistor(int resistor_number);
void Set_Relays_Cal_Load();
void Set_Relays_Cal_Cooler_VDC();
void Set_Relays_Cal_Cooler_AC_Current();
void Set_Relays_Cal_Diode_Volts();

void Wait_Enter();

void K1_On(bool print);
void K2_On(bool print);
void K3_On(bool print);
void K4_On(bool print);
void K5_On(bool print);
void K6_On(bool print);
void K7_On(bool print);
void K8_On(bool print);
void K9_On(bool print);
void K10_On(bool print);
void K11_On(bool print);
void K12_On(bool print);
void K13_On(bool print);
void K14_On(bool print);
void K15_On(bool print);
void K16_On(bool print);
void K17_On(bool print);

void Initialize_Relays()
{
    printf("\nInitializing relays off...  ");
  	Init_BCM2835_Library();
  	delayMicroseconds(1);
  	Initialize_Relay_Board_GPIO_pins();
	Assign_Relay_Board_GPIO_pins();
	delayMicroseconds(10);
	Reset_All_Relays(false);
    delayMicroseconds(10);
    printf("done\n");
}

void Wait_Enter()
{
	int key_pressed;
	bool Enter_Pressed = false;
    printf("\n\t\t< to continue hit Enter >\n");
	while(!Enter_Pressed)
	{
			key_pressed = getchar();
			if((key_pressed == ASCII_NEW_LINE) || (key_pressed == ASCII_CARRIAGE_RETURN))
			{ Enter_Pressed = true; }
			if(key_pressed == ASCII_QUIT)
			{ Enter_Pressed = true; }
	}
	return;
}

void  DelayMicroseconds(uint64_t micros)
{
	bcm2835_delayMicroseconds (micros);
}


void Clear_Line()
{
	printf("\033[1A");	// move up one line
	printf("\033[1D"); 	// move cursor back 1 columns
	printf("\033[K");		// erase to the end of the line
	printf("\033[1B");	// move down one line
	//printf("\n");
}

RELAY* Get_Relay_Object(int relay_number)
{
	RELAY* relay_obj;
	switch( relay_number )
	{
		case (int)(K1_RELAY_NUMBER):
			relay_obj = &K1;
			break;
		case (int)(K2_RELAY_NUMBER):
			relay_obj = &K2;
			break;
		case (int)(K3_RELAY_NUMBER):
			relay_obj = &K3;
			break;
		case (int)(K4_RELAY_NUMBER):
			relay_obj = &K4;
			break;
		case (int)(K5_RELAY_NUMBER):
			relay_obj = &K5;
			break;
		case (int)(K6_RELAY_NUMBER):
			relay_obj = &K6;
			break;
		case (int)(K7_RELAY_NUMBER):
			relay_obj = &K7;
			break;
		case (int)(K8_RELAY_NUMBER):
			relay_obj = &K8;
			break;
		case (int)(K9_RELAY_NUMBER):
			relay_obj = &K9;
			break;
		case (int)(K10_RELAY_NUMBER):
			relay_obj = &K10;
			break;
		case (int)(K11_RELAY_NUMBER):
			relay_obj = &K11;
			break;
		case (int)(K12_RELAY_NUMBER):
			relay_obj = &K12;
			break;
		case (int)(K13_RELAY_NUMBER):
			relay_obj = &K13;
			break;
		case (int)(K14_RELAY_NUMBER):
			relay_obj = &K14;
			break;
		case (int)(K15_RELAY_NUMBER):
			relay_obj = &K15;
			break;
		case (int)(K16_RELAY_NUMBER):
			relay_obj = &K16;
			break;
		case (int)(K17_RELAY_NUMBER):
			relay_obj = &K17;
			break;
	}
	return relay_obj;
}

void K1_Off(bool print)
{
	if(print) printf(COLOR_LIGHT_RED"\t  --  K1 OFF --\n" COLOR_RESET);// verifies other latching coil isnt being driven
	bcm2835_gpio_write(K1_RESET_SIGNAL, LOW);
	bcm2835_gpio_write(K1_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K1_SET_SIGNAL, LOW);
}

void K1_On(bool print)
{
	if(print) printf(COLOR_GREEN"\t  --  K1 ON --\n" COLOR_RESET); // verifies other latching coil isnt being driven
	bcm2835_gpio_write(K1_SET_SIGNAL, LOW);
	bcm2835_gpio_write(K1_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K1_RESET_SIGNAL, LOW);
}


// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K2_K3_K4_K5_K6_K7_Off(bool print)
{
	if(print)printf(COLOR_LIGHT_RED"\t  --  K2-K7 OFF --\n" COLOR_RESET);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);  // verifies other latching coil isnt being driven
	delayMicroseconds(ONE_MILLISECOND*10);

	bcm2835_gpio_write(K2_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K2_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K3_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K3_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K4_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K4_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K5_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K5_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K6_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K6_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K7_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K7_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);
}

void K8_K9_K10_K11_Off(bool print)
{
	if(print)printf(COLOR_LIGHT_RED"\t  --  K8-K11 OFF --\n" COLOR_RESET);

	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);

	bcm2835_gpio_write(K8_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K8_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K9_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K9_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K10_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K10_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K11_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K11_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);
}

void K12_K13_K14_K15_Off(bool print)
{
	if(print)printf(COLOR_LIGHT_RED"\t --  K12-K15 OFF --\n" COLOR_RESET);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K12_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K12_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K13_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K13_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K14_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K14_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K15_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K15_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);
}

void K16_K17_Off(bool print)
{
	if(print)printf(COLOR_LIGHT_RED"\t  --  K16-K17 OFF --  \n" COLOR_RESET);
	bcm2835_gpio_write(CAL_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K16_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K16_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K17_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K17_SET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);
}

void Reset_All_Relays(bool print)
{
	if(print)printf(COLOR_BOLD_RED"\tALL RELAYS OFF\n" COLOR_RESET);
	K1_Off(print);
	K2_K3_K4_K5_K6_K7_Off(print);
	K8_K9_K10_K11_Off(print);
	K12_K13_K14_K15_Off(print);
	K16_K17_Off(print);
	delayMicroseconds(ONE_MILLISECOND*500);
	K2_K3_K4_K5_K6_K7_Off(false);
}

// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K2_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K2 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, HIGH);  // verifies other latching coil isnt being driven
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K3_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K3_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K4_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K4_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K5_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K5_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K6_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K6_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K7_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K7_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K3_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K3 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, HIGH);  // verifies other latching coil isnt being driven
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K2_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K2_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K4_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K4_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K5_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K5_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K6_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K6_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K7_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K7_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K4_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K4 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, HIGH);  // verifies other latching coil isnt being driven
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K2_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K2_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K3_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K3_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K5_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K5_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K6_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K6_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K7_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K7_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K5_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K5 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, HIGH);  // verifies other latching coil isnt being driven
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K2_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K2_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K3_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K3_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K4_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K4_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K6_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K6_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K7_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K7_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K6_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K6 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, HIGH);  // verifies other latching coil isnt being driven
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K2_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K2_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K3_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K3_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K4_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K4_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K5_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K5_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K7_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K7_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K7_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K7 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);  // verifies other latching coil isnt being driven
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, HIGH);  // verifies other latching coil isnt being driven
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(RES_ARRAY_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K2_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K2_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K3_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K3_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K4_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K4_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K5_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K5_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K6_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K6_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}


// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K8_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K8 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K9_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K9_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K10_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K10_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K11_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K11_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// Voltage Ref Relay
void K9_On(bool print)
{
	if(print)printf(COLOR_GREEN"  ## K9 ON ##  " COLOR_RESET);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K8_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K8_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K10_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K10_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K11_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K11_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// Diode sense relay
// connects voltage measurement of HP_A to J13/J14
void K10_On(bool print)
{
	if(print)printf(COLOR_GREEN"  ## K10 ON ##  " COLOR_RESET);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, HIGH);
	delayMicroseconds(2*PULSE_ON);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K8_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K8_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K9_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K9_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K11_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K11_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// Cooler V_div 4
void K11_On(bool print)
{
	if(print)printf(COLOR_GREEN"  ## K11 ON ##  " COLOR_RESET);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K8_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K8_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K9_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K9_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K10_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K10_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// I_Load -> HP34401B relay
void K12_On(bool print)
{
	if(print)printf(COLOR_GREEN"  ## K12 ON ##  " COLOR_RESET);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K13_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K13_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K14_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K14_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K15_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K15_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// V_cooler relay
void K13_On(bool print)
{
	if(print)printf(COLOR_GREEN"  ## K13 ON ##  " COLOR_RESET);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K12_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K12_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K14_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K14_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K15_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K15_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// REF100 relay
void K14_On(bool print)
{
	if(print)printf(COLOR_GREEN "\t## K14 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K12_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K12_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K13_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K13_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K15_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K15_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// V_Load div8 relay
void K15_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K15 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K12_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K12_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K13_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K13_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K14_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K14_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}


// selector between diode volts or cooler volts -> calibration voltage
void K16_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K16 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(CAL_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(CAL_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K17_SET_SIGNAL, HIGH);	// turns "off" K17
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K17_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

// connects calibration voltage source at J17/J20 to calibration bus
void K17_On(bool print)
{
	if(print)printf(COLOR_GREEN"\t## K17 ON ##\n" COLOR_RESET);
	bcm2835_gpio_write(CAL_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(CAL_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K16_SET_SIGNAL, HIGH);	// turns "off" K17
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K16_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

void Set_Relay_CCC()
{
    // CCC Mode
        // Relay 1 Off      ( default, but maybe it should be on )
        // Read from BK5335
	K1_On(true);	// sets power to go to CCC
	delayMicroseconds(100);
}

void Set_Relays_Cal_Load()
{
    printf("\tsetting relays to measure load V, load mA ...\n");
    bool print = false;
	K1_Off(print);
	K2_K3_K4_K5_K6_K7_Off(print);
	K16_K17_Off(print);
	delayMicroseconds(ONE_MILLISECOND*500);
	K2_K3_K4_K5_K6_K7_Off(false);

    // turn on relay K8
    K8_On(print);
    //printf("\n");

    // TURN ON RELAYS K12 & K15
    // first turn ON all relays on BANK B (K12, K13, K14, K15)
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

    // leave on K12 & K15, selectively turning OFF K13, K14
	bcm2835_gpio_write(K13_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K13_SET_SIGNAL, LOW);
	delayMicroseconds(20);

	bcm2835_gpio_write(K14_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K14_SET_SIGNAL, LOW);
	delayMicroseconds(20);
}

void Set_Relays_Cal_Resistor(int resistor_number)
{
    // Calibrate DC current delivered out of PI_DAQ-> cooler
    // Argument: different resistors, 1 through 6, 1 being the lowest value
		// 1 == 20 Ohms
		// 2 == 24 Ohms
		// 3 == 30 Ohms
		// 4 == 150 Ohms
		// 5 == 56 Ohms
		// 6 == 120 Ohms
	K1_Off(false);

	// Intermittently switch on relay 2 through 7, measuring current with BK5335
    switch(resistor_number)
	{
		case 1:
			// turn on resistor #1, which is relay K7
			K7_On(true);
			break;
		case 2:
			// turn on resistor #2, which is relay K6
			K6_On(true);
			break;
		case 3:
			// turn on resistor #3, which is relay K5
			K5_On(true);
			break;
		case 4:
			// turn on resistor #4, which is relay K4
			K4_On(true);
			break;
		case 5:
			// turn on resistor #5, which is relay K3
			K3_On(true);
			break;
		case 6:
			// turn on resistor #6, which is relay K2
			K2_On(true);
			break;
	}
}

// read from 34401_A & 34401_B
void Set_Relays_Cal_Cooler_AC_Current()
{
//Cal Cooler Volts ADC
    // BK5335 is in series with K1
    // turn on relay K1 to power CCC
    printf(COLOR_MAGENTA "\n\tturning cooler power on ...\t" COLOR_RESET);
    K1_On(true);
    printf("\n");
	delayMicroseconds(100);
}

void Set_Relays_Cal_Cooler_VDC()
{
    // hook up voltage source to board at J17/J20
	bcm2835_gpio_write(K10_SET_SIGNAL , HIGH);  // turn off K12
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K10_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);     // this turns on all relays on bank A:  K12, K13, K14, K15
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

    // leave K13 on turn off K12, K14, & K15
    // K13 == connects HP34401B to calibration voltage source
	bcm2835_gpio_write(K12_SET_SIGNAL , HIGH);  // turn off K12
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K12_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

    bcm2835_gpio_write(K14_SET_SIGNAL , HIGH);  // turn off K14
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K14_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

    bcm2835_gpio_write(K15_SET_SIGNAL , HIGH);  // turn off K15
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K15_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K9_SET_SIGNAL , HIGH);   // turn off K9
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K9_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K11_SET_SIGNAL , HIGH);  // turn off K11
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K11_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

    bcm2835_gpio_write(K10_SET_SIGNAL , HIGH);  // turn off K10
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K10_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(CAL_RESET_SIGNAL , HIGH);    // turn ON, both K16, K17
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(CAL_RESET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

    bcm2835_gpio_write(K16_SET_SIGNAL , HIGH);  // turn off K16
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K16_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	delayMicroseconds(100);
    // read 34401_B for Pi output voltage to cooler
}

// read from 34401_A
// sets all relays for calibrating the diode voltage channel o the PI
// K10 on
// K17 on
// k16 on
// k13 off 
// k12 off
void Set_Relays_Cal_Diode_Volts()
{
    // hook up voltage source to board at J17/J20

	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, HIGH);     // this turns on all relays on bank A:  K8, K9, K10, K11
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);     // this turns on all relays on bank A:  K12, K13, K14, K15
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K12_SET_SIGNAL , HIGH);  // turn off K12
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K12_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K13_SET_SIGNAL , HIGH);  // turn off K13
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K13_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

    bcm2835_gpio_write(K15_SET_SIGNAL , HIGH);  // turn off K15
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K15_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K9_SET_SIGNAL , HIGH);   // turn off K9
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K9_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(K11_SET_SIGNAL , HIGH);  // turn off K11
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K11_SET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	bcm2835_gpio_write(CAL_RESET_SIGNAL , HIGH);    // turn ON, both K16, K17
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(CAL_RESET_SIGNAL , LOW);
	delayMicroseconds(ONE_MILLISECOND);

	delayMicroseconds(100);
    // read 34401_A for Diode Volts
}


void Set_Relay_Load_V()
{
// Cal Load Sense Voltage
    // jumpers from PCB Load out to relay box
    // add in some type of load
    // turn on relay K8
	K8_On(true);
	delayMicroseconds(100);
    printf("\n");
    // read voltage from load with 34401_A

    // turn on K15, read VLoad/div8 with 34401_B and Pi
	K15_On(true);
}


void Set_Relay_Read_Load_I()
{
// Cal Load Current
    // turn on K12. Measure with both PI and 34401_B
	K12_On(true);
    printf("\n");
	delayMicroseconds(100);
}


void Set_Relay_REF100()
{
// Cal Diode Current
    // turn on K14
    // read REF100 current with 34401_B.
    Reset_All_Relays(false);
	K14_On(true);
    printf("\n");
	delayMicroseconds(100);
}

void Set_Relay_Read_VRef()
{
// Read Cal board dual voltage reference
    // Set K9, read dual 2V5 ref with 34401_A
    Reset_All_Relays(false);
    K9_On(true);
	delayMicroseconds(100);
    printf("\n");
}


#ifdef __cplusplus
}
#endif

#endif
