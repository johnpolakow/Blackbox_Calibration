
#ifndef _PI_H
#define _PI_H

#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/ioctl.h>


#include "Struct_Defs.h"
#include "StringH.h"
#include "Pi_Socket.h"
#include "TimeH.h"
#include "Test.h"
#include "Data.h"
#include "AC_Data.h"
#include "colors.h"
#include "Cal_Parameters.h"

const char SEARCH_ETHERNET[] =  "sudo arp-scan --localnet -g -I eth0 | grep \"Raspberry\"";
const char SEARCH_WIFI[] = "sudo arp-scan --localnet -g -I wlan0 | grep -e 'Raspberry'\0";

#define SERVER_PORT "2020"
#define RECEIVE_BUFFER_SIZE 5000

#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )    // macro to get number of items in an object array


// Data Structure class to hold measurements read from Pi
class PI_Data_Point
{
public:
  PI_Data_Point()
  {
      // initialize all values to -1. Not all values may be assigned
      sample_number = -1;
      DAC_count     = -1;
      ADC0_CoolV    = -1;
      ADC0_CoolA    = -1;
      ADC1_CoolA    = -1;
      ADC0_DiodeV   = -1;
      ADC1_LoadV    = -1;
      ADC1_LoadmA   = -1;
  }

    Time time;
    Date date;
    int sample_number;
    int DAC_count;

    double ADC0_CoolV;
    double ADC0_CoolA;
    double ADC1_CoolA;
    double ADC0_DiodeV;
    double ADC1_LoadV;
    double ADC1_LoadmA;
};


// constructor of PI automatically turns off the cooler relay and sets load current to 0
class PI
{
public:
    PI()
    {
      this->pi_sock_addr = -1;
      this->connected = false;

      Find_ETH_IP_and_MAC_Addr();

      this->Connect_to_PI( this->ETH_ADDR, SERVER_PORT );
      usleep(ONE_MILLISECOND*10);

      this->Cal_Mode_ON();
      this->Set_DAC_Count(0);
      this->Relay_OFF();
      this->Cal_Mode_OFF();

      pi_data_index = 0;
      pi_Data = NULL;
    }

    bool Is_Connected();
    int Connect_to_PI( const char* server_IP, const char* server_Port );
    void PI_Send( const char* send_string);
    char* PI_Receive( );
    void PI_Wait_Receive();
    static void Free_PI_Data_Memory(PI_Data_Point** data);

    void Relay_ON( );
    void Relay_OFF( );
    void Cal_Mode_ON();
    void Cal_Mode_OFF();
    void Verbose_Mode_OFF();
    char* Read_Metrics( );

    void Set_DAC_Count(int DAC_count);
    void Set_Read_Seq_Load_ON();
    void Set_Read_Seq_Load_OFF();
    void Set_Read_Seq_POWER_CALIBRATION();

    double Get_Coldfinger_Temp( );
    void Erase_String(char* target_string);

    char* Get_Wifi_IP();
    void Find_ETH_IP_and_MAC_Addr();
    char* Get_ETH_IP();
    char* Get_ETH_MAC();
    int Get_Socket_Address();
    static char** Exec_Shell_Command(const char* command);

    PI_Data_Point** Parse_PI_Data();
    static Data_Group* Extract_Load_V_Data(PI_Data_Point** pi_data);
    static Data_Group* Extract_Load_mA_Data(PI_Data_Point** pi_data);
    static Data_Group* Extract_Cooler_V_Data(PI_Data_Point** pi_data);
    static Data_Group* Extract_Cooler_mA_Data(PI_Data_Point** pi_data);
    static AC_Data_Group* Extract_AC_mA_Data(PI_Data_Point** pi_data);
    static Data_Group* Extract_Diode_V_Data(PI_Data_Point** pi_data);
    PI_Data_Point** pi_Data;

private:
    char* Receive_nBytes( int numBytesToRead );
    PI_Data_Point* Parse_Sample(char* pi_string);
    void Add_Sample_To_Data_Array( PI_Data_Point* new_data_point);
    void Free_Mem();
    int String_Array_Num_Strings(char** string_array);
    void Query_Wifi(char* wifi_line);
    void Query_ETH(char* eth_line);

    const int PI_MAX_DATA_POINTS = 20;
    char complete_receive_string[RECEIVE_BUFFER_SIZE];
    int pi_data_index;
    int pi_sock_addr;
    bool connected;

    char ETH_ADDR[20];
    char ETH_MAC[20];
    char WIFI_ADDR[20];
    char WIFI_MAC[20];
};

#endif
