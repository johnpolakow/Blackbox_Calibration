
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

#include "Init_BCM2835.h"

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


bool relay_status[20];
int last_relay = 1;


void Initialize_Relays();
void Reset_All_Relays(bool print);
void Clear_Line();
void K1_Off(bool print);
void Wait_Enter();
void K1_ON(bool print);
void K1_OFF(bool print);
void K8_K9_K10_K11_ON(bool print);
void K12_K13_K14_K15_ON(bool print);
void K16_K17_ON(bool print);
void K8_K9_K10_K11_ON(bool print);
void K8_OFF(bool print);
void K9_OFF(bool print);
void K10_OFF(bool print);
void K11_OFF(bool print);
void K12_K13_K14_K15_ON(bool print);
void K12_OFF(bool print);
void K13_OFF(bool print);
void K14_OFF(bool print);
void K15_OFF(bool print);
void K16_K17_ON(bool print);
void K16_OFF(bool print);
void K17_OFF(bool print);

void Initialize_Relays()
{
    printf("\nInitializing relays off...  ");
  	Init_BCM2835_Library();
  	delayMicroseconds(1);
  	Initialize_Relay_Board_GPIO_pins();
	delayMicroseconds(10);
	Reset_All_Relays(false);
    delayMicroseconds(10);
    printf("done\n");
}

void  DelayMicroseconds(uint64_t micros)
{
	bcm2835_delayMicroseconds (micros);
}

/*
printf("\033[s");           // save cursor position
printf("\033[5;35H");       // move to line 5 column 35

printf("\033[6;35H");       // move to line 6 column 35
printf("")




printf("\033[u");           // restore cursor position

- Position the Cursor:
  \033[<L>;<C>H
     Or
  \033[<L>;<C>f
  puts the cursor at line L and column C.
- Move the cursor up N lines:
  \033[<N>A
- Move the cursor down N lines:
  \033[<N>B
- Move the cursor forward N columns:
  \033[<N>C
- Move the cursor backward N columns:
  \033[<N>D

- Clear the screen, move to (0,0):
  \033[2J
- Erase to end of line:
  \033[K

*/

void Print_Relay_Status(int relay_num)
{
    //printf("last relay: %d\n", last_relay);
    if( relay_num == last_relay)
    {
        if( relay_status[relay_num] == true)
            printf(COLOR_LIGHT_MAGENTA "K%d ON" COLOR_RESET, relay_num);
        else
            printf(COLOR_LIGHT_MAGENTA "K%d OFF" COLOR_RESET, relay_num);
    }
    else if( relay_status[relay_num] == true)
    {
        printf(COLOR_GREEN "K%d ON" COLOR_RESET, relay_num);
    }
    else
    {
        printf(COLOR_RED "K%d OFF" COLOR_RESET, relay_num);
    }
}

void Print_Relays()
{
    printf("\033[s");           // save cursor position
    printf("\033[5;35H");       // move to line 5 column 35

    Print_Relay_Status(1);      // print relay 1 status

    printf("\033[7;35H");       // move to line 7 column 35
    Print_Relay_Status(8);      // print relay 8 status
    printf("\033[8;35H");       // move to line 8 column 35
    Print_Relay_Status(9);      // print relay 9 status
    printf("\033[9;35H");       // move to line 9 column 35
    Print_Relay_Status(10);      // print relay 10 status
    printf("\033[10;35H");       // move to line 10 column 35
    Print_Relay_Status(11);      // print relay 11 status

    printf("\033[12;35H");       // move to line 12 column 35
    Print_Relay_Status(12);      // print relay 12 status
    printf("\033[13;35H");       // move to line 13 column 35
    Print_Relay_Status(13);      // print relay 13 status
    printf("\033[14;35H");       // move to line 14 column 35
    Print_Relay_Status(14);      // print relay 14 status
    printf("\033[15;35H");       // move to line 15 column 35
    Print_Relay_Status(15);      // print relay 15 status

    printf("\033[17;35H");       // move to line 17 column 35
    Print_Relay_Status(16);      // print relay 16 status
    printf("\033[18;35H");       // move to line 18 column 35
    Print_Relay_Status(17);      // print relay 17 status
    printf("\033[u"); 
}


void Clear_Line()
{
	printf("\033[1A");	// move up one line
	printf("\033[1D"); 	// move cursor back 1 columns
	printf("\033[K");	// erase to the end of the line
	printf("\033[1B");	// move down one line
	//printf("\n");
}


void K1_OFF(bool print)
{
	if(print) printf(COLOR_LIGHT_RED"\t  --  K1 OFF --\n" COLOR_RESET);// verifies other latching coil isnt being driven
	bcm2835_gpio_write(K1_RESET_SIGNAL, LOW);
	bcm2835_gpio_write(K1_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K1_SET_SIGNAL, LOW);
    relay_status[1] = false;
    last_relay = 1;
}

void K1_ON(bool print)
{
	if(print) printf(COLOR_GREEN"\t  --  K1 ON --\n" COLOR_RESET); // verifies other latching coil isnt being driven
	bcm2835_gpio_write(K1_SET_SIGNAL, LOW);
	bcm2835_gpio_write(K1_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K1_RESET_SIGNAL, LOW);
    relay_status[1] = true;
    last_relay = 1;
}


void K8_K9_K10_K11_ON(bool print)
{
	if(print)printf(COLOR_LIGHT_GREEN"\t  --  K8-K11 ON --\n" COLOR_RESET);

	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(BANK_A_RESET_SIGNAL, LOW);
    relay_status[8] = true;
    relay_status[9] = true;
    relay_status[10] = true;
    relay_status[11] = true;
    last_relay = 11;
}

void K12_K13_K14_K15_ON(bool print)
{
	if(print)printf(COLOR_LIGHT_GREEN"\t --  K12-K15 ON --\n" COLOR_RESET);
    delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(BANK_B_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
    bcm2835_gpio_write(BANK_B_RESET_SIGNAL, LOW);
    relay_status[12] = true;
    relay_status[13] = true;
    relay_status[14] = true;
    relay_status[15] = true;
    last_relay = 15;
}

void K16_K17_ON(bool print)
{
	if(print)printf(COLOR_LIGHT_RED"\t  --  K16-K17 ON --  \n" COLOR_RESET);
    delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(CAL_RESET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
    bcm2835_gpio_write(CAL_RESET_SIGNAL, LOW);
    relay_status[16] = true;
    relay_status[17] = true;
    last_relay = 17;
}

void Reset_All_Relays(bool print)
{
	if(print)printf(COLOR_BOLD_RED"\tALL RELAYS OFF\n" COLOR_RESET);
	K1_OFF(print);
    K8_OFF(false);
    K9_OFF(false);
    K10_OFF(false);
    K11_OFF(false);
    K12_OFF(false);
    K13_OFF(false);
    K14_OFF(false);
    K15_OFF(false);
    K16_OFF(false);
    K17_OFF(false);

    for(int i=0; i<20; ++i)
        relay_status[i] = false;

	delayMicroseconds(ONE_MILLISECOND*500);
    last_relay = 17;
}


// these are the cal resistor relays, and have opposite polarity. Turning them "on" disconnects the associated resistor
void K8_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K8 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K8_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K8_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[8] = false;
    last_relay = 8;
}

void K9_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K9 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K9_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K9_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[9] = false;
    last_relay = 9;
}

// Diode sense relay
// connects voltage measurement of HP_A to diode sense wires
void K10_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K10 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K10_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K10_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[10] = false;
    last_relay = 10;
}

void K11_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K11 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K11_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K11_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[11] = false;
    last_relay = 11;
}

// I_Load -> HP34401B relay
void K12_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K12 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K12_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K12_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[12] = false;
    last_relay = 12;
}

// V_cooler relay
void K13_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K13 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K13_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K13_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[13] = false;
    last_relay = 13;
}

// REF100 relay
void K14_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K14 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K14_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K14_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[14] = false;
    last_relay = 14;
}

// V_Load div8 relay
void K15_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K15 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K15_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K15_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[15] = false;
    last_relay = 15;
}


// selector between diode volts or cooler volts -> calibration voltage
void K16_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K16 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K16_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K16_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[16] = false;
    last_relay = 16;
}

// connects calibration voltage source at J17/J20 to calibration bus
void K17_OFF(bool print)
{
	if(print)printf(COLOR_RED"\t## K17 OFF ##\n" COLOR_RESET);
	delayMicroseconds(ONE_MILLISECOND);
	bcm2835_gpio_write(K17_SET_SIGNAL, HIGH);
	delayMicroseconds(PULSE_ON);
	bcm2835_gpio_write(K17_SET_SIGNAL, LOW);
	delayMicroseconds(20);
    relay_status[17] = false;
    last_relay = 17;
}


void Set_Relays_Cal_Cooler_VDC()
{
    // hook up voltage source to board at J17/J20
	bcm2835_gpio_write(K10_SET_SIGNAL , HIGH);  // turn off K10
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




#ifdef __cplusplus
}
#endif

#endif
