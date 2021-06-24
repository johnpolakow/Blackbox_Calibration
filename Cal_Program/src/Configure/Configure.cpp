#include "Configure.h"

bool Configure::Check_bcm2835_Lib_Installed()
{
    // execute remote command whereis libbcm2835
        // response should be: /usr/local/lib/libbcm2835.a
    // execute remote command find /usr/local/lib -name '*bcm2835*'
        // response should be: /usr/local/lib/libbcm2835.a
    printf(COLOR_CYAN "\n\tVERIFYING BCM2835 LIBRARY IS PRESENT ...\n" COLOR_RESET);
    char* response = this->ssh->Exec_Remote_Shell_Command("whereis libbcm2835");
    bool installed1 = StringH::String_Contains(response, "libbcm2835.a");
    if( response != NULL)
        free(response);

    response = ssh->Exec_Remote_Shell_Command("find /usr/local/lib -name '*bcm2835*'");
    bool installed2 = StringH::String_Contains(response, "libbcm2835.a");
    if( response != NULL)
        free(response);

    if(installed1 || installed2)
    {
        printf(COLOR_GREEN "SUCCESS \n" COLOR_RESET);
        return true;
    }
    else
    {
        printf(COLOR_RED "FAILURE \n" COLOR_RESET);
        return false;
    }
}


void Configure::Install_Lib_bcm2835()
{
    printf(COLOR_CYAN "\t--INSTALLING BCM2835 LIBRARY ON PI--\n" COLOR_RESET);
    // send script to automatically install the library
    ssh->SCP_Write_Remote_File("./src/Configure/install_bcm2835.sh", "/home/pi/Downloads/install_bcm2835.sh", S_IRUSR |  S_IWUSR | S_IXUSR );
    char* response = ssh->Exec_Remote_Shell_Command("/home/pi/Downloads/install_bcm2835.sh > /home/pi/Downloads/install_log.txt");
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }

    // Delete Install Files
    response = ssh->Exec_Remote_Shell_Command("rm /home/pi/Downloads/config.h");
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }

    response = ssh->Exec_Remote_Shell_Command("rm /home/pi/Downloads/Makefile");
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }

    response = ssh->Exec_Remote_Shell_Command("rm /home/pi/Downloads/bcm2835*.gz");
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }
    printf("\n");
}

void Configure::Install_Lib(char* libname)
{
    // sudo apt install libname
    // if cant find library do sudo apt update then retry
    char install_command[200];
    StringH::Erase_Num_Chars(install_command, 200);
    sprintf(install_command, "sudo apt install -y %s", libname);

    char* response = this->ssh->Exec_Remote_Shell_Command(install_command);
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }
    printf("\n");
}

void Configure::Compile_PI_DAQ_Program()
{
    printf(COLOR_CYAN "\n\tCOMPILING PI DAQ PROG ...\n" COLOR_RESET);
    char compile_command[200];
    StringH::Erase_Num_Chars(compile_command, 200);
    sprintf(compile_command, "make -C /home/pi/firmware/");

    char* response = this->ssh->Exec_Remote_Shell_Command(compile_command);
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }

    printf(COLOR_CYAN "\t--DAQ PROGRAM COMPLETE--\n" COLOR_RESET);
    printf("\n");
}


void Configure::Reload_and_Start_DAQ_Server()
{

    
    char* response = this->ssh->Exec_Remote_Shell_Command("sudo stop Blackberry.service");
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }
    
    this->Compile_PI_DAQ_Program();

    printf(COLOR_CYAN "\tRESTARTING DAQ SERVER ...\n" COLOR_RESET);
    response = this->ssh->Exec_Remote_Shell_Command("sudo systemctl daemon-reload");
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }
    
    response = this->ssh->Exec_Remote_Shell_Command("sudo start Blackberry.service");
    if(response != NULL)
    {
        printf(COLOR_YELLOW "%s\n" COLOR_RESET, response);
        free(response);
    }
}