

#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>



#include "colors.h"
#include "LC_LUT.h"
#include "LC_Data.h"
#include "String_Tokenizer.h"
#include "DC_Data.h"
#include "DC_LUT.h"



class Create_LUT
{
public:

    Create_LUT(const char* log_dir_location, const char* dut_mac_addr)
    {
        if(log_dir_location == NULL)
            return;
        
        this->DUT_mac_addr = strdup(dut_mac_addr);
        int len = strlen(log_dir_location) +2;
        this->log_dir = (char*)malloc(sizeof(char)*len);
        strcpy(this->log_dir, log_dir_location);

        len = strlen(dut_mac_addr) +2;
        this->device_folder_name = (char*)malloc(sizeof(char)*len);
        strcpy(this->device_folder_name, dut_mac_addr);

        if( !FileH::Dir_Exists( ROOT_LUT_DIR ) )         // if the directory doesnt exist already, create it
        {
            printf("creating root LUT file directory: '%s'\n", ROOT_LUT_DIR);
            mkdir( ROOT_LUT_DIR, 0777);
        }

        strcpy(this->LUT_DIR, ROOT_LUT_DIR);
        strcat(this->LUT_DIR, dut_mac_addr);
        strcat(this->LUT_DIR, "/");
        FileH::Create_Dir(this->LUT_DIR);
        printf("LUT Dir Location: %s\n", this->LUT_DIR);

        //this->Print_Log_Files_Available();
    }

    ~Create_LUT()
    {
        free((char*)this->log_dir);
        free((char*)this->device_folder_name);
        free((char*)this->DUT_mac_addr);
    }

    void Print_Log_Files_Available();
    char** Get_Log_Files_Available();
    void Process_Log_Files();

    void Process_DAC_LUT(LUT_TYPE type, char* data_in_filepath, const char* LUT_filepath, bool move_file = false);
    void Process_AC_LUT(LUT_TYPE type, char* data_in_filepath, const char* LUT_filepath);
    void Process_DC_LUT(LUT_TYPE type, char* data_in_filepath, const char* LUT_filepath);
    void Process_All_LUT_Files();

    char* Get_Cooler_mA_Log_File();
    char* Get_Cooler_V_Log_File();
    char* Get_Load_mA_Log_File();
    char* Get_Load_V_Log_File();
    char* Get_Diode_V_Log_File();

    char LUT_DIR[200];

private:
    char* log_dir;
    char* device_folder_name;
    char* DUT_mac_addr;
};


