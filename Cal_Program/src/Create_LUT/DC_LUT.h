

#ifndef DC_LUT_H
#define DC_LUT_H


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
#include "DC_Data.h"
#include "StringH.h"
#include "colors.h"
#include "FileH.h"
#include "Cal_Parameters.h"

#define MAX_DC_LUT_POINTS      500
#define MAX_DC_GROUP_POINTS    50



class DC_LUT_HEADER
{
public:
    DC_LUT_HEADER(LUT_TYPE type, int num_elements, char* MAC);

    ~DC_LUT_HEADER(){}

    char* toString();

    char ifndef[30];
    char define[30];
    char dimension1[80];
    char dimension2[80];
    char num_elements[40];
    char array_declaration[50];
    const char* array_end = "};";
    char comment[128];
    char typestring[50];
    char mac_addr_line[60];
};


class DC_LUT_POINT
{
public:
        DC_LUT_POINT(DC_Segment* in_data)
        {
            this->data = in_data;
            PI_value = Get_PI_Avg(this->data);
            Meter_value = Get_Meter_Avg(this->data);
            if( this->data->LUT_type == DAC_COUNT_t )
                this->DAC_count = Get_DAC_Count(this->data);
        }
        DC_LUT_POINT(DC_Segment* in_data, LUT_TYPE type)
        {
            this->data = in_data;
            if(type == DAC_COUNT_t)
            {
                DAC_count = Get_DAC_Count(this->data);
                PI_value = Get_PI_Avg(this->data);
                Meter_value = Get_Meter_Avg(this->data);
            }
            else
            {
                PI_value = Get_PI_Avg(this->data);
                Meter_value = Get_Meter_Avg(this->data);
                if( this->data->LUT_type == DAC_COUNT_t )
                    this->DAC_count = Get_DAC_Count(this->data);
            }
        }
        ~DC_LUT_POINT(){}

        double PI_value;
        double Meter_value;
        int DAC_count;

private:
        double Get_PI_Avg(DC_Segment* data){ return data->PI_Avg; }
        double Get_Meter_Avg(DC_Segment* data){ return data->Meter_Avg; }
        int Get_DAC_Count(DC_Segment* data){ return data->DAC_count; }
        DC_Segment* data;
};

class DC_Group
{
public:
        DC_Group()
        {
            points = (DC_LUT_POINT**)(malloc(sizeof(DC_LUT_POINT*) * MAX_DC_GROUP_POINTS));
            for(int i=0; i<MAX_DC_GROUP_POINTS; ++i)
            {
                points[i] = NULL;
            }
            this->num_points = 0;
        }

        ~DC_Group()
        {
            for(int i=0; i<MAX_DC_GROUP_POINTS; ++i)
            {
                if( points[i] != NULL)
                {
                    delete points[i];
                    points[i] = NULL;
                }
            }
            free(points);
        }

        bool Has_Point_Over_Threshold();
        bool Add_Point(DC_LUT_POINT* new_point, LUT_TYPE type);
        bool Add_DAC_Point(DC_LUT_POINT* new_point);
        void Remove_Last_Point();
        int Get_Num_Points(){ return this->num_points; };
        void Calc_Metrics();
        void Calc_Counts();
        DC_LUT_POINT** Get_Points(){ return this->points; }
        double Calc_PI_Variance(int index);

        int Get_Index_Largest_Variance();
        void Throwout_Value_at_Index(int index, const char* msg);
        void Print_Group();
        void Print_Group_Highlight_Index(int hi_index);

        int dac_count;
        double pi_average;
        double meter_average;
        double pi_span;
        double pi_span_percent_of_average;
        double pi_std_dev;
        double pi_std_dev_percent_of_average;
        double STD_DEV_THRESHOLD;
        LUT_TYPE type;

private:
        double Calc_Average();
        double Calc_Span();
        double Calc_Std_Dev();
        double Calc_Span_Percent_of_Average();
        double Calc_DAC_Count_Std_Dev();
        double Calc_DAC_Count_Average();

        double count_average;
        double count_std_dev;
        int num_points;
        DC_LUT_POINT** points;
};


// class to hold LUT points ready for writing to file
class DC_LUT
{
public:
        DC_LUT(LUT_TYPE type)
        {
            this->LUT_type = type;
            table = (DC_Group**)malloc( sizeof(DC_Group*) * MAX_DC_LUT_POINTS);
            for(int i = 0; i< MAX_DC_LUT_POINTS; ++i)
                table[i] = NULL;
            this->num_groups = 0;
        }
        DC_LUT()
        {
            table = (DC_Group**)malloc( sizeof(DC_Group*) * MAX_DC_LUT_POINTS);
            for(int i = 0; i< MAX_DC_LUT_POINTS; ++i)
                table[i] = NULL;
            StringH::Erase_Num_Chars(this->mac_addr, 20);
            this->num_groups = 0;
        }
        ~DC_LUT()
        {
            for(int i = 0; i<MAX_DC_LUT_POINTS; ++i)
            {
                if( table[i] != NULL)
                {
                    delete table[i];
                    table[i] = NULL;
                }
            }
            free(table);
            table = NULL;
        }

        void Add_DAC_LUT_Data(DC_Data* data, LUT_TYPE type);
        void Add_LUT_Data(DC_Data* data);
        void Add_Group(DC_Group* group);
        void Write_to_File(const char* filepath);
        void Write_DAC_LUT_to_File(const char* filepath);
        void Print_Groups();
        void Filter_Groups();
        static LUT_TYPE Get_LUT_Type(char* filename);
        char mac_addr[20];
        LUT_TYPE LUT_type;

private:
        void Filter_For_Variance();
        void Filter_For_SD_Percent_of_Average();
        void Remove_Group_At_Index(int delete_index);
        void Write_LUT_Header(LUT_TYPE type, FILE* writefile);
        DC_Group** table;  
        int num_groups;
        

};

#endif
