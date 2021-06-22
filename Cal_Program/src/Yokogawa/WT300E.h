#ifndef WT300E_H
#define WT300E_H

// BK_5535.h - Support for BK5535 power meter in Linux device tree
// ========================================================================================
// uses the /dev/usbtmc interface built into linux

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "TStamp.h"
#include "StringH.h"
#include "colors.h"
#include "Yoko_Metric.h"
//#include "Pi.h"

#define TIME_DELAY 3000

// Error codes.
#define RDS_ERR_OK          0
#define RDS_ERR_CANTOPEN    1
#define RDS_ERR_CANTMALLOC  2
#define RDS_ERR_CMDSYNTAX   3
#define RDS_ERR_CANTWRITE   4
#define RDS_ERR_NOTOPEN     5
#define RDS_ERR_CANTREAD    6
#define RDS_ERR_QUERYSYNTAX 7
#define RDS_NUM_ERR         8 // Count

// Error return values.
#define RDS_IERRVAL (-99999999)
#define RDS_FERRVAL (-9999999.9f)

#define ITEM_VOLTAGE            1
#define WT300E_VOLTAGE          "U"

#define ITEM_CURRENT            2
#define WT300E_CURRENT          "I"

#define ITEM_ACTIVE_POWER       3
#define WT300E_ACTIVE_POWER     "P"

#define ITEM_APPARENT_POWER     4
#define WT300E_APPARENT_POWER   "S"

#define ITEM_REACTIVE_POWER     5
#define WT300E_REACTIVE_POWER    "Q"

#define ITEM_POWER_FACTOR       6
#define WT300E_POWER_FACTOR      "LAMBDA"


class WT300_Measurement
{
public:
    WT300_Measurement(int item_no, std::string sym)
    {
        this->item_number = item_no;
        this->symbol = sym;
        this->set_command = ":NUM:NORM:ITEM" + std::to_string(this->item_number) + " " + this->symbol, + ",1,TOT";
        this->query_value = ":NUM:NORMAL:VAL? " + std::to_string(this->item_number);
    }

    int item_number;
    std::string symbol;
    std::string set_command;
    std::string query_value;
};


class WT300E
{
public:
  WT300E( );
  ~WT300E();

  char* Send( const char* text );  // Send text commands to the meter
  char* Send_Cmd(const char* cmd_string);
  bool Write_No_Reply( const char* text_query );
  char* Read();
  void WaitForAcqComplete();

  double Get_Voltage();
  double Get_Current();
  double Get_Active_Power();
  double Get_Total_Power();
  double Get_Reactive_Power();
  double Get_Power_Factor();

  char* Get_ID();
  void Zero_Cal();
  char* Search_USBTMC();
  bool Is_Connected();
  void Initialize_Meter();
  void Set_Remote();
  void Set_ASCII_Output();
  void Set_Verbose_OFF();
  void Set_Overlap_Commands_OFF();
  void Set_Headers_OFF();
  void Set_Input_Mode_RMS();
  void Set_Line_Filter_ON();
  void Set_Volt_Range_Auto();
  void Set_Current_Range_2A();
  void Set_Current_Range_1A();
  void Set_Resolution_5_Digits();
  void Set_Averaging_ON();
  void Set_Averaging_OFF();
  void Set_Averaging_Linear();
  void Set_Averaging_Count_8();
  void Set_Sample_Rate_100_ms();
  void Set_Sample_Rate_250_ms();
  void Set_Commands_WT300E();
  void Print_Error_Queue();
  void Clear();
  void Set_Item_Metric_Assignments();
  void Reset_Meter();
  void Parse_ID_String(char* id_string);
  int Get_Num_Chars_Waiting(int port_FD);


  Yoko_sample_t*  Measure_RMS_Volts();
  Yoko_sample_t*  Measure_RMS_Current();
  Yoko_sample_t*  Measure_Active_Power();
  Yoko_sample_t*  Measure_Apparent_Power();
  Yoko_sample_t*  Measure_Reactive_Power();
  Yoko_sample_t*  Measure_Power_Factor();


private:
  void Average_Data_Points();

  int lasterr;               // Last error code.
  bool connected;            // Device /dev/usbtmcN opened OK.
  int FD;                    // File descriptor for device i/o.
  int io_count;              // I/o transfer count.
  char write_buffer[2048];   // Multi-purpose buffer.
  int device_number;         // Integer number for /dev/usbtmcN. Could be usbtmc1 or usbtmc0
  char device_path[20];

  char message[1024];        // Diagnostic message buffer.
  char command[100];         // holds string commands to the meter

  WT300_Measurement* volt_measurement;
  WT300_Measurement* current_measurement;
  WT300_Measurement* active_power_measurement;
  WT300_Measurement* apparent_power_measurement;
  WT300_Measurement* reactive_power_measurement;
  WT300_Measurement* power_factor_measurement;
};

#endif
