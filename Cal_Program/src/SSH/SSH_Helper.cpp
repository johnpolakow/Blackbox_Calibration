

#include "SSH_Helper.h"

#define STDOUT 1

char* SSH_Helper::Read_Local_SSH_Keys()
{
    char filepath[100];
    StringH::Erase_Num_Chars(filepath, 100);
    bool pub_key_file_exists = false;

    // if running on the linux mint laptop
    if(FileH::File_Exists("/home/mint/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/mint/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    // if running on Pi Cal Box
    if( FileH::File_Exists("/home/pi/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/pi/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    if(!pub_key_file_exists)
    {
        printf("could not find local SSH public key file. ( /home/pi/ssh/id_rsa.pub )\n");
        printf("create SSH keys with following commands:\n");
        printf("\tmkdir ~/.ssh\n");
        printf("\tcd ~/.ssh\n");
        printf("\tssh-keygen -t rsa -C \"FL100\"\n");
        return NULL;
    }
    else
    {
        printf(COLOR_CYAN "\n\t---READING LOCAL SSH KEYFILE---\n" COLOR_RESET);
    }
    //printf("\npubkey filepath:\t%s\n", filepath);

    FILE* pub_key_file = FileH::Open_Existing_File(filepath);
    char readline[2000];
    StringH::Erase_Num_Chars(readline, 2000);
    char* key_contents = NULL;

    while( (fgets(readline, 1000, pub_key_file) != NULL) )
    {
        //printf("line read[%ld]: \n" COLOR_YELLOW "'%s'\n" COLOR_RESET, strlen(readline), readline);
        bool contains;
        char* first_token = StringH::Get_First_Token(readline);
        contains = StringH::String_Contains_Ignore_Case(first_token, "ssh-rsa");    // public key begins with ssh-rsa
        if(contains)
        {
            StringH::Trim_WhiteSpace(readline);
            int file_len = strlen(readline)*2;
            printf("mem allocation size for file length: %d\n", file_len);
            key_contents = (char*)malloc(sizeof(char) * file_len);
            strncpy(key_contents, readline, file_len-1);
            //key_contents[file_len] = '\0';
        }
        free(first_token);
    }
    FileH::Close_File(pub_key_file);
    printf(" LOCAL KEYFILE[%ld]:\t" COLOR_MAGENTA "%s\n" COLOR_RESET COLOR_YELLOW "%s\n" COLOR_RESET, strlen(key_contents), filepath, key_contents);
    return key_contents;
}


bool SSH_Helper::Local_SSH_Keys_Are_Generated()
{
    char filepath[100];
    StringH::Erase_Num_Chars(filepath, 100);
    bool pub_key_file_exists = false;

    // check that local computer has the ssh keys generated already
    // must detect what user currently is. If mint, check: /home/mint/ssh
    // if pi, check /home/pi/
    if(!FileH::Dir_Exists("/home/mint/.ssh") && !FileH::Dir_Exists("/home/pi/.ssh"))    // check local directory "~/.ssh/" exists
    {
        printf(".ssh dir does not exist on local computer. Cannot send keys\n");
        return false;
    }

    // if running on the linux mint laptop
    if(FileH::File_Exists("/home/mint/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/mint/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    // if running on Pi Cal Box
    if( FileH::File_Exists("/home/pi/.ssh/id_rsa.pub") )    // check the local public key file exists
    {
        strcpy(filepath, "/home/pi/.ssh/id_rsa.pub");
        pub_key_file_exists = true;
    }

    if(!pub_key_file_exists)
    {
        printf("local public key file does not exist, not generated yet or wrong location.\n");
        printf("expecting to find key file in [user home dir]/.ssh \t\tCannot send keys\n");
        return false;
    }

    FILE* pub_key_file = FileH::Open_Existing_File(filepath);
    char readline[256];

    while( (fgets(readline, 256, pub_key_file) != NULL) )
    {
        bool contains;
        printf("line read from key file: %s\n", readline);
        char* first_token = StringH::Get_First_Token(readline);
        printf("first token:  %s\n", first_token);
        contains = StringH::String_Contains_Ignore_Case(first_token, "ssh-rsa");    // public key begins with ssh-rsa
        printf("contains result: %d\n", contains);
        if(contains)
            return true;
    }
    return false;
}


/*
void SSH_Helper::Create_Remote_SSH_DIR()
{ 
    
    const char* SAVE_DIR = "~/.ssh";

    if( !FileH::Dir_Exists( SAVE_DIR) )
    {
        mkdir( SAVE_DIR, 0700);
        printf("\tSSH DIR:  " COLOR_BOLD_MAGENTA "%s\n" COLOR_RESET, SAVE_DIR);
    }
    else
    {
        printf("dir already exists\n");
    }

    return;
}
*/



char* SSH_Helper::Exec_Remote_Shell_Command(const char* command)
{
    int rc;
    ssh_channel channel;
    channel = ssh_channel_new(this->session);

    if (channel == NULL)
    {
        return NULL;
        printf("channel is NULL\n");
    }

    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        printf("SSH open session was not OK\n");
        ssh_channel_free(channel);
        return NULL;
    }

    printf("Executing remote command:" COLOR_MAGENTA "\t%s\n" COLOR_RESET, command);

    rc = ssh_channel_request_exec(channel, command); // execute the command: echo "[ssh key]" >> /home/pi/.ssh/authorized_keys
    if (rc != SSH_OK)
    {
        printf("trouble executing remote command: %s\n", command);
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return NULL;
    }

    char* response = SSH_Helper::Read_SSH_Response(this->session, channel);

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return response;
}



/*
|--------------------------------------------------------------------------
| Exec_Shell_Command
|--------------------------------------------------------------------------
|
| Execute a command at the terminal prompt. The command is passed as argument.
| A string of the shell output response is returned. The string captures
| stdout and stderr.
|
| @param:  command    the shell command to execute
| @return:            a string of the shell response is returned
*/
char** SSH_Helper::Exec_Shell_Command(const char* command)
{
    int MAX_LINES = 100;
    int MAX_LINE_LENGTH = 150;
    char read_line[250];

    FILE* command_shell = popen(command, POPEN_READ);                 //  The popen() function opens a process by creating a pipe, forking and invoking the shell.
    if (command_shell == NULL)
    {
        printf("Error opening pipe for shell command!!\n" );
        exit(1);
    }

    int line_count = 0;
    char**  string_array = (char**)malloc(sizeof(char*) * MAX_LINES);        // allocate memory for pointer to strings
    while (fgets(read_line, sizeof(read_line), command_shell) != NULL)     /* Read the output a line at a time - output it. */
    {
        StringH::Trim_WhiteSpace(read_line);
        printf("RESPONSE:\t%s\n", read_line);
        //string_array[line_count] = (char*)malloc(sizeof(char) * MAX_LINE_LENGTH); // allocate memory for the
		string_array[line_count] = (char*)malloc(sizeof(char) * ( strlen(read_line) +10 ) ); // allocate memory for the return string
        strcpy(string_array[line_count], read_line);
        ++line_count;
    }
    string_array[line_count] = NULL;   // Add a marker for the last string in the array
    pclose(command_shell);     // close file pointer to STDOUT
    return string_array;
}

/*
void SSH_Helper::Free_String_Array_Memory(char** string_array)
{
    int i = 0;
    while(string_array[i] != NULL)
    {
        free(string_array[i]);
        ++i;
    }
    free(string_array);

    return;
}
*/


int SSH_Helper::show_remote_processes(ssh_session session)
{
  printf("\n\n");
  ssh_channel channel;
  int rc;
  char buffer[256];
  int nbytes;
  channel = ssh_channel_new(session);
  if (channel == NULL)
    return SSH_ERROR;
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK)
  {
    ssh_channel_free(channel);
    return rc;
  }
  rc = ssh_channel_request_exec(channel, "ps aux");
  if (rc != SSH_OK)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return rc;
  }
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  printf(COLOR_BOLD_GREEN "\n\nBUFFER [%d]:\n %s\n" COLOR_RESET, nbytes, buffer);
  printf(COLOR_RESET);
  printf("--BUFFER DONE--\n");
  while (nbytes > 0)
  {
    if (write(STDOUT, buffer, nbytes) != (unsigned int) nbytes) // printing to terminal, writes contents of the buffer to standard out
    {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }
  if (nbytes < 0)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_ERROR;
  }

  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
  return SSH_OK;
}


void SSH_Helper::Verify_Remote_Host_Has_Our_SSH_Keys(char* remote_keys, char* local_keys)
{

    bool remote_host_has_our_keys = StringH::String_Contains(remote_keys, local_keys);

    if(!remote_host_has_our_keys)    // if our ssh_key is not installed on remote pi, add it
    {
        this->Append_Local_SSH_Key_To_Remote_Host(local_keys);
    }

    printf(COLOR_CYAN "\t--REMOTE HOST HAS OUR SSH KEYS--\t" COLOR_RESET);
    printf("... login without password is enabled\n\n");
}


bool SSH_Helper::Remote_Pi_Has_SSH_DIR()
{
    printf("\n");
    ssh_channel channel;
    int rc;
    char buffer[600];
    char temp[256];
    int nbytes;
    channel = ssh_channel_new(this->session);


    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, "ls -al | grep .ssh");
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    StringH::Erase_Num_Chars(buffer, 600);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes >0)
    {
        strncat(buffer, temp, nbytes);
    }

    while (nbytes > 0)
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            strncat(buffer, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
        }
    }

    printf("\nSearching remote pi for '.ssh dir' \tls -al | grep .ssh \n");
    printf(COLOR_YELLOW "%s" COLOR_RESET, buffer);
    printf(COLOR_RESET);

    bool has_ssh_dir = StringH::String_Contains(buffer, ".ssh");
    if( has_ssh_dir)
    {
        printf("\t[remote Pi has .ssh dir]\n");
    }
    else
    {
        // create SSH dir
        SSH_Helper::SCP_Create_Remote_Dir("/home/pi/.ssh");
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return has_ssh_dir;
}


char* SSH_Helper::Read_SSH_Response(ssh_session session, ssh_channel channel)
{
    char buffer[5000];
    char temp[256];
    int nbytes;
    StringH::Erase_Num_Chars(buffer, 5000);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes > 0)
    {
        strncat(buffer, temp, nbytes);
    }
    else
    {
        return NULL;
    }

    while(nbytes > 0)
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            strncat(buffer, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
        }
    }

    if( strlen(buffer) > 0 )
        return strdup(buffer);
    
    else
    {
        return NULL;
    }
}

bool SSH_Helper::Remote_DAQ_Process_Is_Running(bool print_status)
{
    int rc;
    ssh_channel channel;
    channel = ssh_channel_new(this->session);

    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }

    int cmd_len = strlen("sudo systemctl status Blackberry.service")+10;
    char status_cmd[cmd_len];
    sprintf(status_cmd, "sudo systemctl status %s", DAQ_PROCESS_SERVICE_NAME);

    if(print_status)
    {
        printf(COLOR_CYAN "\n\t-- CHECKING PI DAQ PROCESS STATUS --\n" COLOR_RESET);
    }

    rc = ssh_channel_request_exec(channel, status_cmd); // execute the command: echo "[ssh key]" >> /home/pi/.ssh/authorized_keys
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    char* service_status_str = SSH_Helper::Read_SSH_Response(this->session, channel);

    bool is_active = parse_systemd::Service_Is_Active(service_status_str);
    bool is_loaded = parse_systemd::Service_Is_Loaded(service_status_str);
    bool is_running = parse_systemd::Service_Is_Running(service_status_str);

    bool is_exited = parse_systemd::Service_Is_Exited(service_status_str);
    bool is_inactive = parse_systemd::Service_Is_Inactive(service_status_str);
    bool is_failed = parse_systemd::Service_Is_Failed(service_status_str);

    if( is_exited || is_inactive || is_failed)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, service_status_str);
        free(service_status_str);
        if(is_exited && print_status)
            printf(COLOR_BOLD_RED "ERROR: Pi DAQ process has exited\n" COLOR_RESET);
        if(is_inactive && print_status)
            printf(COLOR_BOLD_RED "ERROR: Pi DAQ process is inactive (dead)\n" COLOR_RESET);
        if(is_failed && print_status)
            printf(COLOR_BOLD_RED "ERROR: Pi DAQ process has failed\n" COLOR_RESET);
        
        if(print_status)
        {
            printf(COLOR_RED "  --Restart DAQ process on Pi with this command (execute on Pi):\n" COLOR_RESET);
            printf(COLOR_RED "\tsudo systemctl restart Blackberry.service\n\n" COLOR_RESET);
        }
        return false;
    }

    if( is_active && is_loaded && is_running )
    {
        if(print_status)
        {
            printf(COLOR_GREEN "%s\n" COLOR_RESET, service_status_str);
        }
        free(service_status_str);
        return true;
    }
    else
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, service_status_str);
        free(service_status_str);
        if( !is_active && print_status)
        {
            printf(COLOR_RED "Pi DAQ Process is not active\n" COLOR_RESET);
        }
        if( !is_running && print_status)
        {
            printf(COLOR_RED "Pi DAQ Process is not running\n" COLOR_RESET);
        }
        if( !is_loaded && print_status)
        {
            printf(COLOR_RED "Pi DAQ Process is not loaded\n" COLOR_RESET);
        }
    }
    if( is_running)
    {
        free(service_status_str);
        return true;
    }
    // evaluate the buffer as to whether it has the line: 'Active: active (running)'

    free(service_status_str);
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return false;
}

void SSH_Helper::Restart_Remote_DAQ_Process()
{
    int rc;
    ssh_channel channel;
    channel = ssh_channel_new(this->session);

    if (channel == NULL)
        return;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return;
    }

    int cmd_len = strlen("sudo systemctl restart Blackberry.service")+10;
    char status_cmd[cmd_len];
    sprintf(status_cmd, "sudo systemctl restart %s", DAQ_PROCESS_SERVICE_NAME);

    printf(COLOR_CYAN "\t-- RESTARTING Pi DAQ PROCESS: %s --\n", DAQ_PROCESS_SERVICE_NAME);
    printf(COLOR_CYAN "\t\t(wait 30 seconds)  ....\n\n");
    rc = ssh_channel_request_exec(channel, status_cmd); // execute the command: echo "[ssh key]" >> /home/pi/.ssh/authorized_keys
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return;
    }

    char* restart_response = SSH_Helper::Read_SSH_Response(this->session, channel);
    free(restart_response); // response should be NULL
    printf("done restarting.\n");
}

void SSH_Helper::Restart_Remote_Pi()
{

    int rc;
    ssh_channel channel;
    channel = ssh_channel_new(this->session);

    if (channel == NULL)
        return;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return;
    }

    int cmd_len = strlen("sudo reboot -h now")+1;
    char reboot_cmd[cmd_len];
    sprintf(reboot_cmd, "sudo reboot -h now");

    printf(COLOR_CYAN "\t-- REBOOTING Pi --\n" COLOR_RESET);
    rc = ssh_channel_request_exec(channel, reboot_cmd); // execute the command: echo "[ssh key]" >> /home/pi/.ssh/authorized_keys
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return;
    }
    return;
}

char* SSH_Helper::Get_Remote_Host_MAC_Address()
{
    return strdup(this->host_MAC_addr);
}

char** SSH_Helper::Get_Local_LUT_Files(char* dirpath)
{
    char** lut_files = FileH::List_Files_Full_Path(dirpath);  //wrap this func in SSH Helper
    return lut_files;
}


void SSH_Helper::Send_Lookup_Tables_To_DUT(char** filepath_list)
{
    if(filepath_list == NULL)
        return;
    
    char remote_filepath[200];

    int file_index = 0;
    while(filepath_list[file_index] != NULL)
    {
        char* local_LUT_file = filepath_list[file_index];
        bool LUT_matches_host = this->Verify_Cal_File_MAC_Matches_Remote_Pi_MAC(local_LUT_file);
        if( LUT_matches_host )
        {
            //printf("MACs match for file: %s,  and connected host: %s \t sending...\n", local_LUT_file, this->host_MAC_addr);
            char* filename = FileH::Extract_Filename_and_Extension(local_LUT_file);
            char* remote_filepath = FileH::Concat_Path(REMOTE_PI_CAL_FILES_DIR, filename);
            //printf("remote filepath: %s\n", remote_filepath);
            SSH_Helper::SCP_Write_Remote_File(local_LUT_file, remote_filepath);
        }
        else
        {
            printf("MACs do not match for file: %s,  and connected host: %s\n", local_LUT_file, this->host_MAC_addr);
        }
        ++file_index;
    }
    return;
}

void SSH_Helper::Create_Remote_LUT_Directories()
{
    printf(COLOR_CYAN "\n\t--CREATING REMOTE FILES AND DIRECTORIES--" COLOR_RESET);
    this->SCP_Create_Remote_Dir(REMOTE_PI_FIRMWARE_SRC_DIR);
    this->SCP_Create_Remote_Dir(REMOTE_PI_CAL_FILES_DIR);
}

// first line in LUT calibration file should be of the form:
//  LUT for Pi w/ MAC Address: B8.27.EB.2B.FB.70
bool SSH_Helper::Verify_Cal_File_MAC_Matches_Remote_Pi_MAC(const char* cal_filepath)
{

    bool cal_file_exists = FileH::File_Exists(cal_filepath);

    if(!cal_file_exists)
    {
        printf("cal file does not exist: %s\n", cal_filepath);
        return false;
    }

    char* lut_mac_addr = FileH::Get_LUT_File_MAC_Addr(cal_filepath);
    
    if(lut_mac_addr == NULL)
    {
        printf("file could not have its mac parsed: %s\n", cal_filepath);
        return false;
    }

    //printf("file parsed MAC Addr: %s\n", lut_mac_addr);
    //printf("remote host MAC Addr: %s\n", this->host_MAC_addr);

    char* host_mac_addr = strdup(this->host_MAC_addr);
    StringH::Convert_Character(host_mac_addr, ':', '.');

    bool MACs_match = StringH::Strings_Match(lut_mac_addr, host_mac_addr);
    if(MACs_match)
        return true;
    else
    {
        return false;
    }    
}

bool SSH_Helper::Remote_Pi_Has_SSH_dir_and_Authorized_Keys_File()
{
    printf(COLOR_CYAN "\t-- CHECKING REMOTE HOST HAS SSH CONFIGURED --\n" COLOR_RESET);
    bool has_ssh_dir = this->Remote_Pi_Has_SSH_DIR();           // check to see if Pi has dir /home/pi/.ssh
    bool has_authorized_keys = this->Remote_Pi_Has_Authorized_Keys_File();    // check to see if Pi has SSH Keys

    if(!has_ssh_dir)
    {
        printf(COLOR_RED "Error: Pi does not have directory: /home/pi/.ssh" COLOR_RESET);
    }
    if(!has_authorized_keys)
    {
        printf(COLOR_RED "Error: Pi does not have autothorized keys file: /home/pi/.ssh/authorized_keys" COLOR_RESET);
        printf(COLOR_RED "must create SSH keys on remote Pi" COLOR_RESET);
    }

    if(has_ssh_dir && has_authorized_keys)
        return true;
    else
        return false;
}

bool SSH_Helper::Remote_Pi_Has_DIR(const char* dir_path)
{
    printf("\n");
    ssh_channel channel;
    int rc;
    char response[600];
    char temp[256];
    char search_cmd[200];
    int nbytes;

    StringH::Erase_Num_Chars(search_cmd, 200);
    sprintf(search_cmd, "ls %s", dir_path);

    channel = ssh_channel_new(this->session);


    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, search_cmd);
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    StringH::Erase_Num_Chars(response, 600);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes >0)
    {
        strncat(response, temp, nbytes);
    }

    printf(COLOR_RESET);
    while (nbytes > 0)  // loop through until the complete response is read
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            strncat(response, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
            usleep(10);
        }
    }

    bool has_dir = true;
    if( StringH::String_Contains_Ignore_Case(response, "cannot access"))
    {
        has_dir = false;
    }
    if( StringH::String_Contains_Ignore_Case(response, "No such file or directory"))
    {
        has_dir = false;
    }

/*
    if(has_dir == true)
    {
        printf("\tremote dir '%s' " COLOR_GREEN "EXISTS\n" COLOR_RESET, dir_path);
        //printf(COLOR_RESET);
    }
*/
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return has_dir;
}


// pre condition is that the folder .ssh exists on the Pi in user directory
bool SSH_Helper::Remote_Pi_Has_Authorized_Keys_File()
{
    printf("\n");
    ssh_channel channel;
    int rc;
    char buffer[600];
    char temp[256];
    int nbytes;
    channel = ssh_channel_new(this->session);


    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }
    rc = ssh_channel_request_exec(channel, "ls .ssh/");
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    StringH::Erase_Num_Chars(buffer, 600);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes >0)
    {
        strncat(buffer, temp, nbytes);
    }

    printf(COLOR_RESET);
    printf("\n");
    while (nbytes > 0)
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            //printf("temp: %s\n", temp);
            strncat(buffer, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
        }
    }

    printf("searching remote pi for 'authorized_keys' file:\tls .ssh/ \n");
    printf(COLOR_YELLOW "%s" COLOR_RESET, buffer);
    printf(COLOR_RESET);

    bool has_keys_file = StringH::String_Contains(buffer, "authorized_keys");
    if( has_keys_file)
    {
        printf("\t[remote Pi has authorized_keys file]\n\n");
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return has_keys_file;
}


int SSH_Helper::Append_Local_SSH_Key_To_Remote_Host(char* key)
{
    printf(COLOR_CYAN "\n\t--ADDING SSH KEY TO REMOTE HOST--\n" COLOR_RESET);
    ssh_channel channel;
    int rc;
    char buffer[1200];
    char temp[256];
    int nbytes;
    channel = ssh_channel_new(this->session);


    if (channel == NULL)
        return SSH_ERROR;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }

    int keylen = strlen(key);
    int cmd_len = keylen + strlen("echo \"\" >> /home/pi/.ssh/authorized_keys")+1;
    char append_cmd[cmd_len];
    sprintf(append_cmd, "echo \"%s\" >> /home/pi/.ssh/authorized_keys", key);

    rc = ssh_channel_request_exec(channel, append_cmd); // execute the command: echo "[ssh key]" >> /home/pi/.ssh/authorized_keys
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    StringH::Erase_Num_Chars(buffer, 1200);
    StringH::Erase_Num_Chars(temp, 256);
    nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);

    if(nbytes >0)
    {
        strncat(buffer, temp, nbytes);
    }

    printf("\n");
    while (nbytes > 0)
    {
        nbytes = ssh_channel_read(channel, temp, sizeof(temp), 0);
        if(nbytes >0)
        {
            printf("temp: %s\n", temp);
            strncat(buffer, temp, nbytes);
            StringH::Erase_Num_Chars(temp, 256);
        }
    }

    printf(COLOR_BOLD_GREEN "%s\n" COLOR_RESET, buffer);
    printf(COLOR_RESET);

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_OK;
}

char* SSH_Helper::SCP_Read_Remote_File(const char* remote_filepath)
{
    ssh_scp scp;
    int rc;
    
    printf(COLOR_CYAN "\n\t---READING PI REMOTE SSH KEYFILE--\n" COLOR_RESET);
    scp = ssh_scp_new(this->session, SSH_SCP_READ | SSH_SCP_RECURSIVE, remote_filepath);
    if (scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n", ssh_get_error(this->session));
        return NULL;
    }
    
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n",
        ssh_get_error(this->session));
        ssh_scp_free(scp);
        return NULL;
    }
    
    int size, mode;
    
    rc = ssh_scp_pull_request(scp);
    if (rc != SSH_SCP_REQUEST_NEWFILE)
    {
        fprintf(stderr, "Error receiving information about file: %s\n", ssh_get_error(this->session));
        return NULL;
    }
    
    size = ssh_scp_request_get_size(scp);
    char* filename = strdup(ssh_scp_request_get_filename(scp));
    mode = ssh_scp_request_get_permissions(scp);
    printf("\nReceiving file " COLOR_MAGENTA "%s" COLOR_RESET ", size %d bytes, permissions 0%o\n", filename, size, mode);
    free(filename);
    
    char* file_contents = (char*)malloc(size+1);
    if (file_contents == NULL)
    {
        fprintf(stderr, "Memory allocation error for reading remote file: %s\n", remote_filepath);
        printf(COLOR_RED "Memory allocation error for reading remote file: %s\n" COLOR_RESET, remote_filepath);
        printf("\tMemory allocation size: %d\n", size);
        free(file_contents);
        return NULL;
    }
    
    ssh_scp_accept_request(scp);
    rc = ssh_scp_read(scp, file_contents, size);
    file_contents[size] = '\0';
    if (rc == SSH_ERROR)
    {
        fprintf(stderr, "Error receiving remote file data: %s\n", ssh_get_error(this->session));
        free(file_contents);
        return NULL;
    }
    
    rc = ssh_scp_pull_request(scp);
    if (rc != SSH_SCP_REQUEST_EOF)
    {
        fprintf(stderr, "Unexpected request: %s\n", ssh_get_error(this->session));
    }
    
    printf("\nREMOTE KEYFILE[%ld]:\t" COLOR_MAGENTA "%s\n" COLOR_RESET COLOR_YELLOW "%s\n" COLOR_RESET, strlen(file_contents), remote_filepath, file_contents);
    ssh_scp_close(scp);
    ssh_scp_free(scp);
    return file_contents;
}

int SSH_Helper::Verify_Known_Host()
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = NULL;
    ssh_key srv_pubkey = NULL;
    size_t hlen;
    char buf[10];
    char *hexa;
    char *p;
    int cmp;
    int rc;

    rc = ssh_get_server_publickey(this->session, &srv_pubkey);
    if (rc < 0) 
    {   return -1; }

    rc = ssh_get_publickey_hash(srv_pubkey, SSH_PUBLICKEY_HASH_SHA1, &hash, &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) 
    {   return -1; }

    state = ssh_session_is_known_server(this->session);
    switch (state) 
    {
        case SSH_KNOWN_HOSTS_OK:
            /* OK */
            //printf(COLOR_GREEN "\nSSH known hosts OK\n" COLOR_RESET);
            printf(COLOR_RESET);
            break;
        case SSH_KNOWN_HOSTS_CHANGED:
            fprintf(stderr, "Host key for server changed: it is now:\n");
            //ssh_print_hexa("Public key hash", hash, hlen);
            fprintf(stderr, "For security reasons, connection will be stopped\n");
            ssh_clean_pubkey_hash(&hash);
            printf("\nSSH known hosts changed\n");
            printf(COLOR_RED "\tError, could not get authentication\n" COLOR_RESET);
            printf(COLOR_RED "\t-you may need to regenerate SSH keys, or reconfigure PI SSH params.\n");
            printf("Disconnecting, cannot verify remote host\n");
            ssh_disconnect(session);
            return -1;
        case SSH_KNOWN_HOSTS_OTHER:
            fprintf(stderr, "The host key for this server was not found but another "
                    "type of key exists.\n");
            fprintf(stderr, "An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            ssh_clean_pubkey_hash(&hash);
            printf("\nSSH known hosts other\n");
            printf(COLOR_RED "\t-you may need to regenerate SSH keys, or reconfigure PI SSH params.\n");
            printf("Disconnecting, cannot verify remote host\n");
            ssh_disconnect(session);
            return -1;
        case SSH_KNOWN_HOSTS_NOT_FOUND:
            fprintf(stderr, "Could not find known host file.\n");
            fprintf(stderr, "If you accept the host key here, the file will be"
                    "automatically created.\n");
            /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_KNOWN_HOSTS_UNKNOWN:
            printf("\nSSH hosts unknown\n");
            hexa = ssh_get_hexa(hash, hlen);
            fprintf(stderr,"The server is unknown. Do you trust the host key?\n");
            fprintf(stderr, "Public key hash: %s\n", hexa);
            fprintf(stderr, "To accept you must type in 'yes' and hit enter\n");
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
            p = fgets(buf, sizeof(buf), stdin);
            if (p == NULL) {
                printf(COLOR_RED "\tError, could not get authentication (you did not select yes)\n" COLOR_RESET);
                printf("Disconnecting, cannot verify remote host\n");
                ssh_disconnect(session);
                return -1;
            }
            cmp = strncasecmp(buf, "yes", 3);
            if (cmp != 0) {
                printf(COLOR_RED "\tError, could not get authentication (you did not select yes)\n" COLOR_RESET);
                printf("Disconnecting, cannot verify remote host\n");
                ssh_disconnect(session);
                return -1;
            }
            rc = ssh_session_update_known_hosts(session);
            if (rc < 0) {
                fprintf(stderr, "Error %s\n", strerror(errno));
                printf(COLOR_RED "\tError, could not get authentication\n" COLOR_RESET);
                printf(COLOR_RED "\t-you may need to regenerate SSH keys, or reconfigure PI SSH params.\n");
                printf("Disconnecting, cannot verify remote host\n");
                ssh_disconnect(session);
                return -1;
            }
            break;
        case SSH_KNOWN_HOSTS_ERROR:
            fprintf(stderr, "Error %s", ssh_get_error(session));
            ssh_clean_pubkey_hash(&hash);
            printf(COLOR_RED "\tError, could not get authentication\n" COLOR_RESET);
            printf(COLOR_RED "\t-you may need to regenerate SSH keys, or reconfigure PI SSH params.\n");
            printf("Disconnecting, cannot verify remote host\n");
            ssh_disconnect(session);
            return -1;
    }
    ssh_clean_pubkey_hash(&hash);

    return 0;
}

void SSH_Helper::Disconnect()
{
    ssh_disconnect(this->session);
    this->~SSH_Helper();
}


int SSH_Helper::SCP_Write_Remote_File(const char* local_filepath, const char* remote_filepath, int mode)
{
    char* remote_filename = FileH::Extract_Filename_and_Extension(remote_filepath);   // extracts "doc1.txt" from "/home/pi/test/new_dir/doc1.txt"
    char* remote_dir = FileH::Extract_Parent_Dir(remote_filepath);      // extracts "/home/pi/test/" from "/home/pi/test/new_file.txt"
    
    SSH_Helper::SCP_Create_Remote_Dir(remote_dir); // creates remote directory if it does not exist. If it does exist, no change
    
    ssh_scp scp;
    int rc;
    scp = ssh_scp_new(this->session, SSH_SCP_WRITE, remote_dir);   // initialize scp module in the directory where we want to write the file

    if (scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n", ssh_get_error(this->session));
        return SSH_ERROR;
    }
    
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n", ssh_get_error(this->session));
        ssh_scp_free(scp);
        return rc;
    }
 
    // verify source file actually exists
    if( !FileH::File_Exists(local_filepath) )
    {
        printf(COLOR_RED "Error: local file '%s' does not exist\n" COLOR_RESET, local_filepath);
        ssh_scp_close(scp);
        ssh_scp_free(scp);
        return SSH_ERROR;
    }

    bool file_exists = FileH::File_Exists(local_filepath);
    if(!file_exists)
    {
        printf("local file %s does not exist, cannot send to remote host\n", local_filepath);
    }

    printf("\nlocal file: '%s', to remote filepath: " COLOR_MAGENTA "%s\n" COLOR_RESET, local_filepath, remote_filepath);

    //long size = FileH::Get_File_Size(local_filepath);   // get file size, to know how large of a buffer to create
    std::ifstream input_file_stream(local_filepath);    // create input stream of the file
    std::string file_str(std::istreambuf_iterator<char>{input_file_stream}, {});    // read entire file into 'C++' string

    // Convert the C++ string to C string
    char* file_contents = new char [file_str.length()+1];  // allocate memory to store contents of file read as 'C' string
    strcpy (file_contents, file_str.c_str());                 // copy the contents into the allocated memory
    int file_length = strlen(file_contents);

    printf("%s [%d chars]:\n", remote_filepath, file_length);
    printf("------------------------------------\n");
    printf( COLOR_YELLOW "%s\n" COLOR_RESET, file_contents);
    printf("------------------------------------\n");

    rc = ssh_scp_push_file(scp, remote_filename, file_length, mode);    // prepare for file transfer

    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't open remote file: %s\n", ssh_get_error(this->session));
        printf(COLOR_BOLD_RED "Can't open remote file: %s\n" COLOR_RESET, ssh_get_error(this->session));
        return rc;
    }
    
    rc = ssh_scp_write(scp, file_contents, file_length);                // actually write the file
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't write to remote file: %s\n", ssh_get_error(this->session));
        printf(COLOR_BOLD_RED "Can't write to remote file: %s\n" COLOR_RESET, ssh_get_error(this->session));
        return rc;
    }

    delete[] file_contents;
    free(remote_filename);
    free(remote_dir);
    ssh_scp_close(scp);
    ssh_scp_free(scp);
    return SSH_OK;
}

// remote dir argument should be fully qualified path
// for example: /home/pi/test/new_dir
//      -- creates the directory "new_dir" within already existing directory "test"
int SSH_Helper::SCP_Create_Remote_Dir(const char* remote_dir)
{
    char* new_dir_location = FileH::Extract_Parent_Dir(remote_dir); // extracts "/home/pi/test/" from "/home/pi/test/new_dir"
    char* new_dir_name = FileH::Extract_Filename(remote_dir);
      
    bool dir_exists = SSH_Helper::Remote_Pi_Has_DIR(remote_dir);
    if(dir_exists)
    {
        free(new_dir_location);
        free(new_dir_name);
        return SSH_OK;
    }


    ssh_scp scp;
    int rc;
    scp = ssh_scp_new(this->session, SSH_SCP_WRITE | SSH_SCP_RECURSIVE, new_dir_location);   // initialize at the location we want to create dir

    if (scp == NULL)
    {
        free(new_dir_location);
        free(new_dir_name);
        fprintf(stderr, "Error allocating scp session: %s\n", ssh_get_error(this->session));
        return SSH_ERROR;
    }
    
    rc = ssh_scp_init(scp);
    if (rc != SSH_OK)
    {
        free(new_dir_location);
        free(new_dir_name);
        fprintf(stderr, "Error initializing scp session: %s\n", ssh_get_error(this->session));
        ssh_scp_free(scp);
        return rc;
    }
    
    rc = ssh_scp_push_directory(scp, new_dir_name, S_IRWXU);
    if (rc != SSH_OK)
    {
        free(new_dir_location);
        free(new_dir_name);
        fprintf(stderr, "Can't create remote directory: %s\n", ssh_get_error(this->session));
        return rc;
    }

    printf("created remote dir: " COLOR_YELLOW "/%s" COLOR_RESET " in %s\n", new_dir_name, new_dir_location);
    free(new_dir_location);
    free(new_dir_name); 
    ssh_scp_close(scp);
    ssh_scp_free(scp);
    return SSH_OK;
}


int SSH_Helper::Log_In_To_Host()
{
    printf(COLOR_CYAN "\n\t--- CONNECTING TO REMOTE PI VIA SSH ---\n" COLOR_RESET);
    printf("------------------------------\n");
    printf("logging into host " COLOR_MAGENTA "%s" COLOR_RESET " as user " COLOR_MAGENTA "%s" COLOR_RESET " with passwd " COLOR_MAGENTA "%s  " COLOR_RESET, 
            this->host_IP, this->SSH_user, this->SSH_password);

    int verify_result = this->Verify_Known_Host();
    if ( verify_result < 0)     // Verify the server's identity
    {
        printf(COLOR_RED " ... Failed" COLOR_RESET);
        printf("------------------------------\n\n");
        ssh_free(this->session);
        exit(-1);
    }

    int auth_result = ssh_userauth_password(this->session, NULL, this->SSH_password);
    if (auth_result != SSH_AUTH_SUCCESS)
    {
        printf(COLOR_RED " ... Failed" COLOR_RESET);
        printf("------------------------------\n\n");
        fprintf(stderr, "Error authenticating with password. %s\n", ssh_get_error(this->session));
        ssh_disconnect(this->session);
        ssh_free(this->session);
        exit(-1);
    }

    printf(COLOR_GREEN " ... Success\n" COLOR_RESET);
    printf("------------------------------\n\n");
    return 1;
}