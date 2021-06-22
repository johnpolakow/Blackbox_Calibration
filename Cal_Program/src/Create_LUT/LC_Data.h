

#ifndef DATA_H
#define DATA_H


#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include "String_Tokenizer.h"
#include "Data_Segment.h"
#include "StringH.h"
#include "colors.h"
#include "FileH.h"
#include "Cal_Parameters.h"
#include "TimeStamp.h"

#define MAX_DATA_SEGMENTS 1000

// Data Structure class to hold measurements read from Pi or meters
class LC_Data
{
public:
        LC_Data()
        {
            data_array = (Data_Segment**)malloc( sizeof(Data_Segment*) * MAX_DATA_SEGMENTS);
            for(int i = 0; i< MAX_DATA_SEGMENTS; ++i)
                data_array[i] = NULL;

            num_segments = 0;
            data_is_trimmed_for_start_times = false;
            num_deleted_segments = 0;
            total_segments = 0;
        }
        ~LC_Data()
        {
            for(int i = 0; i<num_segments; ++i)
            {
                if( data_array[i] != NULL)
                {
                    delete data_array[i];
                    data_array[i] = NULL;
                }
            }
            free(data_array);
            data_array = NULL;
        }

        static void Print_mA_Reading(bool reading_valid, float pi_val, float meter_val, float error_val, float percent_error);
        Data_Segment** Get_Data(){ return this->data_array; }
        int Get_Num_Data_Points(){ return this->num_segments; }
        void Trim_First_20_Seconds_Sampling_Each_Session();
        void Delete_Flagged_Segments();
        int Get_Index_Next_Valid_Segment(int start_index);
        void Remove_Segments_Large_mA_Correction_Value();
        void Trim_Outlier_mA_Data();
        void Scan_Data(char* filepath);
        char mac_addr[20];


private:
        void Add_New_Data_Segment(Data_Segment* seg);
        void Order_Segments_By_PI_Value();
        int Index_Next_Smallest_PI_Value(int start_index);
        Data_Segment** data_array;   // store max 100 segments before writing to file
        int num_segments;
        bool data_is_trimmed_for_start_times;
        int num_deleted_segments;
        int total_segments;
};

#endif
