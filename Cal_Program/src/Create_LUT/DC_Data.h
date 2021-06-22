

#ifndef DC_DATA_H
#define DC_DATA_H


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
#include "./StringH.h"
#include "./colors.h"

#include "./FileH.h"
#include "DC_Segment.h"
#include "LC_LUT.h"


#define MAX_DC_SEGMENTS 500


// Data Structure class to hold measurements read from Pi or meters
class DC_Data
{
public:
        DC_Data(LUT_TYPE type)
        {
            this->LUT_type = type;
            data_array = (DC_Segment**)malloc( sizeof(DC_Segment*) * MAX_DC_SEGMENTS);
            for(int i = 0; i< MAX_DC_SEGMENTS; ++i)
                data_array[i] = NULL;

            num_segments = 0;
            total_segments = 0;
        }
        ~DC_Data()
        {
            for(int i = 0; i<MAX_DC_SEGMENTS; ++i)
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

        DC_Segment** Get_Data(){ return this->data_array; }
        int Get_Num_Data_Points(){ return this->num_segments; }
        void Delete_Flagged_Segments();
        int Get_Index_Next_Valid_Segment(int start_index);
        void Remove_Segments_Large_Correction_Value();
        void Remove_Segments_Meter_Disconnected();
        void Filter_Data();
        void Scan_Data(char* filepath);
        void Scan_DAC_Data(char* filepath, LUT_TYPE type);
        void Print_Data();
        LUT_TYPE LUT_type;
        int num_segments;
        char mac_addr[20];

private:
        void Add_New_Data_Segment(DC_Segment* seg);
        void Order_Segments_By_PI_Value();
        int Index_Next_Smallest_PI_Value(int start_index);
        DC_Segment** data_array;   // store max 100 segments before writing to file
        int total_segments;

};

#endif
