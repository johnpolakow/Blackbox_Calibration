

#ifndef AC_DATA_H
#define AC_DATA_H


#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <math.h>

#include "colors.h"
#include "StringH.h"
#include "TStamp.h"
#include "Yoko_Metric.h"

typedef enum AC_data_t{  AC_POWER_VOLTS, AC_POWER_MILLIAMPS, AC_POWER } AC_data_t;

#define NUMBER_OF_AC_POINTS    500

// Data Structure class to hold measurements read from Pi or meters
class AC_Data_Point
{
public:
        AC_Data_Point(double meas_value, Timestamp* timestamp, AC_data_t meas_type)
        {
            value           = meas_value;
            data_timestamp  = timestamp;
            data_type       = meas_type;
        }
        double value;
        AC_data_t data_type;
        Timestamp* data_timestamp;
};

// Data Structure class to hold measurements read from Pi or meters
class AC_Data_Group
{
public:
        AC_Data_Group(const char* descr, const char* data_units)
        {
            data_points = (AC_Data_Point**)malloc( sizeof(AC_Data_Point*) * NUMBER_OF_AC_POINTS);
            for(int i = 0; i< NUMBER_OF_AC_POINTS; ++i)
                data_points[i] = NULL;
            num_points = 0;
            strncpy(description, descr, 25);      // constructor takes as an argument a description of the data
            strncpy(units, data_units, 4);
        }
        ~AC_Data_Group();

        static void Print_AC_Data(AC_Data_Group* data);
        static void Print_AC_Data_Compare(AC_Data_Group* pi_data, AC_Data_Group* yokogawa_data);
        static float Get_AC_Data_Average(AC_Data_Group* data);
        static int Get_Num_Samples_Below_100(AC_Data_Group* data);
        static int Get_Num_Samples_Above_450(AC_Data_Group* data);
        void Add_Sample(Yoko_sample_t* new_sample);
        void Clear_All_Data();

        AC_Data_Point** data_points;
        int num_points;
        char units[4];
        char description[25];
};

#endif
