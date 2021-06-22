

#include "Create_LUT.h"
#include "FileH.h"      // File Helper functions
#include "LC_LUT.h"        // class declarations: LUT_TYPE, LUT_HEADER, LUT_POINT, LUT_GROUP, LUT




void Create_LUT::Print_Log_Files_Available()
{
    printf("LOG FILES AVAILABLE in:  %s\n", this->log_dir);
    //printf(COLOR_BOLD_MAGENTA "-------------------------------------------------------------------\n" COLOR_RESET);
    char** log_files = FileH::List_Files(this->log_dir);
    //printf(COLOR_BOLD_MAGENTA "-------------------------------------------------------------------\n" COLOR_RESET);
    StringH::Free_String_Array_Memory(log_files);
}

char** Create_LUT::Get_Log_Files_Available()
{
    char** log_files = FileH::Get_Files_in_Dir(this->log_dir);
    return log_files;
}


void Create_LUT::Process_Log_Files()
{
    char** log_files = FileH::List_Files(this->log_dir);
    int index = 0;
    while(log_files[index] != NULL)
    {
        printf(COLOR_BOLD_MAGENTA "\n-------------------------------------------------------------------\n" COLOR_RESET);
        //printf("\tfile: %s\n", list[index]);
        char* log_filename = log_files[index];
        char* log_filepath = FileH::Get_Filepath(this->log_dir, log_filename);
        printf("\nParsing:  %s\n", log_filepath);

        LUT_TYPE LUT_type;
        //printf("filename: " COLOR_LIGHT_MAGENTA "%s" COLOR_RESET "\n", log_filename);
        LUT_type = LC_LUT::Get_LUT_Type(log_filename); // match to LUT type based on filename

        if(LUT_type == NONE)
        {
            LUT_type = LC_LUT::Get_LUT_Type_From_Content(log_filepath); // match to LUT type based on content in file header
        }

        //char* typestring = LC_LUT::Get_LUT_Type_String(LUT_type);
        //printf("LUT_Type: %s\n", typestring);
        //free(typestring);
        char LUT_filepath[200];
        StringH::Erase_Num_Chars(LUT_filepath,200);
        strcpy(LUT_filepath, this->LUT_DIR);

        switch(LUT_type)
        {
            case COOLER_MA_t:
                strcat(LUT_filepath, COOLER_MA_LUT);
                Process_AC_LUT(COOLER_MA_t, log_filepath, LUT_filepath);
                break;
            case COOLER_V_t:
                strcat(LUT_filepath, COOLER_V_LUT);
                Process_DC_LUT(COOLER_V_t, log_filepath, LUT_filepath);
                break;
            case LOAD_MA_t:
                strcat(LUT_filepath, DAC_COUNT_LUT);
                Process_DAC_LUT(DAC_COUNT_t, log_filepath, LUT_filepath);
                
                strcpy(LUT_filepath, ROOT_LUT_DIR);
                strcat(LUT_filepath, LOAD_MA_LUT);
                Process_DAC_LUT(LOAD_MA_t, log_filepath, LUT_filepath, true);
                break;
            case LOAD_V_t:
                strcat(LUT_filepath, LOAD_V_LUT);
                Process_DC_LUT(LOAD_V_t, log_filepath, LUT_filepath);
                break;
            case DIODE_V_t:
                strcat(LUT_filepath, DIODE_V_LUT);
                Process_DC_LUT(DIODE_V_t, log_filepath, LUT_filepath);
                break;
            case REF100_t:
                break;
            case THERMOCOUPLE_t:
                break;
            case NONE:
                break;
        }
        free(log_filepath);
        ++index;

    }
    printf(COLOR_BOLD_MAGENTA "\n-------------------------------------------------------------------\n" COLOR_RESET);
    StringH::Free_String_Array_Memory(log_files);
    printf("LUT FILES:\n");
    char** list = FileH::List_Files(LUT_DIR);
    StringH::Free_String_Array_Memory(list);
}


char* Create_LUT::Get_Cooler_mA_Log_File()
{

    char** log_files = FileH::Get_Files_in_Dir(this->log_dir);
    int index = 0;
    char target_filepath[200];
    StringH::Erase_Num_Chars(target_filepath,200);

    while(log_files[index] != NULL)
    {
        char* log_filename = log_files[index];
        char* log_filepath = FileH::Get_Filepath(this->log_dir, log_filename);

        LUT_TYPE LUT_type;
        LUT_type = LC_LUT::Get_LUT_Type(log_filename); // match to LUT type based on filename

        if(LUT_type == NONE)
        {
            LUT_type = LC_LUT::Get_LUT_Type_From_Content(log_filepath); // match to LUT type based on content in file header
        }

        if( LUT_type == COOLER_MA_t)
        {
            strcpy(target_filepath, log_filepath);
            free(log_filepath);
            break;
        }
      
        free(log_filepath);
        ++index;

    }
    StringH::Free_String_Array_Memory(log_files);

    if( strlen(target_filepath) > 0)
    {
        //printf("read file: %s\n", target_filepath);
        return strdup(target_filepath);
    }
    else
    {
        printf("file not found.\n");
        return NULL;
    }
}

char* Create_LUT::Get_Cooler_V_Log_File()
{
    char** log_files = FileH::Get_Files_in_Dir(this->log_dir);
    int index = 0;
    char target_filepath[200];
    StringH::Erase_Num_Chars(target_filepath,200);

    while(log_files[index] != NULL)
    {
        char* log_filename = log_files[index];
        char* log_filepath = FileH::Get_Filepath(this->log_dir, log_filename);

        LUT_TYPE LUT_type;
        LUT_type = LC_LUT::Get_LUT_Type(log_filename); // match to LUT type based on filename

        if(LUT_type == NONE)
        {
            LUT_type = LC_LUT::Get_LUT_Type_From_Content(log_filepath); // match to LUT type based on content in file header
        }

        if( LUT_type == COOLER_V_t)
        {
            strcpy(target_filepath, log_filepath);
            free(log_filepath);
            break;
        }
      
        free(log_filepath);
        ++index;

    }
    StringH::Free_String_Array_Memory(log_files);

    if( strlen(target_filepath) > 0)
    {
        //printf("read file: %s\n", target_filepath);



        return strdup(target_filepath);
    }
    else
    {
        printf("file not found.\n");
        return NULL;
    }

}

char* Create_LUT::Get_Load_mA_Log_File()
{
    char** log_files = FileH::Get_Files_in_Dir(this->log_dir);
    int index = 0;
    char target_filepath[200];
    StringH::Erase_Num_Chars(target_filepath,200);

    while(log_files[index] != NULL)
    {
        char* log_filename = log_files[index];
        char* log_filepath = FileH::Get_Filepath(this->log_dir, log_filename);

        LUT_TYPE LUT_type;
        LUT_type = LC_LUT::Get_LUT_Type(log_filename); // match to LUT type based on filename

        if(LUT_type == NONE)
        {
            LUT_type = LC_LUT::Get_LUT_Type_From_Content(log_filepath); // match to LUT type based on content in file header
        }

        if( LUT_type == LOAD_MA_t)
        {
            strcpy(target_filepath, log_filepath);
            free(log_filepath);
            break;
        }
        
        free(log_filepath);
        ++index;
    }
    StringH::Free_String_Array_Memory(log_files);

    if( strlen(target_filepath) > 0)
    {
        //printf("read file: %s\n", target_filepath);
        return strdup(target_filepath);
    }
    else
    {
        printf("file not found.\n");
        return NULL;
    }
}

char* Create_LUT::Get_Load_V_Log_File()
{
    char** log_files = FileH::Get_Files_in_Dir(this->log_dir);
    int index = 0;
    char target_filepath[200];
    StringH::Erase_Num_Chars(target_filepath,200);

    while(log_files[index] != NULL)
    {
        char* log_filename = log_files[index];
        char* log_filepath = FileH::Get_Filepath(this->log_dir, log_filename);

        LUT_TYPE LUT_type;
        LUT_type = LC_LUT::Get_LUT_Type(log_filename); // match to LUT type based on filename

        if(LUT_type == NONE)
        {
            LUT_type = LC_LUT::Get_LUT_Type_From_Content(log_filepath); // match to LUT type based on content in file header
        }

        if( LUT_type == LOAD_V_t)
        {
            strcpy(target_filepath, log_filepath);
            free(log_filepath);
            break;
        }
        
        free(log_filepath);
        ++index;

    }
    StringH::Free_String_Array_Memory(log_files);

    if( strlen(target_filepath) > 0)
    {
        //printf("read file: %s\n", target_filepath);
        return strdup(target_filepath);
    }
    else
    {
        printf("file not found.\n");
        return NULL;
    }
}

char* Create_LUT::Get_Diode_V_Log_File()
{
    char** log_files = FileH::Get_Files_in_Dir(this->log_dir);
    int index = 0;
    char target_filepath[200];
    StringH::Erase_Num_Chars(target_filepath,200);

    while(log_files[index] != NULL)
    {
        char* log_filename = log_files[index];
        char* log_filepath = FileH::Get_Filepath(this->log_dir, log_filename);

        LUT_TYPE LUT_type;
        LUT_type = LC_LUT::Get_LUT_Type(log_filename); // match to LUT type based on filename

        if(LUT_type == NONE)
        {
            LUT_type = LC_LUT::Get_LUT_Type_From_Content(log_filepath); // match to LUT type based on content in file header
        }

        if( LUT_type == DIODE_V_t)
        {
            strcpy(target_filepath, log_filepath);
            free(log_filepath);
            break;
        }
        
        free(log_filepath);
        ++index;

    }
    StringH::Free_String_Array_Memory(log_files);

    if( strlen(target_filepath) > 0)
    {
        //printf("read file: %s\n", target_filepath);
        return strdup(target_filepath);
    }
    else
    {
        printf("file not found.\n");
        return NULL;
    }
}



void Create_LUT::Process_All_LUT_Files()
{
    this->Process_Log_Files();
}


void Create_LUT::Process_DC_LUT(LUT_TYPE type, char* data_in_filepath, const char* LUT_filepath)
{
    // parse file based on type
    // perform filtering
    // do averaging to LUT points
    // write new file
    printf("read file: %s\n", data_in_filepath);
    printf("target LUT: %s\n\n", LUT_filepath);
    bool file_exists = FileH::File_Exists(data_in_filepath);
    if(!file_exists)
    {
        printf(COLOR_RED "Error: Log filepath  '%s' does not exist.\n" COLOR_RESET, data_in_filepath);
        printf(COLOR_RED "Cannot process this log file.\n" COLOR_RESET);
        return;
    }

    DC_Data* data = new DC_Data(type);
    data->Scan_Data(data_in_filepath);
    data->Filter_Data();

    DC_LUT* Lookup_table = new DC_LUT();
    Lookup_table->LUT_type = type;
    Lookup_table->Add_LUT_Data(data);
    Lookup_table->Print_Groups();
    Lookup_table->Write_to_File(LUT_filepath);

    delete Lookup_table;
    delete data;

    // Move file that has had data extracted
    char dut_processed_dir[200];
    StringH::Erase_Num_Chars(dut_processed_dir,200);
    strcpy(dut_processed_dir, ROOT_PROCESSED_DIR);
    strcat(dut_processed_dir, this->DUT_mac_addr);
    strcat(dut_processed_dir, "/");

    if( !FileH::Dir_Exists( dut_processed_dir ) )         // if the directory doesnt exist already, create it
    {
        printf("creating  directory: '%s'\n", dut_processed_dir);
        mkdir( dut_processed_dir, 0777);
    }
    
    // process executing this program is root. Hence created files are owned by root
    // Here changing ownership and permissions to user pi to avoid permission denied errors if manually editing or moving the files
    FileH::Change_File_Owner_to_User_Pi(data_in_filepath);
    FileH::Change_Permissions_all_RW(data_in_filepath);
    FileH::Move_File(data_in_filepath, dut_processed_dir);  // move the log file to completed directory now that we're done reading the data


    printf(COLOR_LIGHT_GREEN);
    FileH::Print_File(LUT_filepath);
    printf(COLOR_RESET);
}


void Create_LUT::Process_DAC_LUT(LUT_TYPE type, char* data_in_filepath, const char* LUT_filepath, bool move_file)
{

    // parse file based on type
    // perform filtering
    // do averaging to LUT points
    // write new file
    printf("read file: %s\n", data_in_filepath);
    printf("target LUT: %s\n\n", LUT_filepath);
    bool file_exists = FileH::File_Exists(data_in_filepath);
    if(!file_exists)
    {
        printf(COLOR_RED "Error: Log filepath  '%s' does not exist.\n" COLOR_RESET, data_in_filepath);
        printf(COLOR_RED "Cannot process this log file.\n" COLOR_RESET);
        return;
    }

    DC_Data* data = new DC_Data(type);

    data->Scan_DAC_Data(data_in_filepath, type);
    data->Filter_Data();    

    DC_LUT* Lookup_table = new DC_LUT(type);
    Lookup_table->Add_DAC_LUT_Data(data, type);
    Lookup_table->Print_Groups();

    if(type == DAC_COUNT_t)
        Lookup_table->Write_DAC_LUT_to_File(LUT_filepath);
    else
        Lookup_table->Write_to_File(LUT_filepath);

    delete Lookup_table;
    delete data;


    // Move file that has had data extracted
    char dut_processed_dir[200];
    StringH::Erase_Num_Chars(dut_processed_dir,200);
    strcpy(dut_processed_dir, ROOT_PROCESSED_DIR);
    strcat(dut_processed_dir, this->DUT_mac_addr);
    strcat(dut_processed_dir, "/");

    if( !FileH::Dir_Exists( dut_processed_dir ) )         // if the directory doesnt exist already, create it
    {
        printf("creating  directory: '%s'\n", dut_processed_dir);
        mkdir( dut_processed_dir, 0777);
    }
    
    // process executing this program is root. Hence created files are owned by root
    // Here changing ownership and permissions to user pi to avoid permission denied errors if manually editing or moving the files
    FileH::Change_File_Owner_to_User_Pi(data_in_filepath);
    FileH::Change_Permissions_all_RW(data_in_filepath);
    if( move_file == true)
    {
        FileH::Move_File(data_in_filepath, dut_processed_dir);  // move the log file to completed directory now that we're done reading the data  
    }

    printf("CREATED LOCAL FILE: \n");
    printf(COLOR_LIGHT_GREEN);
    FileH::Print_File(LUT_filepath);
    printf(COLOR_RESET);
}

// this function only applies to data taken while driving a "live cooler". The power applied will be AC, not DC
// multiple mA current readings per second from the meter, per single mA PI value
// this function only used to process mA current readings
void Create_LUT::Process_AC_LUT(LUT_TYPE type, char* data_in_filepath, const char* LUT_filepath)
{
    printf("read file: %s\n", data_in_filepath);
    printf("target LUT: %s\n\n", LUT_filepath);
    bool file_exists = FileH::File_Exists(data_in_filepath);
    if(!file_exists)
    {
        printf(COLOR_RED "Error: Log filepath  '%s' does not exist.\n" COLOR_RESET, data_in_filepath);
        printf(COLOR_RED "Cannot process this log file.\n" COLOR_RESET);
        return;
    }

    LC_Data* data = new LC_Data();
    data->Scan_Data(data_in_filepath);
    data->Trim_Outlier_mA_Data();
    LC_LUT* Lookup_table = new LC_LUT();
    Lookup_table->Add_LUT_Data(data);
    Lookup_table->Print_Groups();

    Lookup_table->Write_to_File(LUT_filepath);

    delete Lookup_table;
    delete data;

    // Move log file that has had data extracted
    char dut_processed_dir[200];
    StringH::Erase_Num_Chars(dut_processed_dir,200);
    strcpy(dut_processed_dir, ROOT_PROCESSED_DIR);
    strcat(dut_processed_dir, this->DUT_mac_addr);
    strcat(dut_processed_dir, "/");

    if( !FileH::Dir_Exists( dut_processed_dir ) )         // if the local directory doesnt exist already, create it
    {
        printf("creating  directory: '%s'\n", dut_processed_dir);
        mkdir( dut_processed_dir, 0777);
    }
    
    // process executing this program is root. Hence created files are owned by root
    // Here changing ownership and permissions to user pi to avoid permission denied errors if manually editing or moving the files
    FileH::Change_File_Owner_to_User_Pi(data_in_filepath);
    FileH::Change_Permissions_all_RW(data_in_filepath);
    FileH::Move_File(data_in_filepath, dut_processed_dir);  // move the log file to completed directory now that we're done reading the data

    printf(COLOR_LIGHT_GREEN);
    FileH::Print_File(LUT_filepath);
    printf(COLOR_RESET);
}