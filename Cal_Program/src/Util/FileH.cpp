
#include "FileH.h"





void FileH::Create_Dir(const char* dir_path)  
{
    struct stat st = {0};
    int dir_exists = stat(dir_path, &st);

    if( dir_exists == -1)
    {
        mkdir(dir_path, 0777);
    }
}



bool FileH::File_Exists(const char* filename)
{
    FILE *file;
    if ((file = fopen(filename, "r")))
    {
        fclose(file);
        return true;
    }
    return false;
}


FILE* FileH::Open_Existing_File(const char* filepath)
{
    FILE* data_file_ptr;
    if( FileH::File_Exists(filepath) )
    {
        data_file_ptr = fopen(filepath, "r"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
        if(data_file_ptr == NULL)
        {
            printf("error opening Log File:  %s\n", filepath);
            perror("opening existing Log File");
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
    return data_file_ptr;
}

FILE* FileH::Create_New_File(const char* filepath)
{
    FILE* data_file_ptr;

    // File doesnt exist yet, must be created
    data_file_ptr = fopen(filepath, "w"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
    if( data_file_ptr == NULL )
    {
        printf("creation error on new data file:   %s\n", filepath);
        perror("open new data file error");
        return NULL;
    }

    return data_file_ptr;
}

void FileH::Change_File_Owner_to_User_Pi(const char* filepath)
{
  uid_t          uid;
  gid_t          gid;
  struct passwd *pwd;
  struct group  *grp;

    pwd = getpwnam("pi");
    if (pwd == NULL) {
        printf("Failed to get uid");
    }
    uid = pwd->pw_uid;

    grp = getgrnam("pi");
    if (grp == NULL) {
        printf("Failed to get gid");
    }
    gid = grp->gr_gid;


    if (chown(filepath, uid, gid) == -1) {
        printf("chown fail");
    }
}


void FileH::Change_Permissions_all_RW(const char* filepath)
{
    // gives read and write permissions to users groups and others
    mode_t filemode = ( S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR | S_IWGRP | S_IXOTH);
    chmod(filepath, filemode);
}



void FileH::Close_File(FILE* finished_file)
{
    fflush(finished_file);
    fclose(finished_file);
}

// Example LUT Header
/*
// LUT for Pi w/ MAC Address: B8.27.EB.2B.FB.70
#ifndef LOAD_mA_LUT_H
#define LOAD_mA_LUT_H

#define PI_LOAD_mA_LUT          0   // index in the array
#define METER_LOAD_mA_LUT       1   // index in the array
#define NUM_LOAD_mA_LUT_ELEMENTS 113

//  [pi meas. mA][ meter mA ]
float LOAD_mA_LUT[113][2] = {
    ...
*/
char* FileH::Get_Local_LUT_Dir_for_MAC_Address(const char* MAC_address)
{
    // list directories in LUT dir
    //ROOT_LUT_DIR
    char* MAC_addr = strdup(MAC_address);
    StringH::Convert_Character(MAC_addr, ':', '.'); // change colons to periods
    printf("\nLocal LUT calibration folders:\n");
    char** dir_list = FileH::List_Folders(ROOT_LUT_DIR);

    if(dir_list == NULL)
        return NULL;

    //printf("dir list:\n");
    //StringH::Print_String_Array(dir_list);

    int index = 0;
    char* dir_name = NULL;
    char* dir_path = NULL;
    while( dir_list[index] != NULL)
    {
        dir_path = strdup( dir_list[index] );
        if( !FileH::Is_Directory( dir_path) )
        {
            printf("%s is not a directory\n", dir_path);
            free(dir_path);
            dir_path = NULL;
            ++index;
            continue;
        }
        
        dir_name = FileH::Extract_Directory_Name( dir_path );    // the name of each directory is a MAC address
        if( StringH::Strings_Match(MAC_addr, dir_name) )
        {
            //printf("dir name matches target MAC\n");
            free(dir_name);                             // free memory for just the "name" of the directory, we want the whole path
            dir_name = NULL;
            break;
        }
        else
        {
            //printf("strings do not match:\t %s \t %s\n", dir_name, MAC_addr);
            free(dir_name);
            dir_name = NULL;
            free(dir_path);
            dir_path = NULL;
            ++index;
            continue;
        }

    }

    StringH::Free_String_Array_Memory(dir_list);
    free(MAC_addr);
    return dir_path;
}

bool FileH::Is_Directory(const char* path)
{
    struct stat s;
    if( stat(path,&s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            return true;
        }
        else if( s.st_mode & S_IFREG )
        {
            return false;   // is a regular file
        }
        else
        {
            return false;
        }
    }
    return false;
}

char* FileH::Get_LUT_File_MAC_Addr(const char* filepath)
{
    if( !FileH::File_Exists(filepath))
        return NULL;
    
    FILE* lut_file_ptr = FileH::Open_Existing_File(filepath);

    char readline[512];
    int line_number = 0;
    bool is_lut_file = false;
    char* mac_address = NULL;

    // looking for one specific line :      LUT for Pi w/ MAC Address: B8.27.EB.2B.FB.70
    while( (fgets(readline, 512, lut_file_ptr) != NULL) && line_number < 3)      // LUT file header w/ MAC Addr should be in first few lines of file
    {
        ++line_number;

        bool line_is_whitespace = StringH::Line_Is_Whitespace(readline);
        bool line_contains_LUT_str = StringH::String_Contains(readline, "LUT");
        if( !line_is_whitespace && !line_contains_LUT_str)      // every line in the header of a Calibration file should contain the string "LUT", or be whitespace
        {
            FileH::Close_File(lut_file_ptr);    
            return NULL;
        }
      
        bool line_contains_pi_str = StringH::String_Contains(readline, "LUT for Pi w/ MAC");

        if( line_contains_pi_str )
        {
            mac_address = StringH::Parse_MAC_Addr(readline);
        }

        if( mac_address != NULL )
        {
            FileH::Close_File(lut_file_ptr);   
            return mac_address;
        }
    }

    FileH::Close_File(lut_file_ptr);    
    return NULL;
}



long FileH::Get_File_Size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}



char* FileH::Concat_Path(const char* dirpath, const char* filename)
{
    if(dirpath == NULL || filename == NULL)
        return NULL;

    int pathlen = strlen(dirpath) + strlen(filename) + 2;

    char* dir = strdup(dirpath);    // make copy of input dir path so we're not modifying the input string
    StringH::Trim_WhiteSpace(dir);
    char last_char = dir[ strlen(dir) - 1];     // get last character of dir path, to see if we need to add in a path separator

    char filepath[ pathlen ];   // allocate memory for new string
    strcpy(filepath, dir);      // copy in dirpath
    
    if( !(last_char == '/') )
    {
        strcat(filepath, "/");  // append path separator if necessary
    }

    strcat(filepath, filename); // end with filename
    return strdup(filepath);
}


/*
|--------------------------------------------------------------------------
| List_Folders
|--------------------------------------------------------------------------
|
| List all regular folders in the directory given in the argument path.
| Hidden folders are not included. (folders beginning with a '.')
|
| @param:     path         filepath to directory whose folders are listed
| @return:                 a pointer to a list of strings, each string is a fully qualified path in the directory
*/
char** FileH::List_Folders(const char* path)
{
    unsigned char filetype;
    int path_len = strlen(path);
    struct dirent *directory_entry;   // Pointer for directory entry
    DIR *directory = opendir(path);   // opendir() returns a pointer of DIR type.

    if (directory == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open directory: %s\n", path);
        return NULL;
    }

    //printf("\nLIST FOLDERS:\n");
    printf("------------------------------------------------------------------------------\n");
    printf("ls %s\n", path);
    int num_folders = 0;
    while ((directory_entry = readdir(directory)) != NULL)
    {
        filetype = directory_entry->d_type;
        if(filetype == DT_DIR)
        {
            if(directory_entry->d_name[0] != '.' && directory_entry->d_name[1] != '.')
            {
                printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, directory_entry->d_name);
                ++num_folders;
            }
        }
    }
    printf("------------------------------------------------------------------------------\n");
    closedir(directory);

    if( num_folders > 0 )
    {
        char** folder_list = (char**)malloc( sizeof(char*)*(num_folders +1) );
        for(int i = 0; i<num_folders+1; ++i)
            folder_list[i] = NULL;

        directory = opendir(path);
        int folder_index = 0;
        while ((directory_entry = readdir(directory)) != NULL)
        {
            filetype = directory_entry->d_type;
            if(filetype == DT_DIR)
            {
                if(directory_entry->d_name[0] != '.' && directory_entry->d_name[1] != '.')
                {
                    int mem_alloc_size = strlen(directory_entry->d_name) + strlen(path);
                    char* dir_path = (char*)malloc( sizeof(char) * (mem_alloc_size+1) );
                    strcpy(dir_path, path);
                    strcat(dir_path, directory_entry->d_name);
                    dir_path[mem_alloc_size] = '\0';
                    folder_list[folder_index] = dir_path;
                    folder_list[folder_index+1] = NULL;
                    ++folder_index;
                }
            }
        }
        closedir(directory);
        return folder_list;
    }
    else
    {
        return NULL;
    }
}

char* FileH::Get_Filepath(const char* directory, char* filename)
{
    // First allocate memory for the concatenated string:
    int memory_size = ( strlen(directory) + strlen("/") + strlen(filename) + 1);
    char* temp = (char*)malloc( sizeof(char)*  memory_size);
    StringH::Erase_Num_Chars(temp, memory_size);
    strcpy(temp, directory);

    // Get last character to determine if '/' was used at end of dir path
    StringH::Trim_WhiteSpace(temp);
    char last;
    for(int i =0; i<memory_size; ++i)
    {
        if(temp[i] != '\0')
            last = temp[i];
        else if( temp[i] == '\0')
            break;
    }

    if(last == '/') // no need to add an extra '/'
    {
        strcat(temp, filename);
    }
    else
    {
        strcat(temp, "/");
        strcat(temp, filename);
    }
    return temp;
}

// this function takes as an argument something of the form: /home/pi/test  or /home/pi/test/  
// and returns a string of the filename: test
// the return val is the argument
// client is responsible for freeing memory used in return string
char* FileH::Extract_Directory_Name(const char* dirpath)
{
    if(dirpath == NULL || !strlen(dirpath)>0)
        return NULL;

    char path_copy[strlen(dirpath)+1];
    strcpy(path_copy, dirpath); 
    StringH::Trim_WhiteSpace(path_copy); // remove leading and trailing whitespace
    int path_end = strlen(path_copy);
    
    int index_last_delim = StringH::Get_Last_Index_Of(path_copy, '/');  // find index of last path delimiter '/'
    if(path_copy[index_last_delim+1] == '\0')   // if the function argument is a directory, not a file: /home/pi/test/dir1/
    {
        path_copy[index_last_delim] = '\0'; // if the last character is a delimiter, strip the path delimiter to get directory
        index_last_delim = StringH::Get_Last_Index_Of(path_copy, '/');  // get new index last delim
    }

    if( (path_end - index_last_delim) > 0 && index_last_delim != -1)
    {
        char* dir_name = StringH::Get_Substring(path_copy, index_last_delim+1, path_end);
        return dir_name;
    }
    else
    {
        return NULL;
    }
}

// Assumes the File is already open
// essentially just moving the file pointer through the beginning header which doesnt contain data
// grabbing the MAC address however
void FileH::Read_File_Header(FILE* read_file, char* MAC)
{
    char FILE_BEGIN[] = "───────────────────";
    bool read_mac = false;
    bool read_timestamp = false;
    bool read_title = false;
    bool read_header = false;
    char readline[300];
    int lines_read = 0;
    char* mac_address;

    while( (fgets(readline, 256, read_file) != NULL) && lines_read < 8 && !read_header)
    {
    // loop until past the start of file header, which is after:
    // line: _______________________________________________

    		++lines_read;
            bool contains;
            if(!read_mac)
            {
                contains = StringH::String_Contains_Ignore_Case(readline, "log for MAC address");
                if(contains)
                {
                    mac_address = StringH::Parse_MAC_Addr(readline); //example line: "// Cal log for MAC address:  B8.27.EB.87.3D.76"
                    //printf("readline: %s\n", readline);
                    //printf("parsed MAC address: %s\n\n", mac_address);
                    sprintf(MAC, "%s", mac_address);
                    free(mac_address);
                    read_mac = true;
                    continue;   // read another line
                }
            }
            if(!read_timestamp && StringH::String_Contains(readline, "Created:"))
            {
                //printf("checking if line is creation timestamp\n");
                int index_first_colon = StringH::Index_First_Colon(readline);
                char* timestamp = StringH::Get_Substring(readline, index_first_colon+1, strlen(readline));
                StringH::Trim_WhiteSpace(timestamp);
                printf("log creation: %s\n", timestamp);
                contains = StringH::String_Is_TimeStamp(timestamp);
                free(timestamp);
                if(contains)
                {
                    read_timestamp = true;
                    continue;   // read another line from file
                }
            }
            if(!read_title)
            {
                contains = FileH::Is_String_File_Title(readline);
                if(contains)
                {
                    read_title = true;
                    continue;   // read another line from file
                }
            }
            if(!read_header)
            {
        		contains = strstr(readline, FILE_BEGIN); // checks for line separator: ____________________________________
        		if(contains)
        		{
        			read_header = true;
                    break;  // done with file header
        		}
            }
    }
}

// assumes file is already open
// make sure to close file after done reading the title, so it re-opens at the beginning
char* FileH::Get_File_Title(FILE* read_file)
{
    char FILE_BEGIN[] = "───────────────";
    bool read_mac_addr = false;
    bool read_timestamp = false;
    bool read_title = false;
    bool read_header = false;
    char readline[300];
    char* file_title = NULL;
    int lines_read = 0;

    // set file position pointer at the beginning of the file (to read the title):
    //fseek(read_file, 0, SEEK_SET);

    while( (fgets(readline, 256, read_file) != NULL) && lines_read < 8 && !read_header)
    {
    // loop until past the start of file header, which is after:
    // line: _______________________________________________

    		++lines_read;
            bool contains;
            if(!read_mac_addr)
            {
        		contains = StringH::String_Contains_Ignore_Case(readline, "log for MAC address");
                if(contains)
                {
                    //printf("read MAC address\n");
                    read_mac_addr = true;
                    continue;   // read another line
                }
            }
            else if(!read_timestamp && StringH::String_Contains(readline, "Created:"))
            {
                int index_first_colon = StringH::Index_First_Colon(readline);
                char* timestamp = StringH::Get_Substring(readline, index_first_colon+1, strlen(readline));
                contains = StringH::String_Is_TimeStamp(timestamp);
                free(timestamp);
                if(contains)
                {
                    read_timestamp = true;
                    continue;   // read another line from file
                }
            }
            else if(!read_title)
            {
                contains = FileH::Is_String_File_Title(readline);
                if(contains)
                {
                    read_title = true;
                    file_title = FileH::Parse_Title(readline);
                    return file_title;
                }
            }
            if(!read_header)
            {
        		contains = strstr(readline, FILE_BEGIN); // checks for line separator: ____________________________________
        		if(contains)
        		{
                    // this means we have read past the title line, return NULL
                    return NULL;
        		}
            }
    }  
    return NULL;
}


//     Example of Header of File to parse title from:
/*
// Cal log for MAC address:  B8.27.EB.87.3D.76
15:46:30   11-25-20

                            ## Power Meter mA CAL Data ##


─────────────────── ──────────────────────── ───────────────────────── ──────────
*/
char* FileH::Parse_Title(char* title_line)
{
    int index_first_alpha = -1;
    int index_last_alpha = 0;
    char* parsed_title;

    if( !StringH::String_Contains(title_line, "##") )
    {
        return NULL;
    }
    else
    {   
        // parse out the string that consists of only alphabet characters: [a-z][A-Z]
        // for above example this would be: "Power Meter mA CAL Data"
        char cur_char;
        // get index of first and last valid alpha character ( a letter: [a-z][A-Z])
        for(int i = 0; i< strlen(title_line); ++i)
        {
            cur_char = title_line[i];
            if( (cur_char >= 'a' && cur_char <= 'z') || (cur_char >= 'A' && cur_char <= 'Z') || (cur_char == ' ') )
            {
                if( index_first_alpha == -1 )
                    index_first_alpha = i;
                else
                    index_last_alpha = i;
            }
        }

        // in the string, if there is '.' or '_' character,  convert to space
        for(int i =0; i<strlen(title_line); ++i)
        {
            cur_char = title_line[i];
            if( cur_char == '.')
                title_line[i] = ' ';
        }

        for(int i =0; i<strlen(title_line); ++i)
        {
            cur_char = title_line[i];
            if( cur_char == '_')
                title_line[i] = ' ';
        }

        char* title = StringH::Get_Substring(title_line, index_first_alpha, index_last_alpha);
        parsed_title = strdup(title);
        StringH::Erase_Num_Chars(parsed_title, strlen(parsed_title) );
        parsed_title[0] = ' ';  // variable to hold string for valid description tokens

        Tokenizer token(title);
        char* current_token;

        // tokenize title line, adding only valid descriptor words to the parsed title
        while( (current_token = token.Get_Next_Token() ) != NULL )
        {
            // remove words CAL and Data and LOG
            if( StringH::String_Contains_Ignore_Case(current_token, "CAL") || StringH::String_Contains_Ignore_Case(current_token, "Data") || StringH::String_Contains_Ignore_Case(current_token, "LOG") )
                continue;
            else
            {
                strcat(parsed_title, current_token);
                strcat(parsed_title, " ");
            }
        }
        free(title);
    }
    return parsed_title;
}

char* FileH::Extract_Filename(const char* filepath)
{
    int index_last_slash = -1;
    int index = 0;

    while(filepath[index] != '\0')
    {
        if(filepath[index] == '/')
            index_last_slash = index;
        ++index;
    }

    if(index_last_slash > 0)
    {
        char* path = strdup(filepath);
        char* filename = StringH::Get_Substring(path, index_last_slash+1, strlen(filepath) );
        free(path);
        int index_file_extension = StringH::Index_First_Period(filename);
        filename[index_file_extension] = '\0';  // truncates the file extension
        return filename;
    }
    else
    {
        return strdup(filepath);
    }
}

FileH::FileH(){}
FileH::~FileH(){}

char* FileH::Extract_Filename_and_Extension(const char* filepath)
{
    int index_last_slash = -1;
    int index = 0;

    while(filepath[index] != '\0')
    {
        if(filepath[index] == '/')
            index_last_slash = index;
        ++index;
    }

    if(index_last_slash > 0)
    {
        char* path = strdup(filepath);
        char* filename = StringH::Get_Substring(path, index_last_slash+1, strlen(filepath) );
        free(path);
        //printf("filename&extension: %s\n", filename);
        return filename;
    }
    else
    {
        return strdup(filepath);
    }
}

void FileH::Move_File(const char* old_filepath, const char* new_dir)
{
    // Move file 
    char* old_file_name = FileH::Extract_Filename_and_Extension(old_filepath);
    int len = strlen(old_file_name) + strlen(new_dir) + 5;
    char new_filepath[len];
    strcpy(new_filepath, new_dir);
    strcat(new_filepath, old_file_name);

    if( !FileH::Dir_Exists( new_dir ) )         // if the directory doesnt exist already, create it
    {
        printf("creating  directory: '%s'\n", new_dir);
        mkdir( new_dir, 0777);
    }
    rename(old_filepath, new_filepath);
}


bool FileH::Dir_Exists(const char* dir_path)
{
    struct stat info;

    if(stat( dir_path, &info ) != 0)
        return false;
    else if(info.st_mode & S_IFDIR)
        return true;
    else
        return false;
}


// this function takes as an argument something of the form: /home/pi/test/dir1/dir2  OR /home/pi/test/dir1/document.txt
// and returns a string of the parent directory: /home/pi/test/dir1
// if an argument is of the form /home/pi/test/dir1/dir2/, it is assumed the user wants the parent directory when inside dir2, which would be /home/pi/test/dir1/dir2
// if there are no path delimiters in the argument, NULL is returned
// client is responsible for freeing memory used in return string
char* FileH::Extract_Parent_Dir(const char* filepath)
{
    if(filepath == NULL || !strlen(filepath)>0)
        return NULL;

    char path_copy[strlen(filepath)+1];
    strcpy(path_copy, filepath); 
    StringH::Trim_WhiteSpace(path_copy); // remove leading and trailing whitespace
    
    int index_last_path_delim = 0;  // index of last forward slash in the filepath, start at front of string

    // find index of last path delimiter
    for(int i=0; i<strlen(path_copy); ++ i) 
    {
        if( path_copy[i] == '/')
        {
            index_last_path_delim = i;
            continue;
        }
    }

    if(index_last_path_delim == 0 && path_copy[0] == '/')   // if the function argument is "/" or "/somedir", then return "/"
    {
        path_copy[1] = '\0';    // terminate the string after the slash and return the string
        return strdup(path_copy);
    }
    else if(index_last_path_delim == 0 && path_copy[0] != '/')  // if there are no path separators in the argument
    {
        return NULL;
    }
    else
    {
        for(int i=index_last_path_delim; i<strlen(path_copy); ++ i) 
        {
            path_copy[i] = '\0';   // erase any remaining chars after the last path separator
        }
    }
    return strdup(path_copy);
}

/*
|--------------------------------------------------------------------------
| List_Files
|--------------------------------------------------------------------------
|
| List the files in the directory given by path. Only "regular" files are
| returned. Folders are not returned
|
| @param:   path    the path to the directory whose files are listed
| @return:          a pointer to a list of strings, each string is a file in the directory
*/
char** FileH::List_Files(const char* path)
{
  int num_files = 0;
  unsigned char filetype;
  struct dirent *directory_entry;   // Pointer for directory entry
  DIR *directory = opendir(path);   // opendir() returns a pointer of DIR type.

  if (directory == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open directory: %s\n", path);
      return NULL;
  }

  printf(LINE_SEPARATOR);
  printf("%s\n", path);
  while ((directory_entry = readdir(directory)) != NULL)
  {
      filetype = directory_entry->d_type;
      if(filetype == DT_REG)
      {
            printf("\t%s\n", directory_entry->d_name);
            ++num_files;
      }
  }
  printf(LINE_SEPARATOR);

  free(directory);

  char** file_list = (char**)malloc( sizeof(char*)*(num_files +1) );
  for(int i = 0; i<num_files+1; ++i)
    file_list[i] = NULL;

  int file_index = 0;
  directory = opendir(path);   // opendir() returns a pointer of DIR type.
  while ((directory_entry = readdir(directory)) != NULL)
  {
      filetype = directory_entry->d_type;
      if(filetype == DT_REG)
      {
          char* name = (char*)malloc( sizeof(char) * (strlen(directory_entry->d_name)+1));
          strcpy(name, directory_entry->d_name);
          name[strlen(directory_entry->d_name)] = '\0';
          file_list[file_index] = name;
          file_list[file_index+1] = NULL;
          ++file_index;
      }
  }

  free(directory);
  return file_list;
}

char** FileH::Get_Files_in_Dir(const char* path)
{
  int num_files = 0;
  unsigned char filetype;
  struct dirent *directory_entry;   // Pointer for directory entry
  DIR *directory = opendir(path);   // opendir() returns a pointer of DIR type.

  if (directory == NULL)  // opendir returns NULL if couldn't open directory
  {
      printf("Could not open directory: %s\n", path);
      return NULL;
  }

  while ((directory_entry = readdir(directory)) != NULL)
  {
      filetype = directory_entry->d_type;
      if(filetype == DT_REG)
      {
            ++num_files;
      }
  }

  free(directory);

  char** file_list = (char**)malloc( sizeof(char*)*(num_files +1) );
  for(int i = 0; i<num_files+1; ++i)
    file_list[i] = NULL;

  int file_index = 0;
  directory = opendir(path);   // opendir() returns a pointer of DIR type.
  while ((directory_entry = readdir(directory)) != NULL)
  {
      filetype = directory_entry->d_type;
      if(filetype == DT_REG)
      {
          char* name = (char*)malloc( sizeof(char) * (strlen(directory_entry->d_name)+1));
          strcpy(name, directory_entry->d_name);
          name[strlen(directory_entry->d_name)] = '\0';
          file_list[file_index] = name;
          file_list[file_index+1] = NULL;
          ++file_index;
      }
  }

  free(directory);
  return file_list;
}






/*
|--------------------------------------------------------------------------
| List_Files
|--------------------------------------------------------------------------
|
| List the files in the directory given by path. Only "regular" files are
| returned. Folders are not returned
|
| @param:   path    the path to the directory whose files are listed
| @return:          a pointer to a list of strings, each string is a file in the directory
*/
char** FileH::List_Files_Full_Path(const char* path)
{
    if(path == NULL)
    {
        printf(COLOR_RED "Error<FileH::List_Files_Full_Path> argument path is NULL\n" COLOR_RESET);
    }

    int num_files = 0;
    unsigned char filetype;
    struct dirent *directory_entry;   // Pointer for directory entry
    DIR *directory = opendir(path);   // opendir() returns a pointer of DIR type.
    
    int path_len = strlen(path)+2;
    char path_copy[path_len];
    strcpy(path_copy, path);
    strcat(path_copy, "/");
    path_copy[path_len-1] = '\0';

    if (directory == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open directory: %s\n", path);
        return NULL;
    }

    while ((directory_entry = readdir(directory)) != NULL)
    {
        filetype = directory_entry->d_type;
        if(filetype == DT_REG)
        {
            ++num_files;
        }
    }

    free(directory);

    char** file_list = (char**)malloc( sizeof(char*)*(num_files +1) );
    for(int i = 0; i<num_files+1; ++i)
        file_list[i] = NULL;

    int file_index = 0;
    directory = opendir(path);   // opendir() returns a pointer of DIR type.
    while ((directory_entry = readdir(directory)) != NULL)
    {
        filetype = directory_entry->d_type;
        if(filetype == DT_REG)
        {
            int mem_alloc_size = strlen(directory_entry->d_name) + strlen(path_copy);
            char* filepath = (char*)malloc( sizeof(char) * (mem_alloc_size+1) );
            strcpy(filepath, path_copy);
            strcat(filepath, directory_entry->d_name);
            filepath[mem_alloc_size] = '\0';
            file_list[file_index] = filepath;
            file_list[file_index+1] = NULL; // make sure list is null terminated
            ++file_index;
        }
    }
    free(directory);
    return file_list;
}







// look for beginning with '##', then ending with '##'
bool FileH::Is_String_File_Title(char* line)
{
    StringH::Trim_Leading_WhiteSpace(line);
    char first_char = line[0];
    char second_char = line[1];

    if( !(first_char == '#' && second_char == '#') )
        return false;

    StringH::Trim_Trailing_WhiteSpace(line);
    int index_last_char = strlen(line)-1;
    int index_sec_last = strlen(line)-2;
    char last_char = line[index_last_char];
    char sec_last = line[index_sec_last];

    if( !(last_char == '#' && sec_last == '#') )
        return false;

    return true;
}


char* FileH::Get_Date()
{
     char* buffer = (char*)malloc(sizeof(char) * 22);
	 time_t rawtime;
	 struct tm* timeinfo;
	 time(&rawtime);
	 timeinfo = localtime(&rawtime);
	 strftime (buffer,30,"%m-%d-%y",timeinfo);

	 return buffer;
}

char* FileH::Get_Time()
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

void FileH::Print_File(const char* filepath)
{
    FILE* file_ptr;
    char readline[256];

    // File doesnt exist yet, must be created
    file_ptr = fopen(filepath, "r"); // open for writing, 'w' means overwrite any existing file, "a" means to append to file
    if( file_ptr == NULL )
    {
        printf("read error on file:   %s\n", filepath);
        perror("read file error");
        return;
    }
    printf("\n-----------------%s----------------------------------\n", filepath);
    while( fgets(readline, 256, file_ptr) != NULL )
    {
        printf("%s", readline);
    }
    printf("\n-------------------------------------------------------------------\n");
}