
#ifndef WRITE_LOG
#define WRITE_LOG

#include <fcntl.h>  /* File Control Definitions          */
#include <errno.h>
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

#include "colors.h"
#include "LUT_Match.h"
#include "StringH.h"
#include "Data.h"
#include "Cal_Parameters.h"
#include "Thermocouple.h"

#define BUF_LEN   400
#define ADDR_LEN 20
#define FILEPATH_LEN 200
#define FILENAME_LEN 60
#define DIR_LEN 100
#define NAME_LINE_LEN 150
#define TIMEDATE_LINE_LEN 100
#define TIMESTAMP_LINE_LEN 50
#define DATE_HEADER_LEN 15
#define WRITE_LINE_LEN 200


static const char* line_hbreak = "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\
\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\
\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\
\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n";



enum class logfile_t { Load_mA, Load_V, Diode_V, Cooler_V, Cooler_mA };

// Data Structure class to hold measurements that are read from Pi or meters
class Write_Log
{
public:
        Write_Log(char* MAC_Address)    // argument is Ethernet MAC address of the PI we are calibrating
        {
            this->log_file_ptr = NULL;
            if( !this->Dir_Exists( CAL_LOG_DIR) )         // if the directory doesnt exist already, create it
            {
                printf("creating root Calibration Log file directory: '%s'\n", CAL_LOG_DIR);
                mkdir( CAL_LOG_DIR, 0777);
            }

            if( !this->Dir_Exists( ROOT_PROCESSED_DIR) )         // if the directory doesnt exist already, create it
            {
                printf("creating root Calibration Log file directory: '%s'\n", ROOT_PROCESSED_DIR);
                mkdir( ROOT_PROCESSED_DIR, 0777);
            }

            StringH::Erase_Num_Chars(this->MAC_Address, ADDR_LEN);
            // replace instances of ":" in mac address with ".", as ":" is not safe for filenames
            for(int i = 0; i<strlen(MAC_Address); ++i)
            {
                char ch = toupper( MAC_Address[i] ); // if character is a lowercase letter, convert to uppercase
                if(ch == ':')
                    ch = '.';
                this->MAC_Address[i] = ch;

                if(i == ADDR_LEN-1)
                    break;
            }
            // now MAC address will be of the form: B8.27.EB.87.3D.76

            // create LOG dir filepath
            StringH::Erase_Num_Chars(this->Log_Dir, DIR_LEN);
            strcpy(this->Log_Dir, CAL_LOG_DIR);             //  /home/pi/CAL_LOGS/


            strcat(this->Log_Dir, this->MAC_Address);       //  here should look like: /home/pi/CAL_LOGS/B8.27.EB.87.3D.76
            this->Log_Dir[ strlen(this->Log_Dir) ] = '\0';

            if( !this->Dir_Exists( this->Log_Dir) )         // if the directory doesnt exist already, create it
            {
                mkdir( this->Log_Dir, 0777);
            }
            printf("\tSave CAL DIR:  " COLOR_BOLD_MAGENTA "%s\n" COLOR_RESET, Log_Dir);
        }

        static char* Get_Date();
        static char* Get_Time();
        char* Get_MAC_Addr();
        static FILE* Open_File(char* filepath);
        static void Close_File(FILE* finished_file);
        static bool File_Exists(const char* filename);
        static bool Dir_Exists(const char* dir_path);
        void Create_DIR(char* dir_path);

        char* Get_Log_Dir();

        static FILE* Initialize_Log_File(char* filepath, char* filename, char* mac_address);
        void Write_Power_Meter_mA_Data(LUT_Match** matches, int num_matches);


        void Write_Load_mA_Data(int DAC_count, Data_Group* pi_mA, Data_Group* meter_mA);
        void Write_Volt_Data(Data_Group* pi_V, Data_Group* meter_V, logfile_t type);
        void Write_REF100_Data(Data_Group* uA_data);
        void Write_Thermocouple_Data(Thermocouple_Data* therm_data);

        void Advance_to_Index(char* write_buffer, int* index_ptr, int target_index);
        void Write_Chars(char* write_buffer, int* column_index, char* print_buffer, int MAX_CHARS);
        void Clear_Print_Buffer(char* buffer);


        char MAC_Address[ADDR_LEN];

private:
        FILE* log_file_ptr;                 //  file pointer for log file that is being written
        char Log_Filepath[FILEPATH_LEN];    // temporary variable, contains filepath of file being written. EX: /home/pi/CAL_LOGS/B8.27.EB.87.3D.76/COOL_MA_DATA.log
        char Log_Dir[DIR_LEN];              // unchanging once assigned by constructor. EX:  /home/pi/CAL_LOGS/B8.27.EB.87.3D.76/
        char Log_Filename[FILENAME_LEN];   // temp variable, contains filename (just the name, not path) EX: DIODE_V_DATA.log
};

#endif
