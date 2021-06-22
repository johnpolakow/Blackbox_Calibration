
#ifndef DATA_SEGMENT_H
#define DATA_SEGMENT_H

#include <math.h>

#include "Data_Line.h"
#include "TimeH.h"
#include "String_Tokenizer.h"
#include "colors.h"
#include "Cal_Parameters.h"
#include "TimeStamp.h"

#define DATA_LINES_PER_SEGMENT  10

// Data Structure class to hold measurements read from Pi or meters
class Data_Segment
{
public:
        Data_Segment()
        { 
            this->num_data_lines = 0; 
            this->Lines = (Data_Line**)malloc( sizeof(Data_Line*) * DATA_LINES_PER_SEGMENT ); 
            for(int i =0; i< DATA_LINES_PER_SEGMENT; ++i)
                this->Lines[i] = NULL;       

            this->date = NULL;
            this->start_timestamp = NULL;
            this->stop_timestamp = NULL;
            this->ignore_segment = false;
        }
        ~Data_Segment()
        { 
            // printf("deleting data segment. num lines: %d\n", this->num_data_lines);
            int num_deleted = 0;
            for(int i =0; i< DATA_LINES_PER_SEGMENT; ++i)
            {
                if( this->Lines[i] != NULL)
                {
                    delete this->Lines[i];
                    this->Lines[i] = NULL;
                }
            }
            free(this->Lines); 

            if(this->start_timestamp != NULL)
            {
                delete this->start_timestamp;
                this->start_timestamp = NULL;
            }

            if(this->stop_timestamp != NULL)
            {
                delete this->stop_timestamp;
                this->stop_timestamp = NULL;
            }

            if(this->date != NULL)
            {
                delete(this->date);
                this->date = NULL;
            }
        }

        void Add_Line(Data_Line* line);
        static bool Is_Date(char* readline);
        static bool Line_Is_Column_Legend(char* line);
        static bool Line_Is_Segment_Delimiter(char* readline);
        static Data_Segment* Scan_Data_Segment(FILE* read_file);
        void Filter_Data_Lines();
        int Get_Num_Data_Lines();

        Data_Line** Lines;
        double PI_Avg_10_sec;
        double Meter_Avg_10_sec;
        double PI_Spread_10_sec;
        double Meter_Spread_10_sec;
        double PI_STD_DEV;
        double METER_STD_DEV;
        TimeStamp* start_timestamp;
        TimeStamp* stop_timestamp;
        Date* date;
        bool ignore_segment;

private:

        double Get_PI_Average_10sec();
        double Get_Meter_Average_10sec();
        double Get_PI_Spread_10sec();
        double Get_Meter_Spread_10sec();
        double Get_PI_Std_Dev();
        double Get_Meter_Std_Dev();
        int num_data_lines;
};


#endif
