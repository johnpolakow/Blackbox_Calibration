
#ifndef _SSH_HELPER_H
#define _SSH_HELPER_H

#include <libssh/libssh.h>
#include <iterator>
#include <iostream>
#include <fstream>

#include "../Util/FileH.h"
#include "../Util/StringH.h"
#include "../SSH/systemd.h"
#include "../../Cal_Parameters.h"


class SSH_Helper
{
public:
    SSH_Helper(const char* host_ip, const char* host_mac_addr, const char* ssh_user, const char* ssh_password)
    {
        int ip_len = strlen(host_ip) + 1;
        int mac_len = strlen(host_mac_addr) +1;
        int user_len = strlen(ssh_user) + 1;
        int password_len = strlen(ssh_password) +1;

        this->host_IP = (char*)malloc(sizeof(char) * ip_len);
        strncpy(this->host_IP, host_ip, ip_len);

        this->host_MAC_addr = (char*)malloc(sizeof(char) * mac_len);
        strncpy(this->host_MAC_addr, host_mac_addr, mac_len);

        this->SSH_user = (char*)malloc(sizeof(char) * user_len);
        strncpy(this->SSH_user, ssh_user, user_len);

        this->SSH_password = (char*)malloc(sizeof(char) * password_len);
        strncpy(this->SSH_password, ssh_password, password_len);

        // Open session and set options
        this->session = ssh_new();
        if (this->session == NULL)
            exit(-1);
        ssh_options_set(this->session, SSH_OPTIONS_HOST, this->host_IP);
        ssh_options_set(this->session, SSH_OPTIONS_USER, this->SSH_user);
        ssh_options_set(this->session, SSH_OPTIONS_LOG_VERBOSITY, &SSH_VERBOSITY);
        ssh_options_set(this->session, SSH_OPTIONS_CIPHERS_C_S,"aes128-ctr");
        //ssh_options_set(this_session, SSH_OPTIONS_PORT, &SSH_PORT);

        // Connect to server
        int result = ssh_connect(this->session);
        if (result != SSH_OK)  
        {
            fprintf(stderr, "Error connecting: %s\n", ssh_get_error(this->session)); 
            ssh_free(this->session);
            free(this->host_IP);
            free(this->host_MAC_addr);
            free(this->SSH_user);
            free(this->SSH_password);
            this->session = NULL;
            exit(-1);
        }
    }

    ~SSH_Helper()
    {
        if( this->session != NULL)
        {
            ssh_free(this->session);
            this->session = NULL;
        }
        if(this->host_IP != NULL)
        {
            free(this->host_IP);
            this->host_IP = NULL;
        }
        if(this->host_MAC_addr != NULL)
        {
            free(this->host_MAC_addr);
            this->host_MAC_addr = NULL;
        }
        if(this->SSH_user != NULL)
        {
            free(this->SSH_user);
            this->SSH_user = NULL;
        }
        if(this->SSH_password != NULL)
        {
            free(this->SSH_password);
            this->SSH_password = NULL;
        }
    }

    // Incomplete Methods:
    void Create_Remote_SSH_DIR();

    // Completed Methods
    static bool Local_SSH_Keys_Are_Generated();
    char* Read_Local_SSH_Keys();

    bool Remote_Pi_Has_DIR(const char* dir_path);
    bool Remote_Pi_Has_SSH_dir_and_Authorized_Keys_File();

    int Verify_Known_Host();
    int Log_In_To_Host();
    void Disconnect();
    char* Read_SSH_Response(ssh_session session, ssh_channel channel);
    char* Get_Remote_Host_MAC_Address();

    bool Remote_DAQ_Process_Is_Running(bool print_status=true);
    void Restart_Remote_DAQ_Process();
    void Restart_Remote_Pi();
    void Verify_Remote_Host_Has_Our_SSH_Keys(char* remote_keys, char* local_keys);
    bool Verify_Cal_File_MAC_Matches_Remote_Pi_MAC(const char* cal_filepath);

    // these UNIX file permission bits are available for the mode argument (default is read and write for owner)
    //  S_IRUSR     read permission for the owner
    //  S_IWUSR     write permission for the owner
    //  S_IXUSR     execute permission for the owner of the file
    //  S_IROTH     read permission for other users
    //  S_IWOTH     write permission for other users
    //  S_IXOTH     execute permission for other users
    int SCP_Write_Remote_File(const char* local_filepath, const char* remote_filepath, int mode = S_IRUSR |  S_IWUSR);
    int SCP_Create_Remote_Dir(const char* remote_dir);
    char* SCP_Read_Remote_File(const char* remote_filepath);
    int Append_Local_SSH_Key_To_Remote_Host(char* key);
    void Send_Lookup_Tables_To_DUT(char** filepath_list);
    void Create_Remote_LUT_Directories();

    static char** Exec_Shell_Command(const char* command);
    char* Exec_Remote_Shell_Command(const char* command);
    static void Free_String_Array_Memory(char** string_array);
    static int show_remote_processes(ssh_session session);
    static char** Get_Local_LUT_Files(char* dirpath);   // finds files matching MAC Addr of remote host we're connected to

    char* host_IP;
    char* SSH_user;
    char* SSH_password;
    char* host_MAC_addr;

private:
    bool Remote_Pi_Has_SSH_DIR();
    bool Remote_Pi_Has_Authorized_Keys_File();
    ssh_session session;
};

#endif