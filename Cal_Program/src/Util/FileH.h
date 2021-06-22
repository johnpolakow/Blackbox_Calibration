

#ifndef _FILE_H
#define _FILE_H


#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <errno.h>
#include <dirent.h>
#include <time.h>
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
#include <fstream>
#include <iostream>
//#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


#include "colors.h"
#include "String_Tokenizer.h"
#include "Cal_Parameters.h"

using namespace std;

// Data Structure class to hold measurements read from Pi or meters
class FileH
{
public:
        FileH();
        ~FileH();

        static bool File_Exists(const char* filename);
        static FILE* Open_Existing_File(const char* filepath);
        static FILE* Create_New_File(const char* filepath);
        static void Create_Dir(const char* dir_path);
        static void Close_File(FILE* finished_file);
        static void Move_File(const char* old_filepath, const char* new_dir);
        static bool Dir_Exists(const char* dir_path);
        static void Change_Permissions_all_RW(const char* filepath);
        static void Change_File_Owner_to_User_Pi(const char* filepath);

        static char* Extract_Filename(const char* filepath);
        static char* Extract_Filename_and_Extension(const char* filepath);
        static void Print_File(const char* filepath);
        static char* Get_Filepath(const char* directory, char* filename);
        static bool Is_String_File_Title(char* line);
        
        static char* Extract_Parent_Dir(const char* filepath);
        static long Get_File_Size(std::string filename);

        static char** List_Folders(const char* path);
        static char** List_Files(const char* path);
        static char** Get_Files_in_Dir(const char* path);
        static char* Get_Time();
        static char* Get_Date();

        static char* Get_New_Filtered_Filename(char* original_filepath);
        static void Read_File_Header(FILE* read_file, char* MAC);
        static char* Get_File_Title(FILE* read_file);
        static char* Parse_Title(char* title_line);

        static char* Get_LUT_File_MAC_Addr(const char* filepath);     // parse the MAC address from the first line of a calibration look up table file
        static char* Get_Local_LUT_Dir_for_MAC_Address(const char* MAC_address);

        static char** List_Files_Full_Path(const char* path);       // returns absolute path for every file
        static bool Is_Directory(const char* path);
        static char* Extract_Directory_Name(const char* dirpath);
        static char* Concat_Path(const char* dirpath, const char* filename);

};

#endif
