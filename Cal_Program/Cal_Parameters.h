

#ifndef PARAMETERS_H
#define PARAMETERS_H


#include <libssh/libssh.h>

// These are declarations of global variables defined in Cal_Parameters.cpp
// Definition of Raspberry PI Pin #s are in ./src/PI/Pin_Defs.h
// to compile the project, enter the command "make" in the Cal_Program directory
// default IP address of the Calibration Box is 192.168.123.7, setting is in /etc/dhcpcd.conf 


#define POPEN_READ                   "re"  // "r" opens for reading, "e" sets close on exec flag ("FD_CLOEXEC")
#define POPEN_WRITE                   "w"  // "r" opens for reading


extern const char* CAL_STATION_MAC_ADDR;

// PARAMETERS FOR SSH CODE
//////////////////////////////////////////////////////////////////
extern const int SSH_VERBOSITY;
extern const char* REMOTE_PI_CAL_FILES_DIR;
extern const char* REMOTE_PI_FIRMWARE_SRC_DIR;
extern const char* REMOTE_SSH_USER;
extern const char* REMOTE_SSH_PASSWORD;
extern const char* REMOTE_KEYS_FILE;
extern const int SSH_PORT;


extern const char* DAQ_PROCESS_SERVICE_NAME;



//  PARAMETERS FOR CALIBRATION PROCEDURE CODE
//////////////////////////////////////////////////////////////////
extern const char* DEFAULT_SERVER_PORT;
extern const char CAL_LOG_DIR[];

extern const char* HP_A_ID;
extern const char* HP_B_ID;
extern const char* AGILENT_PS_ID;

extern double DCV_Set_Points[];
extern int V_POINTS_NUM_ELEMENTS;

extern double Diode_Cal_Points[];
extern int DIODE_POINTS_NUM_ELEMENTS;

extern const int DAC_INCREMENT;
/////////////////////////////////////////////////////////////////



// PARAMETERS FOR DATA FILTER AND LUT CREATION
/////////////////////////////////////////////////////////////////

extern const char* ROOT_LOG_DIR;
extern const char* ROOT_PROCESSED_DIR;
extern const char* ROOT_LUT_DIR;

extern const char* COOLER_MA_LUT;
extern const char* COOLER_V_LUT;
extern const char* LOAD_V_LUT;
extern const char* LOAD_MA_LUT;
extern const char* DAC_COUNT_LUT;
extern const char* DIODE_V_LUT;
extern const char* DIODE_V_TEMP_LUT;


// Parameter for sorting data points into groups
//      the max span of the data group (highest val -lowest val) as a percentage of the average value of the group
//      if a point added to the group causes the group to exceed this value, the point is not added
#define MAX_SPAN_PERCENT_AVERAGE 1

// the max span of Load Voltage the data group (highest-lowest) as a percentage of the average value of the group
#define MAX_LOAD_V_SPAN_PERCENT_AVERAGE 0.75

// max standard deviation for the points in DAC count group
#define MAX_COUNT_STD_DEV      100

#define MAX_DATA_POINT_VARIANCE 2       // max percentage variance for 1 sec in 10 seconds of sampling

// the max span of the DC Cooler Voltage data group (highest-lowest) as a percentage of the average value
#define MAX_COOLER_V_SPAN_PERCENT_AVERAGE 0.75


// Parameters that apply to Cooler mA measurements as it is driving the Compressor
// max percentage correction between pi and meter. The values may vary per data set
#define MAX_PERCENT_CORRECTION_UNDER_300_MILLIAMPS 7
#define MAX_PERCENT_CORRECTION_OVER_300_UNDER_600_MILLIAMPS 6
#define MAX_PERCENT_CORRECTION_UNDER_1000_MILLIAMPS 7

//////////////////////////////////////////////////////////////////

#endif