#ifndef BK5335_H
#define BK5335_H

// BK_5535.h - Support for BK5535 power meter in Linux device tree
// ========================================================================================
// uses the /dev/usbtmc interface built into linux

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "TStamp.h"
#include "StringH.h"
#include "BK_Metric.h"
#include "colors.h"
#include "Pi.h"

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

class BK5335
{
public:
  BK5335( );
  virtual ~BK5335();

  void notify( const char* fmt, ... );
  virtual void notify_out( void );
  const char* error_string( void );

  char* Send( const char* text );  // Send text commands to the meter
  char* Send_Cmd(const char* cmd_string);
  bool Write_No_Reply( const char* text_query );
  char* Read();

  char* Get_ID();
  void Turn_Beep_Off();
  void Set_Continuous_Trigger();
  void Zero_Cal();
  char* Search_USBTMC();
  bool Is_Connected();
  void Initialize_Meter();

  BK5335_sample_t*  Measure_DC_Volts();
  BK5335_sample_t*  Measure_AC_Volts();
  BK5335_sample_t*  Measure_RMS_Volts();
  BK5335_sample_t*  Measure_Mean_Volts();

  BK5335_sample_t*  Measure_AC_Current();
  BK5335_sample_t*  Measure_DC_Current();
  BK5335_sample_t*  Measure_RMS_Current();
  BK5335_sample_t*  Measure_Peak_Current();

  BK5335_sample_t*  Measure_Active_Power();
  BK5335_sample_t*  Measure_Apparent_Power();
  BK5335_sample_t*  Measure_Reactive_Power();
  BK5335_sample_t*  Measure_Power_Factor();
  BK5335_sample_t*  Measure_Phase_Angle();

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
};

#endif
