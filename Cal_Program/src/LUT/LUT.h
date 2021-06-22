

#ifndef LUT_H
#define LUT_H


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
#include "AC_Data.h"
#include "Write_Log.h"

#define MAX_MATCHES 500
#define NUM_YOKOGAWA_SAMPLES_PER_SECOND 15

class Yokogawa_samples
{
public:
        Yokogawa_samples()
        {
            cur_index = 0;
            for(int i = 0; i<ARRAY_SIZE(data_equal_time); ++i)
                data_equal_time[i] = -1;
        }

        void Add_Yokogawa_sample(double value);
        int Get_Num_Samples();
        double* Get_Yokogawa_samples();

        int cur_index;
        double data_equal_time[NUM_YOKOGAWA_SAMPLES_PER_SECOND];    // Yokogawa can take max 6 samples per second
};

// Data Structure class to hold measurements that are read from Pi or meters
class LUT_Data
{
public:
        LUT_Data(const char* descr)
        {
            this->data_matches = (LUT_Match**)malloc( sizeof(LUT_Match*) * MAX_MATCHES);
            for(int i = 0; i< MAX_MATCHES; ++i)
                this->data_matches[i] = NULL;

            this->num_points = 0;
            strncpy(this->description, descr, 40);                     // constructor takes as an argument a description of the data
            //strncpy(PI_MAC_Address, MAC_Address, 20);            // constructor takes as an argument the PI MAC address
        }
        ~LUT_Data();

        void Match_Time_Samples(AC_Data_Group* pi_data_group, AC_Data_Group* Yokogawa_data_group);
        static void Print_Matches(LUT_Data* matches);
        void Add_Match(LUT_Match* new_match);
        void Clear_All_Matches();
        static void Clean_Up_Data(LUT_Match** noisy_data);
        static void Write_Data_To_File(LUT_Match** matches, int num_matches);

        LUT_Match** data_matches;
        int num_points;
        char description[40];
        //char PI_MAC_Address[20];
};

#endif
