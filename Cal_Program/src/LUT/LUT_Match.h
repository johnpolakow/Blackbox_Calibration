#ifndef LUT_MATCH_H
#define LUT_MATCH_H


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
#include "AC_Data.h"


const static int COL_INDEX_PI_MA        = 12;
const static int COL_INDEX_BK5335_MA    = 24;
const static int COL_INDEX_BK_AVG       = 93;
const static int COL_INDEX_BK_STD_DEV   = 107;
const static int COL_INDEX_BK_SPREAD    = 119;

const static int MAX_TIMESTAMP_CHARS    = 9;
const static int MAX_PI_CHARS           = 8;
const static int MAX_BK_CHARS           = 65;
const static int MAX_BK_AVG_CHARS       = 11;
const static int MAX_BK_STD_DEV_CHARS   = 12;
const static int MAX_BK_SPREAD          = 12;

#define MAX_MATCHES 500
#define TEMP_STRING_SIZE 15

class LUT_Match     // Data Structure class to hold measurements read from Pi or meters
{
public:
        LUT_Match(double pi_val, double YK_vals[], Timestamp* timestamp, const char* data_units)
        {
            this->pi_value = pi_val;
            this->data_timestamp = timestamp;
            this->num_Yokogawa_values = 0;
            strncpy(this->units, data_units, 4);

            for(int i=0; i< ARRAY_SIZE(Yokogawa_values); ++i )
            {
                if( YK_vals[i] != -1 )
                {
                    this->Yokogawa_values[i] = YK_vals[i];
                    ++this->num_Yokogawa_values;
                }
                else
                    this->Yokogawa_values[i] = -1;
            }
            this->Calc_Yokogawa_Spread();
            this->Calc_Yokogawa_Average();
            this->Calc_Yokogawa_Std_Dev();
        }
        double pi_value;
        double Yokogawa_values[15];
        int num_Yokogawa_values;
        double Yokogawa_Avg;
        double Yokogawa_Std_Dev;
        double Yokogawa_spread;
        double Avg_Difference;
        char units[4];
        Timestamp* data_timestamp;

        static void Print_Match(LUT_Match* match);
        static char* toString(LUT_Match* match);
        static char* toString_Header();
private:
        double Calc_Yokogawa_Average();
        double Calc_Yokogawa_Std_Dev();
        double Calc_Yokogawa_Spread();
};

#endif
