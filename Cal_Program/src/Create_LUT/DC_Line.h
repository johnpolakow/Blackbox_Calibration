
#ifndef DC_LINE_H
#define DC_LINE_H

#include "./TimeH.h"
#include "./String_Tokenizer.h"
#include "colors.h"

#define DATA_LINES_PER_SEGMENT  10

class DC_Line
{
public:
        DC_Line(char* line)
        {
            this->Parse_Line(line);
        }

        DC_Line(){}

        ~DC_Line(){}

        void Parse_Line(char* readline);
        void Parse_DAC_Line(char* readline);
        static bool Is_Data_Line(char* line);
        static bool Is_Data_Average(char* line);
        static bool Is_Std_Dev(char* line);

        double Pi_value;
        double Meter_value;
        int DAC_count;
        double Error;
};

#endif