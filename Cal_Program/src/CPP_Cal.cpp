#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>     //UNIX Standard function definitions
#include <fcntl.h>      //File control
#include <errno.h>      //Error number def
#include <termios.h>    //POSIX terminal control
#include <termio.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <libssh/libssh.h>
#include <errno.h>
#include <string.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h> 

#ifdef __arm__
    #include <bcm2835.h>
#endif

#include "PI/Pi.h"

#include "AutoCal.h"
#include "HP34401/HP_34401.h"
#include "PI/relay.h"
#include "HP34401/HP_Measurement_Functions.h"
#include "Calibrate.h"
#include "./Yokogawa/WT300E.h"
#include "./Write_Log.h"
#include "./E3648A/E36468.h"
#include "Menu.h"
#include "USB_Helper.h"
#include "Cal_Parameters.h"
#include "SSH_Helper.h"
#include "StringH.h"
#include "Net_Interface.h"
#include "Cal_Parameters.h"
#include "colors.h"
#include "Configure.h"



// External global variables. Defined in Cal_Parameters.cpp
extern const char* HP_A_ID;
extern const char* HP_B_ID;
extern const int DAC_INCREMENT;
const char* serverPort = DEFAULT_SERVER_PORT;


void Upload_Cal_Files();
void Automatic_Calibration(PI* pi, WT300E* yokogawa, Meter* HP34401A, Meter* HP34401B, PowerSupply* E3648A, Write_Log* FileWrite);

void Automatic_Calibration(PI* pi, WT300E* yokogawa, Meter* HP34401A, Meter* HP34401B, PowerSupply* E3648A, Write_Log* FileWrite)
{
    // following quantites of Cal Runs:
    // Thermocouples    4
    // REF100           3
    // Diode Volts      2
    // Cooler mA        6
    // Cooler Volts DC  3
    // Load Increments  5
    
    Calibrate_Thermocouples(pi, FileWrite);                 
    printf(COLOR_BOLD_RED "\nTEST#1 REF100    1 minute\n\n" COLOR_RESET);
    Calibrate_REF100(HP34401A, HP34401B, FileWrite);

    printf(COLOR_BOLD_RED "\nTEST#2 DIODE VOLTS    40 minutes\n\n" COLOR_RESET);
// cable change
    Change_Cables_Diode_Volts();
    Auto_Calibrate_Diode_Volts(pi, HP34401A, E3648A, FileWrite); 
    usleep(ONE_MILLISECOND*1000);
    printf(COLOR_BOLD_RED "\nTEST#3 COOLER mA   8 minutes\n\n" COLOR_RESET);
    Auto_AC_Power_Calibration(pi, yokogawa, FileWrite);        
    usleep(ONE_MILLISECOND*1000);


    printf(COLOR_BOLD_RED "\nTEST#4 COOLER VDC  11 minutes\n\n" COLOR_RESET);
// cable change
    Change_Cables_Cooler_V_DC();
    Auto_Calibrate_Cooler_V_DC(pi, HP34401A, HP34401B, E3648A, FileWrite);
    usleep(ONE_MILLISECOND*1000);


    printf(COLOR_BOLD_RED "\nTEST#5 COOLER mA   8 minutes\n\n" COLOR_RESET);
// cable change
    Change_Cables_Cooler_mA();
    Auto_AC_Power_Calibration(pi, yokogawa, FileWrite);
    printf(COLOR_BOLD_RED "\nTEST#6 REF100\n\n" COLOR_RESET);
    usleep(ONE_MILLISECOND*1000);
    Calibrate_REF100(HP34401A, HP34401B, FileWrite);
    usleep(ONE_MILLISECOND*1000);
    printf(COLOR_BOLD_RED "\nTEST#7 LOAD INCREMENTS   25 minutes\n\n" COLOR_RESET);
    Auto_Calibrate_Load_Increments(pi, HP34401A, HP34401B, DAC_INCREMENT, FileWrite); // load mA & V calibration point every 500 counts
    usleep(ONE_MILLISECOND*1000);
    Calibrate_Thermocouples(pi, FileWrite);                
 
    usleep(ONE_MILLISECOND*1000);
    printf(COLOR_BOLD_RED "\nTEST#8 COOLER mA  8 minutes\n\n" COLOR_RESET);
    Auto_AC_Power_Calibration(pi, yokogawa, FileWrite);
    usleep(ONE_MILLISECOND*1000);
    Calibrate_Thermocouples(pi, FileWrite);   

    printf(COLOR_BOLD_RED "\nTEST#9 COOLER VDC   11 minutes\n\n" COLOR_RESET);
// cable change
    Change_Cables_Cooler_V_DC();
    Auto_Calibrate_Cooler_V_DC(pi, HP34401A, HP34401B, E3648A, FileWrite);
    usleep(ONE_MILLISECOND*1000);


    printf(COLOR_BOLD_RED "\nTEST#10 COOLER mA   8 minutes\n\n" COLOR_RESET);
// cable change
    Change_Cables_Cooler_mA();
    Auto_AC_Power_Calibration(pi, yokogawa, FileWrite);
    usleep(ONE_MILLISECOND*1000);

    printf(COLOR_BOLD_RED "\nTEST#11 LOAD INCREMENTS    25 minutes\n\n" COLOR_RESET);
    Auto_Calibrate_Load_Increments_Reverse(pi, HP34401A, HP34401B, DAC_INCREMENT+100, FileWrite);
    usleep(ONE_MILLISECOND*1000);

    printf(COLOR_BOLD_RED "\nTEST#12 LOAD INCREMENTS    25 minutes\n\n" COLOR_RESET);
    Auto_Calibrate_Load_Increments(pi, HP34401A, HP34401B, DAC_INCREMENT+200, FileWrite);
    usleep(ONE_MILLISECOND*1000);

    printf(COLOR_BOLD_RED "\nTEST#13 LOAD INCREMENTS    25 minutes\n\n" COLOR_RESET);
    Auto_Calibrate_Load_Increments_Reverse(pi, HP34401A, HP34401B, DAC_INCREMENT+300, FileWrite);
    usleep(ONE_MILLISECOND*1000);


    printf(COLOR_BOLD_RED "\nTEST#14 DIODE VOLTS    40 minutes\n\n" COLOR_RESET);
// cable change
    Change_Cables_Diode_Volts();
    Auto_Calibrate_Diode_Volts(pi, HP34401A, E3648A, FileWrite);
    usleep(ONE_MILLISECOND*1000);
    Calibrate_Thermocouples(pi, FileWrite);   
    usleep(ONE_MILLISECOND*1000);

    printf(COLOR_BOLD_RED "\nTEST#15 REF100\n\n" COLOR_RESET);
    Calibrate_REF100(HP34401A, HP34401B, FileWrite);
    usleep(ONE_MILLISECOND*1000);

    printf(COLOR_BOLD_RED "\nTEST#16 COOLER mA    8 minutes\n\n" COLOR_RESET);
    Auto_AC_Power_Calibration(pi, yokogawa, FileWrite);
    usleep(ONE_MILLISECOND*1000);

    printf(COLOR_BOLD_RED "\nTEST#17 LOAD INCREMENTS     25 minutes\n\n" COLOR_RESET);
    Auto_Calibrate_Load_Increments(pi, HP34401A, HP34401B, DAC_INCREMENT+500, FileWrite);

    printf(COLOR_BOLD_RED "\nTEST#18 COOLER VDC     11 minutes\n\n" COLOR_RESET);
// cable change
    Change_Cables_Cooler_V_DC();
    Auto_Calibrate_Cooler_V_DC(pi, HP34401A, HP34401B, E3648A, FileWrite);
}

void Manual_Calibration(PI* pi, WT300E* yokogawa, Meter* HP34401A, Meter* HP34401B, PowerSupply* E3648A, Write_Log* FileWrite)
{
    while(true)
    {
        int selection = Menu::Get_Calibration_Selection();

        switch(selection)
        {
            case CAL_COOLER_DCV:
                Calibrate_Cooler_V_DC(pi, HP34401A, HP34401B, E3648A, FileWrite);   // defined in "Calibrate.h"
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case CAL_COOLER_AC_MA:
                AC_Power_Calibration(pi, yokogawa, FileWrite);          // defined in Calibrate.h
                continue;
            case CAL_REF100:
                Calibrate_REF100(HP34401A, HP34401B, FileWrite);           // defined in Calibrate.h
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case CAL_LOAD:
                Calibrate_Load_Increments(pi, HP34401A, HP34401B, DAC_INCREMENT, FileWrite); // load mA & V calibration point every 500 counts
                continue;
            case CAL_DIODE:
                Calibrate_Diode_Volts(pi, HP34401A, E3648A, FileWrite);    // defined in Calibrate.h
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case CAL_THERMOCOUPLES:
                Calibrate_Thermocouples(pi, FileWrite);    // defined in Calibrate.h
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case CAL_EXIT:
                return;
            case SEL_ERROR:
                continue;
        }
    }
}

void LUT_Menu(Create_LUT* LUT_creator)
{
    while(true)
    {
        int selection = Menu::Get_LUT_Selection(LUT_creator);
        char* log_filepath;
        bool move_log_file = false;

        char LUT_filepath[200];
        StringH::Erase_Num_Chars(LUT_filepath,200);
        strcpy(LUT_filepath, LUT_creator->LUT_DIR);

        switch(selection)
        {
            case SEL_COOLER_VOLTS:
                strcat(LUT_filepath, COOLER_V_LUT);
                log_filepath = LUT_creator->Get_Cooler_V_Log_File();
                if(log_filepath == NULL )
                {
                    printf("Cooler Volts Log file not found.\n\n");
                    continue;
                }

                printf(COLOR_LIGHT_MAGENTA "\tProcessing Cooler Volts LUT\n\n" COLOR_RESET);
                LUT_creator->Process_DC_LUT(COOLER_V_t, log_filepath, LUT_filepath);
                free(log_filepath);
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case SEL_COOLER_MILLIAMPS:
                log_filepath = LUT_creator->Get_Cooler_mA_Log_File();
                if(log_filepath == NULL )
                {
                    printf("Cooler milliAmps Log file not found.\n\n");
                    continue;
                }

                printf(COLOR_LIGHT_MAGENTA "\tProcessing Cooler milliAmps LUT\n\n" COLOR_RESET);
                strcat(LUT_filepath, COOLER_MA_LUT);
                LUT_creator->Process_AC_LUT(COOLER_MA_t, log_filepath, LUT_filepath);
                free(log_filepath);
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case SEL_LOAD_MILLIAMPS:

                log_filepath = LUT_creator->Get_Load_mA_Log_File();
                if(log_filepath == NULL )
                {
                    printf("Load milliAmps Log file not found.\n\n");
                    continue;
                }

                printf(COLOR_LIGHT_MAGENTA "\tProcessing LOAD milliAmps LUT\n\n" COLOR_RESET);
                strcpy(LUT_filepath, LUT_creator->LUT_DIR);
                strcat(LUT_filepath, DAC_COUNT_LUT);
                move_log_file = false;
                LUT_creator->Process_DAC_LUT(DAC_COUNT_t, log_filepath, LUT_filepath, move_log_file);

                strcpy(LUT_filepath, LUT_creator->LUT_DIR);
                strcat(LUT_filepath, LOAD_MA_LUT);
                move_log_file = true;
                LUT_creator->Process_DAC_LUT(LOAD_MA_t, log_filepath, LUT_filepath, move_log_file);  //boolean value is whether to move the log file to processed dir after completion
                free(log_filepath);
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case SEL_LOAD_VOLTS:
                log_filepath = LUT_creator->Get_Load_V_Log_File();
                if(log_filepath == NULL )
                {
                    printf("Load Volts Log file not found.\n\n");
                    continue;
                }

                printf(COLOR_LIGHT_MAGENTA "\tProcessing LOAD Volts LUT\n\n" COLOR_RESET);
                strcat(LUT_filepath, LOAD_V_LUT);
                LUT_creator->Process_DC_LUT(LOAD_V_t, log_filepath, LUT_filepath);
                free(log_filepath);
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case SEL_DIODE_VOLTS:
                log_filepath = LUT_creator->Get_Diode_V_Log_File();
                if(log_filepath == NULL )
                {
                    printf("Diode Volts Log file not found.\n\n");
                    continue;
                }
                
                printf(COLOR_LIGHT_MAGENTA "\tProcessing Diode Volts LUT\n\n" COLOR_RESET);
                strcat(LUT_filepath, DIODE_V_LUT);
                LUT_creator->Process_DC_LUT(DIODE_V_t, log_filepath, LUT_filepath);
                free(log_filepath);
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case PROCESS_ALL_LUT:
                printf(COLOR_LIGHT_MAGENTA "\tProcessing All LUT Files\n\n" COLOR_RESET);
                LUT_creator->Process_All_LUT_Files();
                Menu::Wait_Enter();
                Menu::Clear_Console_Screen();
                continue;
            case UPLOAD_CAL_FILES:
                Upload_Cal_Files();
                Menu::Wait_Enter();
                continue;
            case LUT_EXIT:
                return;
            case LUT_SEL_ERROR:
                continue;
        }
    }
}

void Upload_Cal_Files()
{
    printf("\tUPLOAD CALIBRATION FILES & BULD PI APPLICATION\n");
    char REMOTE_HOST_IP[20];
    char REMOTE_HOST_MAC_ADDR[20];
    Net_Interface* local_interfaces = new Net_Interface();
    remote_pi** remote_pis = local_interfaces->Find_Connected_PIs();    // filters out pis found with MAC address of Calibration Box
    //Net_Interface::Print_Connected_Pi_List(remote_pis);     // print raspberry pis attached to the local network 

    if(remote_pis != NULL)
    {
        remote_pi* connect_pi = remote_pis[0];  // select the first item in the list to connect to
        strncpy(REMOTE_HOST_IP, connect_pi->pi_ip_addr, 20);
        strncpy(REMOTE_HOST_MAC_ADDR, connect_pi->pi_mac_addr, 20);

    }
    Net_Interface::Free_Mem_Pi_List(remote_pis);
    delete local_interfaces;

    if(!(strlen(REMOTE_HOST_IP)) > 1)
    {   
        printf("Could not find Pi to connect to\n");
        printf("Exiting....\n");
        exit(-1);
    }

    // user and password are defined in "Cal_Parameters.h"
    SSH_Helper* ssh = new SSH_Helper(REMOTE_HOST_IP, REMOTE_HOST_MAC_ADDR, REMOTE_SSH_USER, REMOTE_SSH_PASSWORD);
    ssh->Log_In_To_Host();
    bool remote_ssh_is_configured = ssh->Remote_Pi_Has_SSH_dir_and_Authorized_Keys_File();

    char* local_ssh_keys = ssh->Read_Local_SSH_Keys();  // read our local SSH keys
    char* remote_keyfile_contents = ssh->SCP_Read_Remote_File(REMOTE_KEYS_FILE);   // REMOTE_KEYS_FILE defined in "Cal_Parameters.h"
    ssh->Verify_Remote_Host_Has_Our_SSH_Keys(remote_keyfile_contents, local_ssh_keys);

    free(local_ssh_keys);
    free(remote_keyfile_contents);

    char* remote_host_mac_address = ssh->Get_Remote_Host_MAC_Address();
    char* local_LUT_dir = FileH::Get_Local_LUT_Dir_for_MAC_Address(remote_host_mac_address);

    char** LUT_Send_list = ssh->Get_Local_LUT_Files(local_LUT_dir);  //wrap this func in SSH Helper
    free(local_LUT_dir);
    free(remote_host_mac_address);

    ssh->Create_Remote_LUT_Directories();
    ssh->Send_Lookup_Tables_To_DUT(LUT_Send_list);
    StringH::Free_String_Array_Memory(LUT_Send_list);



    Configure* config = new Configure(ssh);

    bool bcm_library_is_installed = config->Check_bcm2835_Lib_Installed();
    if( !bcm_library_is_installed )
        config->Install_Lib_bcm2835();

    config->Install_Lib_bcm2835();
    config->Reload_and_Start_DAQ_Server();

    usleep(ONE_MILLISECOND*10);
    bool is_running = ssh->Remote_DAQ_Process_Is_Running();
    if( !is_running )
    {
        ssh->Restart_Remote_DAQ_Process();   // this function takes 30 seconds to return
    }

    ssh->Disconnect();
}


int main( int argc, char** argv )
{
  Initialize_Relays();                                // defined in Relays.h

  PI* pi = new PI();    // constructor of PI turns off cooler relay and turns off load current
  char* pi_eth_MAC = pi->Get_ETH_MAC(); // gets Ethernet Address of PI DUT
  Write_Log* FileWrite = new Write_Log(pi_eth_MAC);
  free(pi_eth_MAC);

  Meter HP34401A;
  Meter HP34401B;
  StringH::Erase_Num_Chars(HP34401A.device_path, 255);
  StringH::Erase_Num_Chars(HP34401B.device_path, 255);
  HP34401A.file_descriptor = -1;
  HP34401B.file_descriptor = -1;

  PowerSupply E3648A;
  E3648A.file_descriptor = -1;      // initialize device descriptor to an invalid number, so we know once we get a valid file descriptor
  StringH::Erase_Num_Chars(E3648A.device_path, 255);    // initialize device path to empty, until it is set
  printf("\n---------------------------------\n\n");

  USB_Device** dev_list = USB::Search_ttyUSB_Devices(); // searches the linux device tree for ttyUSB[x] using shell command: "ls /dev | grep ttyUSB"
                                                        // opens serial port to device, and queries it for mfr, ID string, and Model

  if( dev_list == NULL)
  {
      printf("Searching for ttyUSB meters....\tNone\n");
      printf(COLOR_BOLD_RED "--NO USB METERS PRESENT--\n" COLOR_RESET);
      printf(COLOR_BOLD_RED "CANNOT RUN CAL. exiting\n" COLOR_RESET);
      exit(0);
  }

  int index = 0;
  while( dev_list[index] != NULL)   // process each device returned from the 'Search_ttyUSB_Devices()' command
  {                                 // if the device ID string matches one in our pre-defined list, we assign that device to its proper class object
                                    // possible models are HP34401A, HP34401B, and E3648A
      USB_Device* dev = dev_list[index];
      char* ID = dev->ID_string;
      char* mfr = dev->manufacturer;
      char* model = dev->model;

      if( StringH::String_Contains_Ignore_Case(mfr, "HEWLETT-PACKARD"))
      {
          // device must match one of two IDs. We know the IDs of the HP meters already
          if( StringH::String_Contains_Ignore_Case(ID, HP_A_ID))        // if the USB device matches the HP_A ID, which is "0,11-5-2"
          {                                                                 
              HP34401A.file_descriptor = dev->file_descriptor;          // file descriptor and path are recorded, and its ID is queried
              strcpy(HP34401A.device_path, dev->device_path);
              strcpy(HP34401A.text, "--HP_A--");        // text to show on meter display to identify it
              strcpy(HP34401A.model, dev->model);
              strcpy(HP34401A.manufacturer, dev->manufacturer);
              strcpy(HP34401A.meter_string, "HP34401A");    // description of meter for use in printf statements
              HP34401A.configuration.meas_type = VOLTS_DC;
              HP34401A.configuration.sample_count = 2;
              HP34401A.configuration.sample_count = 5;
              Initialize_Meter(&HP34401A);
          }
          else if( StringH::String_Contains_Ignore_Case(ID, HP_B_ID))   // if the USB device matches the HP_B ID, which is "0,2-1-1"
          {
              HP34401B.file_descriptor = dev->file_descriptor;
              strcpy(HP34401B.device_path, dev->device_path);
              strcpy(HP34401B.text, "--HP_B--");        // text to show on meter display to identify it
              strcpy(HP34401B.model, dev->model);
              strcpy(HP34401B.manufacturer, dev->manufacturer);
              strcpy(HP34401B.meter_string, "HP34401B");    // description of meter for use in printf statements
              HP34401A.configuration.meas_type = VOLTS_DC;
              HP34401A.configuration.sample_count = 2;
              HP34401A.configuration.sample_count = 5;
              Initialize_Meter(&HP34401B);
          }
      }
      else if( StringH::String_Contains_Ignore_Case(mfr, "AGILENT"))
      {
        // assign info from this device to E3648A
        E3648A.file_descriptor = dev->file_descriptor;
        strcpy(E3648A.device_path, dev->device_path);
        strncpy(E3648A.model, dev->model, 20);
        strncpy(E3648A.PS_ID, dev->ID_string, 25);
        strncpy(E3648A.manufacturer, dev->manufacturer, 30);
        strncpy(E3648A.model, dev->model, 20);
        strcpy(E3648A.text, "--E3648A--");        // text to show on meter display to identify it
        Initialize_PowerSupply(&E3648A);
      }
      ++index;
  }

  USB::Free_Mem_Device_List(dev_list);  // free memory dynamically allocated to hold objects from the 'Search_ttyUSB_Devices()' command

  if( !( HP34401A.file_descriptor > 0 && HP34401B.file_descriptor > 0 ) )   // if file descriptors haven't been assigned to BOTH HP meters, throw an error
  {
      printf( COLOR_BOLD_RED "\nDID NOT DETECT BOTH HP34401 METERS\n" COLOR_RESET);
      if( HP34401A.file_descriptor < 0)
         printf("\t(" COLOR_BOLD_RED "HP_A" COLOR_RESET " not detected)\n");
      if( HP34401B.file_descriptor < 0)
         printf("\t(" COLOR_BOLD_RED "HP_B" COLOR_RESET " not detected)\n");
      printf("\n CANNOT CONTINUE WITH CALIBRATION\n");
      exit(0);
  }
  if( !(E3648A.file_descriptor > 0) )                       // if E3648A file descriptor hasnt been assigned (could not connect to the device), throw an error
  {
      printf( COLOR_BOLD_RED "\nDID NOT DETECT AGILENT POWER SUPPLY\n" COLOR_RESET);
      printf("\n CANNOT CONTINUE WITH CALIBRATION...  exiting\n");
      exit(0);
  }



  // Open communications with WT310E
  WT300E* yokogawa = new WT300E( );
  yokogawa->Initialize_Meter();

  //BK->Zero_Cal();

  int mode = Menu::User_Sel_Auto_or_Manual();

  if( mode == CAL_AUTOMATIC )
  {
    Automatic_Calibration(pi, yokogawa, &HP34401A, &HP34401B, &E3648A, FileWrite);
  }
  else if( mode == CAL_MANUAL )
  {
    Manual_Calibration(pi, yokogawa, &HP34401A, &HP34401B, &E3648A, FileWrite);
  }

// print: these calibration log files were created: [], for board with MAC address and IP Address:
// calibration log files are located here :......
// to create Calibration Lookup Tables from the log files, run this command: .....
// to upload the Lookup Tables to the PI, use this command: 


  //HP34401_Display_Text(&HP34401A, "  --DONE--");
  //HP34401_Display_Text(&HP34401B, "  --DONE--");

  close(HP34401A.file_descriptor);
  close(HP34401B.file_descriptor);
  close(E3648A.file_descriptor);
  delete yokogawa;




  char* log_file_location = FileWrite->Get_Log_Dir();
  char* DUT_mac_address = FileWrite->Get_MAC_Addr();
  Menu::Clear_Console_Screen();
  Create_LUT* LUT_creator = new Create_LUT(log_file_location, DUT_mac_address);
  free(log_file_location);
  free(DUT_mac_address);


// Menu to Select LUT processing
  LUT_Menu(LUT_creator);


  


  return 0;
}
