

#include "Menu.h"


#define MESSAGE_LENGTH 100

void Menu::Wait_Enter()
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


void Menu::Read_Console_Line(char* usr_string, int size)
{
    size_t num_characters = 0;
  	size_t bufsize = 128;
	char* keyboard_input_buffer = (char*)malloc(sizeof( char* ) * bufsize );
	char send_buffer[MESSAGE_LENGTH];

    num_characters = 0;
    bool valid_input = false;
    int cal_choice = -1;
    while(!valid_input)
    {
        num_characters = getline(&keyboard_input_buffer,&bufsize,stdin); // read line from the keyboard (doesnt return until enter key pushed)
        //printf("num characters read: %ld\n", num_characters);
        //printf("keyboard buffer: %s\n", keyboard_input_buffer);
        keyboard_input_buffer[num_characters-1] = '\0';	// null terminate the string
        sprintf(send_buffer, "%s", keyboard_input_buffer);  // copy text read from keyboard to send buffer
        send_buffer[strlen(send_buffer)] = '\0';

        if( strlen(send_buffer) > 0 )
        {
            cal_choice = StringH::Parse_Int(send_buffer);
            if( cal_choice > 0 )
                valid_input = true;
        }
        

        StringH::Erase_String(usr_string);
        strncpy(usr_string, send_buffer, size);     // copy line read from console to usr_string
    }

}


int Menu::User_Sel_Auto_or_Manual()
{
    // print menu options
    // get input
    // validate it is either a 1 or a 2
    //return the int
    Menu::Clear_Console_Screen();
    printf(COLOR_BOLD_MAGENTA "\t--CALIBRATION Select--\n" COLOR_RESET);
    printf(COLOR_YELLOW "Select Cal Mode:  \n" COLOR_RESET);
    printf(COLOR_YELLOW "  1. Automatic\n" COLOR_RESET);
    printf(COLOR_YELLOW "  2. Manual\n" COLOR_RESET);

    Terminal* terminal = new Terminal();
    terminal->Save_Settings();
    terminal->Set_For_getKey();

    bool valid_selection = false;
    char ch;

    while(!valid_selection)
    {
        ch = Menu::getKey();
        if( ch == -1)
            usleep(50000);
        if( (int)ch == 49 || (int)ch == 50)   // ASCII integer values for valid chars
        {
            valid_selection = true;
            //printf("selection: %c\n", ch);
            break;
        }
        else
        {
            usleep(10000);
            continue;
        }
    }

    terminal->Restore_Settings();
    delete terminal;
    int cal_choice = (int)ch - 48;   // get integer value from ASCII value

    switch(cal_choice)
    {
        case 1:
            //printf(COLOR_YELLOW "\nAutomatic Calibration Selected\n" COLOR_RESET);
            return CAL_AUTOMATIC;

        case 2:
            //printf(COLOR_YELLOW "\nManual Calibration Selected\n" COLOR_RESET);
            return CAL_MANUAL;
    }
}

// prints message to the terminal console, and prompts for Enter key to be pressed.
// used during the Cooler DC voltage calibration, instructing the user to set voltage to specific values
void Menu::Prompt_User_Message(char* message)
{
    printf("%s\n", message);
    Wait_Enter();
}

void Menu::Clear_Console_Screen()
{
      	printf( "\033[0m\033[2J\033[H");  // clears the terminal and brings cursor to first line
      	printf("\n");
}

void Menu::Print_Main_Menu()
{
    printf(COLOR_YELLOW "\t--CALIBRATION MENU--\n" COLOR_RESET);
    printf(COLOR_YELLOW "Select Cal Function:  \n" COLOR_RESET);
    printf(COLOR_YELLOW "  1. Cal Cooler DC Volts  \t\t(11 minutes)\n" COLOR_RESET);
    printf(COLOR_YELLOW "  2. Cal AC milliAmps (driving cooler)  (7 minutes)\n" COLOR_RESET);
    printf(COLOR_YELLOW "  3. Cal REF100   \t\t\t(1 minute)\n" COLOR_RESET);
    printf(COLOR_YELLOW "  4. Cal Load, DCV & DC milliAmps \t(25 minutes)\n" COLOR_RESET);
    printf(COLOR_YELLOW "  5. Cal Diode V    \t\t\t(40 minutes)\n" COLOR_RESET);
    printf(COLOR_YELLOW "  6. Read Thermocouples   \t\t(1 minute)\n" COLOR_RESET);
    printf(COLOR_YELLOW "  7. Done   \n" COLOR_RESET); 
}

void Menu::Move_Cursor_Up_nLines(int num_lines)
{
    printf("\033[%dA", num_lines);
}

void Menu::Move_Cursor_Down_nLines(int num_lines)
{
    printf("\033[%dB", num_lines);
}

void Menu::Move_Cursor_Forward_nColumns(int num_columns)
{
    printf("\033[5D");  // move to beginning of current line
    printf("\033[%dC", num_columns);
}

void Menu::Move_Cursor_Backward_nColumns(int num_columns)
{
    printf("\033[%dD", num_columns);
}

void Menu::Save_Cursor_Location()
{
    printf("\033[s");
}

void Menu::Restore_Cursor_Location()
{
    printf("\033[u");
}



int Menu::Get_Calibration_Selection()
{
    Menu::Clear_Console_Screen();     // Clears the screen to blank. defined in Menu class
    Menu::Print_Main_Menu();

    Terminal* terminal = new Terminal();
    terminal->Save_Settings();
    terminal->Set_For_getKey();

    bool valid_selection = false;
    char ch;

    while(!valid_selection)
    {
        ch = Menu::getKey();
        if( ch == -1)
            usleep(50000);
        if( (int)ch > 30 && (int)ch < 58)   // ASCII integer values for valid chars
        {
            valid_selection = true;
            //printf("selection: %c\n", ch);
            break;
        }
        else
        {
            usleep(10000);
            continue;
        }
    }

    terminal->Restore_Settings();
    delete terminal;
    int cal_choice = (int)ch - 48;   // get integer value from ASCII value
    if(cal_choice > 0 && cal_choice <= 7)
    {
        switch(cal_choice)
        {
            case 1:
                return CAL_COOLER_DCV;
            case 2:
                return CAL_COOLER_AC_MA;
            case 3:
                return CAL_REF100;
            case 4:
                return CAL_LOAD;
            case 5:
                return CAL_DIODE;
            case 6:
                return CAL_THERMOCOUPLES;
            case 7:
                return CAL_EXIT;
            default:
                return SEL_ERROR;
        }
    }
    else
    {
        printf("\nselection valid.\n");
        return LUT_SEL_ERROR;
    }
}

void Menu::Print_LUT_Filter_Menu(Create_LUT* creator)
{
    //Menu::Clear_Console_Screen();
    printf(COLOR_LIGHT_MAGENTA "\t--PROCESS CALIBRATION DATA--\n" COLOR_RESET);
    printf(COLOR_LIGHT_MAGENTA "\t    **LUT CREATE MENU**\n\n" COLOR_RESET);
    printf(COLOR_YELLOW "Select LUT to Process and Create:  \n" COLOR_RESET);
    printf(COLOR_YELLOW "  1. Cooler Volts LUT \n" COLOR_RESET);
    printf(COLOR_YELLOW "  2. Cooler milliAmps LUT\n" COLOR_RESET);
    printf(COLOR_YELLOW "  3. Load milliAmps LUT\n" COLOR_RESET);
    printf(COLOR_YELLOW "  4. Load Volts LUT\n" COLOR_RESET);
    printf(COLOR_YELLOW "  5. Diode Volts LUT\n" COLOR_RESET);
    printf(COLOR_YELLOW "  6. Process All LUT\n" COLOR_RESET);
    printf(COLOR_YELLOW "  7. Upload LUTs & Compile Pi DAQ App\n" COLOR_RESET);
    printf(COLOR_YELLOW "  8. Exit\n" COLOR_RESET);

    // Get the files
    // get num lines in the file
    // move the cursor up X+2 lines
    // before printing each file, advance the cursor X files
    // 
    char** log_files = creator->Get_Log_Files_Available();
    int num_files = StringH::Get_String_Array_Length(log_files);

    int num_lines_up = 9;
    Menu::Save_Cursor_Location();
    Menu::Move_Cursor_Up_nLines(num_lines_up);

    int num_columns = strlen("Select LUT to Process and Create:  ")+20;
    Menu::Print_List_Forward_nColumns(log_files, num_columns);
    Menu::Restore_Cursor_Location();
    Menu::Move_Cursor_Down_nLines(3);
}

void Menu::Print_List_Forward_nColumns(char** strlist, int num_columns)
{
    int index = 0;
    Menu::Move_Cursor_Forward_nColumns(num_columns);
    printf("--------------------------------------\n");
    Menu::Move_Cursor_Forward_nColumns(num_columns);
    printf("Log Files Available to Process:\n");

    while(strlist[index] != NULL)
    {
        char* log_filename = strlist[index];
        Menu::Move_Cursor_Forward_nColumns(num_columns);
        printf("\t  %s\n", log_filename);
        ++index;
    }
    Menu::Move_Cursor_Forward_nColumns(num_columns);
    printf("--------------------------------------\n");
}






int Menu::getKey() 
{
    char character = fgetc(stdin);
    while (getchar() != EOF);
    usleep(10000);

    return character;
}

int Menu::Get_LUT_Selection(Create_LUT* creator)
{
    Menu::Clear_Console_Screen();
    Menu::Print_LUT_Filter_Menu(creator);
    bool valid_selection = false;
    char ch;

    Terminal* terminal = new Terminal();
    terminal->Save_Settings();
    terminal->Set_For_getKey();

    while(!valid_selection)
    {
        ch = Menu::getKey();
        if( ch == -1)
            usleep(50000);
        if( (int)ch > 30 && (int)ch < 58)   // ASCII integer values for valid chars  (see ASCII decimal table)
        {
            valid_selection = true;
            //printf("selection: %c\n", ch);
            break;
        }
        else
        {
            usleep(10000);
            continue;
        }
    }

    terminal->Restore_Settings();
    int int_value = (int)ch - 48;   // get integer value from ASCII value
    if(int_value > 0)
    {
        switch(int_value)
        {
            case 1:
                return SEL_COOLER_VOLTS;
            case 2:
                return SEL_COOLER_MILLIAMPS;
            case 3:
                return SEL_LOAD_MILLIAMPS;
            case 4:
                return SEL_LOAD_VOLTS;
            case 5:
                return SEL_DIODE_VOLTS;
            case 6:
                return PROCESS_ALL_LUT;
            case 7:
                return UPLOAD_CAL_FILES;
            case 8:
                return LUT_EXIT;
            default:
                printf("selection error\n");
                return SEL_ERROR;
        }
    }
    else
    {
        printf("selection not valid.\n");
        return LUT_SEL_ERROR;
    }

}


/*
int Menu::getKey() {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    // set the terminal to raw mode 
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    // read a character from the stdin stream without blocking 
    //   returns EOF (-1) if no character is available 
    character = fgetc(stdin);

    // restore the original terminal attributes 
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
    usleep(10000);

    return character;
}
*/