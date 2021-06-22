
#ifndef DC_SEGMENT_H
#define DC_SEGMENT_H

#include <math.h>

#include "TimeH.h"
#include "String_Tokenizer.h"
#include "colors.h"
#include "DC_Line.h"
#include "LC_LUT.h"

#define DATA_LINES_PER_SEGMENT  10


// Data Structure class to hold measurements read from Pi or meters
class DC_Segment
{
public:
        DC_Segment()
        { 
            this->num_data_lines = 0; 
            this->Lines = (DC_Line**)malloc( sizeof(DC_Line*) * DATA_LINES_PER_SEGMENT ); 
            for(int i =0; i< DATA_LINES_PER_SEGMENT; ++i)
                this->Lines[i] = NULL;       

            this->ignore_segment = false;
        }
        DC_Segment( LUT_TYPE type )
        { 
            this->LUT_type = type;
            this->num_data_lines = 0; 
            this->Lines = (DC_Line**)malloc( sizeof(DC_Line*) * DATA_LINES_PER_SEGMENT ); 
            for(int i =0; i< DATA_LINES_PER_SEGMENT; ++i)
                this->Lines[i] = NULL;       

            this->ignore_segment = false;
        }
        ~DC_Segment()
        { 
            for(int i =0; i< this->num_data_lines; ++i)
            {
                delete this->Lines[i];  
            }
            free(this->Lines); 
        }

        void Add_Line(DC_Line* line);
        static bool Is_Date(char* readline);
        static bool Line_Is_Column_Legend(char* line);
        static bool Line_Is_Segment_Delimiter(char* readline);
        static DC_Segment* Scan_DC_Segment(FILE* read_file, LUT_TYPE type);
        void Calc_Metrics();
        void Read_DAC_Count();

        DC_Line** Lines;
        int DAC_count;
        double PI_Avg;
        double Meter_Avg;
        double PI_Std_Dev;
        double Meter_Std_Dev;
        bool ignore_segment;
        int num_data_lines;
        LUT_TYPE LUT_type;

private:
        void Calc_PI_Average();
        void Calc_Meter_Average();
        void Calc_PI_Std_Dev();
        void Calc_Meter_Std_Dev();

};


#endif
