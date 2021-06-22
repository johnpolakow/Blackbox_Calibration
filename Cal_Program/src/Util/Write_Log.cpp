
#include "Write_Log.h"

/*
const char* LOAD_MA_FILE = "LOAD_mA_DATA.log";
const char* LOAD_V_FILE  = "LOAD_V_DATA.log";
const char* DIODE_V_FILE = "DIODE_V_DATA.log";
const char* REF100_FILE  = "REF100_DATA.log";
const char* COOLER_V_FILE  = "COOLER_V_DATA.log";
const char* COOLER_MA_FILE = "COOLER_mA_DATA.log";
*/
const char* LOAD_MA_FILE = "LOAD_mA.log";
const char* LOAD_V_FILE  = "LOAD_V.log";
const char* DIODE_V_FILE = "DIODE_V.log";
const char* REF100_FILE  = "REF100_DATA.log";
const char* COOLER_V_FILE  = "COOLER_V.log";
const char* COOLER_MA_FILE = "COOLER_mA.log";
const char* THERMOCOUPLE_FILE = "THERMOCOUPLE.log";


char* Write_Log::Get_Date()
{
    char* buffer = (char*)malloc(sizeof(char) * 22);
	 time_t rawtime;
	 struct tm* timeinfo;
	 time(&rawtime);
	 timeinfo = localtime(&rawtime);
	 strftime (buffer,30,"%m-%d-%y",timeinfo);

	 return buffer;
}

char* Write_Log::Get_Time()
{
   char* buffer = (char*)malloc(sizeof(char) * 22);
	 time_t rawtime;
	 struct tm* timeinfo;
	 time(&rawtime);
	 timeinfo = localtime(&rawtime);
     buffer[0] = '\0';
	 strftime (buffer,22,"%H:%M:%S  ",timeinfo);
	 buffer[strlen(buffer)-1] = '\0';
	 return buffer;
}


bool Write_Log::File_Exists(const char* filename)
{
    FILE *file;
    if ((file = fopen(filename, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}


bool Write_Log::Dir_Exists(const char* dir_path)
{
    DIR* dir = opendir(dir_path);
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else if (ENOENT == errno)
    {
        return false;
    }
    else
    {
        return false;
    }
}


FILE* Write_Log::Open_File(char* filepath)
{
    FILE* log_file_ptr;
    if( Write_Log::File_Exists(filepath) )
    {
        log_file_ptr = fopen(filepath, "a"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if(log_file_ptr == NULL)
        {
            printf("error opening Log File:  %s\n", filepath);
            perror("opening existing Log File");
            return NULL;
        }
    }
    else
    {
        // File doesnt exist yet, must be created
        log_file_ptr = fopen(filepath, "w"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if( log_file_ptr == NULL )
        {
            printf("creation error on new Log file:   %s\n", filepath);
            perror("open new Log error");
            return NULL;
        }
    }
    return log_file_ptr;
}


void Write_Log::Create_DIR(char* dir_path)
{
    if( !Write_Log::Dir_Exists( dir_path) )
    {
        mkdir( dir_path, 0777);  // 0777 is the linux permissions for the directory
    }
    return;
}

char* Write_Log::Get_Log_Dir()
{
    return strdup(this->Log_Dir);
}

char* Write_Log::Get_MAC_Addr()
{
    return strdup(this->MAC_Address);
}

void Write_Log::Write_Load_mA_Data(int DAC_count, Data_Group* pi_mA, Data_Group* meter_mA)
{
    StringH::Erase_Num_Chars(this->Log_Filepath, FILEPATH_LEN);
    StringH::Erase_Num_Chars(this->Log_Filename, FILENAME_LEN);

    strcpy(this->Log_Filename, LOAD_MA_FILE);
    sprintf(this->Log_Filepath, "%s/%s", Log_Dir, LOAD_MA_FILE);

    if( File_Exists(this->Log_Filepath) )
    {
        // open file for appending, the file has already had data written to it
        this->log_file_ptr = fopen(this->Log_Filepath, "a"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if(log_file_ptr == NULL)
        {
            printf("error opening existing File:  %s\n", this->Log_Filepath);
            perror("opening existing File");
            return;
        }
    }
    else
    {
        // File doesnt exist yet, must be created
        this->log_file_ptr = Write_Log::Initialize_Log_File(this->Log_Filepath, this->Log_Filename, this->MAC_Address);
    }

    // write the current timestamp to file
    char TimeDate_Header[TIMEDATE_LINE_LEN];
    StringH::Erase_Num_Chars(TimeDate_Header, TIMEDATE_LINE_LEN);
    char* time = Get_Time();
    char* date = Get_Date();
    sprintf( TimeDate_Header, "%s %s", time, date);
    fprintf(log_file_ptr , "%s\n", TimeDate_Header);
    free(time);
    free(date);


    fprintf(log_file_ptr , "\t[ DAC count, Pi mA, 34401 mA]\n");

    char write_line[100];

    int i = 0;
    while( i< (pi_mA->num_points) )
    {
        StringH::Erase_Num_Chars(write_line, 100);
        sprintf(write_line, "\t[ %d, %.4f, %.4f ]", DAC_count, pi_mA->data_points[i], meter_mA->data_points[i] );
        fprintf(log_file_ptr , "%s\n", write_line);
        ++i;
    }
    fprintf(log_file_ptr , "\tAVG[ %.4f, %.4f]\n", pi_mA->average, meter_mA->average);
    fprintf(log_file_ptr , "\tSTD DEV[ %.4f, %.4f]\n", pi_mA->std_dev, meter_mA->std_dev);
    fprintf(log_file_ptr , "%s\n", line_hbreak);                     // write separation line: ______________
    fclose( log_file_ptr  );
}

FILE* Write_Log::Initialize_Log_File(char* filepath, char* filename, char* mac_address)
{
    // File doesnt exist yet, must be created
    FILE* file_ptr = fopen(filepath, "w"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
    if( file_ptr == NULL )
    {
        printf("creation error on new file:   %s\n", filepath);
        perror("open new Log error");
        return NULL;
    }

    char Time_Date_Line[TIMEDATE_LINE_LEN];
    char Declaration_Line[FILENAME_LEN];
    char MAC_Addr_Line[NAME_LINE_LEN];
    StringH::Erase_Num_Chars(Declaration_Line, FILENAME_LEN);
    StringH::Erase_Num_Chars(MAC_Addr_Line, NAME_LINE_LEN);
    StringH::Erase_Num_Chars(Time_Date_Line, TIMEDATE_LINE_LEN);
    StringH::Erase_Num_Chars(MAC_Addr_Line, NAME_LINE_LEN);

    char* date = Get_Date();
    char* time = Get_Time();
    sprintf( MAC_Addr_Line, "// Cal log for MAC address:  %s", mac_address);
    sprintf( Time_Date_Line, "// Created:   %s %s", time, date);
    sprintf( Declaration_Line, "\t\t   ## %s ##\n", filename);
    free(date);
    free(time);

    fprintf(file_ptr , "%s\n", MAC_Addr_Line);                      // filename: XXXXXXX.log
    fprintf(file_ptr , "%s\n\n", Time_Date_Line);                   // filename: XXXXXXX.log
    fprintf(file_ptr , "\t\t%s\n\n", Declaration_Line );            // write ""## LOAD mA CAL LOG ## "
    fprintf(file_ptr , "%s\n", line_hbreak);                     // write separation line: ______________

    return file_ptr;
}


void Write_Log::Write_Volt_Data(Data_Group* pi_mA, Data_Group* meter_vals, logfile_t log_type)
{
    StringH::Erase_Num_Chars(this->Log_Filepath, FILEPATH_LEN);
    StringH::Erase_Num_Chars(this->Log_Filename, FILENAME_LEN);

    switch(log_type)
    {
        case logfile_t::Load_V:
            strcpy(this->Log_Filename, LOAD_V_FILE);
            break;
        case logfile_t::Diode_V: 
            strcpy(this->Log_Filename, DIODE_V_FILE);
            break;
        case logfile_t::Cooler_V: 
            strcpy(this->Log_Filename, COOLER_V_FILE);
            break;
    }


    sprintf(this->Log_Filepath, "%s/%s", Log_Dir, this->Log_Filename);

    if( File_Exists(this->Log_Filepath) )
    {
        // open file for appending, the file has already had data written to it
        this->log_file_ptr = fopen(this->Log_Filepath, "a"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if(log_file_ptr == NULL)
        {
            printf("error opening existing File:  %s\n", this->Log_Filepath);
            perror("opening existing File");
            return;
        }
    }
    else
    {
        // File doesnt exist yet, must be created
        this->log_file_ptr = Write_Log::Initialize_Log_File(this->Log_Filepath, this->Log_Filename, this->MAC_Address);
    }

    // write the current timestamp to file
    char TimeDate_Header[TIMEDATE_LINE_LEN];
    StringH::Erase_Num_Chars(TimeDate_Header, TIMEDATE_LINE_LEN);
    char* time = Get_Time();
    char* date = Get_Date();
    sprintf( TimeDate_Header, "%s %s", time, date);
    fprintf(log_file_ptr , "%s\n", TimeDate_Header);
    free(time);
    free(date);


    fprintf(log_file_ptr , "\t[ Pi meas V, 34401 meas V]\n");
    char write_line[100];

    int i = 0;
    while( i< (pi_mA->num_points) )
    {
        StringH::Erase_Num_Chars(write_line, 100);
        sprintf(write_line, "\t[ %.6f, %.6f ]", pi_mA->data_points[i], meter_vals->data_points[i] );
        fprintf(log_file_ptr , "%s\n", write_line);
        ++i;
    }
    fprintf(log_file_ptr , "\tAVG[ %.6f, %.6f]\n", pi_mA->average, meter_vals->average);
    fprintf(log_file_ptr , "\tSTD DEV[ %.6f, %.6f]\n", pi_mA->std_dev, meter_vals->std_dev);
    fprintf(log_file_ptr, "---------------------------------------------------------------------------\n\n");      
    fclose(log_file_ptr  );
}


void Write_Log::Write_Power_Meter_mA_Data(LUT_Match** matches, int num_matches)
{

    StringH::Erase_Num_Chars(this->Log_Filepath, FILEPATH_LEN);
    StringH::Erase_Num_Chars(this->Log_Filename, FILENAME_LEN);


    sprintf(this->Log_Filename, "%s", COOLER_MA_FILE);
    sprintf(this->Log_Filepath, "%s/%s", Log_Dir, this->Log_Filename);

    if( !Write_Log::File_Exists(this->Log_Filepath) ) // if file doesnt exist already, create a new file, and add header to it
    {
        this->log_file_ptr = Write_Log::Initialize_Log_File(this->Log_Filepath, this->Log_Filename, this->MAC_Address);
    }
    else
    {
        this->log_file_ptr = Write_Log::Open_File(this->Log_Filepath);
    }

    // Write a header legend for the Data
    char Date_Header[DATE_HEADER_LEN];
    StringH::Erase_Num_Chars(Date_Header, DATE_HEADER_LEN);
    char* date = Write_Log::Get_Date();
    strcpy(Date_Header, date);
    free(date);
    fprintf(this->log_file_ptr , "%s\n", Date_Header);

    char* header_string = LUT_Match::toString_Header();
    fprintf(this->log_file_ptr , "%s\n", header_string);
    free(header_string);

    char write_line[WRITE_LINE_LEN];
    for(int i = 0; i< num_matches; ++i)
    {
        LUT_Match* match = matches[i];
        char* match_string = LUT_Match::toString(match);
        StringH::Erase_Num_Chars(write_line, WRITE_LINE_LEN);
        strcpy(write_line, match_string);
        free(match_string);
        fprintf(this->log_file_ptr, "%s\n", write_line);
    }

    fprintf( this->log_file_ptr, "---------------------------------------------------------------------------\n\n");      
    Write_Log::Close_File( this->log_file_ptr );
}

void Write_Log::Write_REF100_Data(Data_Group* uA_data)
{
    StringH::Erase_Num_Chars(this->Log_Filepath, FILEPATH_LEN);
    StringH::Erase_Num_Chars(this->Log_Filename, FILENAME_LEN);

    sprintf(this->Log_Filename, "%s", REF100_FILE);
    sprintf(this->Log_Filepath, "%s/%s", Log_Dir, this->Log_Filename);

    if( !Write_Log::File_Exists(this->Log_Filepath) ) // if file doesnt exist already, create a new file, and add header to it
    {
        this->log_file_ptr = Write_Log::Initialize_Log_File(this->Log_Filepath, this->Log_Filename, this->MAC_Address);
    }
    else
    {
        this->log_file_ptr = Write_Log::Open_File(this->Log_Filepath);
    }

    char TimeDate_Header[100];
    StringH::Erase_Num_Chars(TimeDate_Header, 100);
    char* time = Get_Time();
    char* date = Get_Date();
    strcpy(TimeDate_Header, time);
    strcat(TimeDate_Header, " ");
    strcat(TimeDate_Header, date);

    fprintf(log_file_ptr , "%s\n", TimeDate_Header);
    fprintf(log_file_ptr , "\t[ HP meas. uA ]\n");

    char write_line[100];

    int i = 0;
    while( i< uA_data->num_points )
    {
        StringH::Erase_Num_Chars(write_line, 100);
        sprintf(write_line, "\t[ %.4f ]", uA_data->data_points[i] );
        fprintf(log_file_ptr , "%s\n", write_line);
        ++i;
    }
    fprintf( log_file_ptr , "\tAVG[ %.4f ]\n", uA_data->average );
    fprintf( log_file_ptr , "\tSTD DEV[ %.4f ]\n", uA_data->std_dev );
    fprintf( log_file_ptr , "%s\n", line_hbreak);           // write separation line: ______________
    fclose( log_file_ptr  );
}



void Write_Log::Write_Thermocouple_Data(Thermocouple_Data* therm_data)
{
    StringH::Erase_Num_Chars(this->Log_Filepath, FILEPATH_LEN);
    StringH::Erase_Num_Chars(this->Log_Filename, FILENAME_LEN);

    sprintf(this->Log_Filename, "%s", THERMOCOUPLE_FILE);
    sprintf(this->Log_Filepath, "%s/%s", Log_Dir, this->Log_Filename);

    if( !Write_Log::File_Exists(this->Log_Filepath) ) // if file doesnt exist already, create a new file, and add header to it
    {
        this->log_file_ptr = Write_Log::Initialize_Log_File(this->Log_Filepath, this->Log_Filename, this->MAC_Address);
    }
    else
    {
        this->log_file_ptr = Write_Log::Open_File(this->Log_Filepath);
    }

    char TimeDate_Header[100];
    StringH::Erase_Num_Chars(TimeDate_Header, 100);
    char* time = Get_Time();
    char* date = Get_Date();
    strcpy(TimeDate_Header, time);
    strcat(TimeDate_Header, " ");
    strcat(TimeDate_Header, date);

    fprintf(log_file_ptr , "%s\n", TimeDate_Header);
    fprintf(log_file_ptr , "\tTHRMCPL1 \tTHRMCPL2 \tTHRMCPL3\n");

    char write_line[100];

    int vector_size = therm_data->data.size();

    float sum1 = 0;
    float sum2 = 0;
    float sum3 = 0;
    for(int i = 0; i< vector_size; ++i)
    {
        float therm1, therm2, therm3;
        therm1 = therm_data->data[i]->thermocouple1_tempC;
        therm2 = therm_data->data[i]->thermocouple2_tempC;
        therm3 = therm_data->data[i]->thermocouple3_tempC;
        sum1 += therm1;
        sum2 += therm2;
        sum3 += therm3;
        sprintf(write_line, "\t%.2f\t\t%.2f\t\t%.2f", therm1, therm2, therm3);
        fprintf(log_file_ptr , "%s\n", write_line);
    }
    float avg1 = sum1/vector_size;
    float avg2 = sum2/vector_size;
    float avg3 = sum3/vector_size;
    sprintf(write_line, "\n   AVG:\t%.2f\t\t%.2f\t\t%.2f  \u00B0C\n", avg1, avg2, avg3);
    fprintf(log_file_ptr , "%s", write_line);

    sprintf(write_line, " STDEV:  %.2f\t\t %.2f\t\t %.2f  \u00B0C\n", therm_data->stdev1, therm_data->stdev2, therm_data->stdev3);
    fprintf(log_file_ptr , "%s", write_line);


    fprintf( log_file_ptr , "%s\n", line_hbreak);           // write separation line: ______________
    fclose( log_file_ptr  );
}


void Write_Log::Close_File(FILE* finished_file)
{
    fflush(finished_file);
    fclose(finished_file);
}
