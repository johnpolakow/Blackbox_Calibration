#ifndef MENU_H
#define MENU_H


#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <termios.h>

#include "StringH.h"
#include "Struct_Defs.h"
#include "colors.h"
#include "Create_LUT.h"

#define STDIN_FILENO 0

static const int NEW_LINE  = 10;	       // ASCII integer code for new line
static const int CARRIAGE_RETURN = 13;	// ASCII integer code for carriage return
static const int QUIT      = 113;       // ASCII for 'q'
static const int ENTER_KEY = 10;        // ASCII character code


const static int CAL_COOLER_DCV = 1;
const static int CAL_COOLER_AC_MA = 2;
const static int CAL_REF100 = 3;
const static int CAL_LOAD = 4;
const static int CAL_DIODE = 5;
const static int CAL_THERMOCOUPLES = 6;
const static int CAL_EXIT = 7;
const static int SEL_ERROR = 10;
const static int CAL_AUTOMATIC = 11;
const static int CAL_MANUAL = 12;

const static int SEL_COOLER_VOLTS = 1;
const static int SEL_COOLER_MILLIAMPS = 2;
const static int SEL_LOAD_MILLIAMPS = 3;
const static int SEL_LOAD_VOLTS = 4;
const static int SEL_DIODE_VOLTS = 5;
const static int PROCESS_ALL_LUT = 6;
const static int UPLOAD_CAL_FILES = 7;
const static int LUT_EXIT = 8;
const static int LUT_SEL_ERROR = 9;


class Terminal
{
public:
    Terminal(){ this->Save_Settings(); }

    void Save_Settings()
    {
        /* save current terminal settings */
        tcgetattr(fileno(stdin), &orig_term_attr);  // save current settings
    }

    void Set_For_getKey()
    {
        struct termios new_term_attr;
        memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));    // copy from original terminal settings to save work
        
        /* set the terminal to raw mode (not buffered) to get any pressed chars immediately */
        new_term_attr.c_lflag &= ~(ECHO|ICANON);
        new_term_attr.c_cc[VTIME] = 0;
        new_term_attr.c_cc[VMIN] = 0;
        tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
    }

    void Restore_Settings()
    {
        tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);     // restore terminal attributes
    }

    struct termios orig_term_attr;
};



// Data Structure class to hold measurements read from Pi or meters
class Menu
{
public:
        Menu();

        ~Menu();

        static void Print_Main_Menu();
        static void Wait_Enter();
        static void Prompt_User_Message(char* message);
        static void Clear_Console_Screen();
        static int Get_Calibration_Selection();
        static int Get_LUT_Selection(Create_LUT* creator);
        static int User_Sel_Auto_or_Manual();
        static void Print_Mode_Menu();
        static void Print_LUT_Filter_Menu(Create_LUT* creator);
        static void Get_LUT_Filter_Selection();
        static int getKey();

        static void Move_Cursor_Up_nLines(int num_lines);
        static void Move_Cursor_Down_nLines(int num_lines);
        static void Move_Cursor_Forward_nColumns(int num_columns);
        static void Move_Cursor_Backward_nColumns(int num_columns);
        static void Save_Cursor_Location();
        static void Restore_Cursor_Location();

        static void Print_List_Forward_nColumns(char** strlist, int num_columns);

private:
        static void Read_Console_Line(char* usr_string, int size);
};

#endif
