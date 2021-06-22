
#ifndef DATA_LINE_H
#define DATA_LINE_H

#include "TimeH.h"
#include "String_Tokenizer.h"
#include "colors.h"

#define DATA_LINES_PER_SEGMENT  10

// class that represents a "line" of data read from a log file
class Data_Line
{
public:
        Data_Line(char* line)
        {
            for(int i=0; i<ARRAY_SIZE(Meter_Data); ++i)
            {   Meter_Data[i] = 0; }

            this->Meter_index=0;
            this->Parse_Line(line);
            //this->Print_Data();
            this->ignore_line = false;
        }

        Data_Line(){}

        ~Data_Line()
        { 
            delete this->time; 
        }

        void Parse_Line(char* readline);
        static bool Is_Data_Line(char* line);
        void Print_Data();
        static void Print_Data(Data_Line* printline);
        int Get_Num_Meter_Data_Points();

        Time* time;
        double Pi_value;
        double Meter_Data[12];
        double Meter_Avg;
        double Meter_Std_Dev_1sec;
        double Meter_spread_1sec;
        double Avg_Error;
        double Pi_variance;
        double Meter_variance;

private:
        double Get_Meter_Average_1sec();
        double Get_Meter_Std_Dev_1sec();
        double Get_Meter_Spread_1sec();
        bool ignore_line;
        int Meter_index;
};



#endif