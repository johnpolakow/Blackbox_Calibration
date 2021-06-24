#include "Cal_Parameters.h"
#include "Struct_Defs.h"

const char* DEFAULT_SERVER_PORT = "2020";
const char* HP_A_ID = "0,11-5-2";
const char* HP_B_ID = "0,2-1-1";
const char* AGILENT_PS_ID = "0,1.7-5.0-1.0";

const char CAL_LOG_DIR[] = "/home/pi/CAL_LOGS/";    // output directory for log files during calibration


const char* CAL_STATION_MAC_ADDR = "B8:27:EB:2E:B7:3A";

const int SSH_VERBOSITY = SSH_LOG_WARNING;   // verbosity options for SSH library reporting to console
    // verbosity options:
        // SSH_LOG_NOLOG
        // SSH_LOG_WARNING
        // SSH_LOG_PROTOCOL
        // SSH_LOG_PACKET
        // SSH_LOG_FUNCTION

const char* REMOTE_PI_CAL_FILES_DIR    = "/home/pi/firmware/CAL_DATA"; 
const char* REMOTE_PI_FIRMWARE_SRC_DIR = "/home/pi/firmware/src";

const char* REMOTE_SSH_USER = "pi";
const char* REMOTE_SSH_PASSWORD  = "raspberry";
const char* REMOTE_KEYS_FILE = "/home/pi/.ssh/authorized_keys";
const int SSH_PORT  =  22;

const char* DAQ_PROCESS_SERVICE_NAME = "Blackberry.service";


// Defined here are different DAC counts to increment by, when calibrating the mapping of Dac_count to PI Load mA out
const int DAC_INCREMENT = 200;

// these are the calibration points for the PI DAQ measuring the output voltage to the CCC
double DCV_Set_Points[] = { 0.1, 0.5, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10.5,
                            10.75, 11, 11.25, 11.5, 11.75, 11.9, 12, 12.05, 12.1,
                            12.15, 12.2, 12.25, 12.3, 12.35, 12.4, 12.45, 12.5,
                            12.55, 12.6, 12.65, 12.7, 12.75, 12.8, 12.85, 12.9,
                            13, 13.5, 14, 14.5
                          };


// Diode voltage is calibrated every 5mV from 0.6V to 1.20V. Complete range is 0.1V to 1.5V
// these are the voltage calibration points for the diode voltage
double Diode_Cal_Points[] = { 
                            0.1, 0.2, 0.3, 0.4, 0.5, 0.55, 0.58, 0.60, 0.605, 0.610, 0.615, 
                            0.620, 0.625, 0.630, 0.635, 0.64, 0.645, 0.650, 0.655, 0.660, 
                            0.665, 0.670, 0.675, 0.680, 0.685, 0.690, 0.695, 0.700, 0.705, 
                            0.710, 0.715, 0.720, 0.725, 0.730, 0.735, 0.74, 0.745, 0.750, 
                            0.755, 0.760, 0.765, 0.770, 0.775, 0.780, 0.785, 0.790, 0.795, 
                            0.800, 0.805, 0.810, 0.815, 0.820, 0.825, 0.830, 0.835, 0.840, 
                            0.845, 0.850, 0.855, 0.860, 0.865, 0.870, 0.875, 0.880, 0.885, 
                            0.890, 0.895, 0.900, 0.905, 0.910, 0.915, 0.920, 0.925, 0.930, 
                            0.935, 0.94, 0.945, 0.950, 0.955, 0.960, 0.965, 0.970, 0.975, 
                            0.980, 0.985, 0.99, 0.995, 1.0, 1.005, 1.010, 1.015, 1.020, 
                            1.025, 1.030, 1.035, 1.040, 1.045, 1.05, 1.055, 1.060, 1.065, 
                            1.070, 1.075, 1.080, 1.085, 1.090, 1.095, 1.10, 1.115, 1.12, 
                            1.125, 1.130, 1.135, 1.140, 1.145, 1.150, 1.155, 1.160, 1.165, 
                            1.170, 1.175, 1.180, 1.185, 1.190, 1.195, 1.200, 1.21, 1.22, 
                            1.23, 1.24, 1.25, 1.26, 1.27, 1.28, 1.29, 1.30, 1.31, 1.32, 1.33, 
                            1.34, 1.35, 1.36, 1.37, 1.38, 1.39, 1.40, 1.41, 1.42, 1.43, 1.44, 
                            1.45, 1.46, 1.47, 1.48, 1.49, 1.50
                          };


int V_POINTS_NUM_ELEMENTS = ARRAY_SIZE(DCV_Set_Points);
int DIODE_POINTS_NUM_ELEMENTS = ARRAY_SIZE(Diode_Cal_Points);



//// FILE LOCATIONS FOR DATA FILTER AND LUT CREATE:
const char* ROOT_LOG_DIR = "/home/pi/CAL_LOGS/";
const char* ROOT_LUT_DIR = "/home/pi/CAL_LUT/";
const char* ROOT_PROCESSED_DIR = "/home/pi/CAL_LOGS/PROCESSED_LOGS/";

const char* COOLER_MA_LUT = "COOLER_mA_LUT.h";
const char* COOLER_V_LUT  = "COOLER_V_LUT.h";
const char* LOAD_V_LUT    = "LOAD_V_LUT.h";
const char* LOAD_MA_LUT   = "LOAD_mA_LUT.h";
const char* DAC_COUNT_LUT = "DAC_COUNT_LUT.h";
const char* DIODE_V_LUT   = "DIODE_V_LUT.h";
const char* DIODE_V_TEMP_LUT = "TEMP_DIODE.h";
