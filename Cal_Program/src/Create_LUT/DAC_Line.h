
#ifndef DAC_LINE_H
#define DAC_LINE_H

#include "TimeH.h"
#include "String_Tokenizer.h"
#include "colors.h"

#define DATA_LINES_PER_SEGMENT  10

class DAC_Line
{
public:
        DAC_Line(char* line)
        {
            this->Parse_Line(line);
        }

        DAC_Line(){}

        ~DAC_Line(){}

        void Parse_Line(char* readline);
        static bool Is_Data_Line(char* line);

        double Pi_value;
        double Meter_value;
        int DAC_Count;
        double Error;
};

#endif