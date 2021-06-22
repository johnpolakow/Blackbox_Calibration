

#ifndef LC_LUT_H
#define LC_LUT_H


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

//#include "Cal_Parameters.h"
#include "String_Tokenizer.h"
#include "Data_Segment.h"
#include "LC_Data.h"
#include "StringH.h"
#include "colors.h"
#include "FileH.h"




#define MAX_LUT_POINTS      500
#define MAX_LUT_GROUP_POINTS    50


typedef enum LUT_t
{
    COOLER_MA_t,
    COOLER_MA_DC_t,
    COOLER_V_t,
    LOAD_MA_t,
    LOAD_V_t,
    DIODE_V_t,
    REF100_t,
    DAC_COUNT_t,
    THERMOCOUPLE_t,
    NONE,
} LUT_TYPE;

class AC_LUT_HEADER
{
public:
    AC_LUT_HEADER(LUT_TYPE type, int num_elements, char* MAC);

    ~AC_LUT_HEADER(){}

    char* toString();

    char ifndef[30];
    char define[30];
    char dimension1[80];
    char dimension2[80];
    char num_elements[40];
    char array_declaration[50];
    char comment[60];
    const char* array_end = "};";
    char typestring[40];
    char mac_addr_line[60];
};


class LUT_POINT
{
public:
        LUT_POINT(Data_Segment* in_data)
        {
            this->data = in_data;
            PI_value = Get_PI_Avg(this->data);
            Meter_value = Get_Meter_Avg(this->data);
        }
        ~LUT_POINT(){}

        double Calc_PI_Variance(double group_average);

        double PI_value;
        double Meter_value;

private:
        void Throwout_One_PI_Val();
        void Throwout_One_BK_Val(); 
        double Get_PI_Avg(Data_Segment* data){ return data->PI_Avg_10_sec; }
        double Get_Meter_Avg(Data_Segment* data){ return data->Meter_Avg_10_sec; }

        Data_Segment* data;
};

class LUT_Group
{
public:
        LUT_Group()
        {
            points = (LUT_POINT**)(malloc(sizeof(LUT_POINT*) * MAX_LUT_GROUP_POINTS));
            for(int i=0; i<MAX_LUT_GROUP_POINTS; ++i)
            {
                points[i] = NULL;
            }
            this->num_points = 0;
        }

        ~LUT_Group()
        {
            for(int i=0; i<MAX_LUT_GROUP_POINTS; ++i)
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
        bool Add_Point(LUT_POINT* new_point);
        void Remove_Last_Point();
        int Get_Num_Points(){ return this->num_points; };
        void Calc_Metrics();
        LUT_POINT** Get_Points(){ return this->points; }

        int Get_Index_Largest_Variance();
        void Throwout_Value_at_Index(int index);


        double pi_average;
        double meter_average;
        double pi_span;
        double pi_span_percent_of_average;
        double pi_std_dev;
        double pi_std_dev_percent_of_average;
        double STD_DEV_THRESHOLD;

private:
        double Calc_Average();
        double Calc_Span();
        double Calc_Variance(int index);
        double Calc_Std_Dev();
        double Calc_Span_Percent_of_Average();
        int num_points;
        LUT_POINT** points;
};


// class to hold LUT points ready for writing to file
class LC_LUT
{
public:
        LC_LUT()
        {
            table = (LUT_Group**)malloc( sizeof(LUT_Group*) * MAX_LUT_POINTS);
            for(int i = 0; i< MAX_LUT_POINTS; ++i)
                table[i] = NULL;

            this->num_groups = 0;
        }
        ~LC_LUT()
        {
            for(int i = 0; i<num_groups; ++i)
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

        void Add_LUT_Data(LC_Data* data);
        void Add_Group(LUT_Group* group);
        void Write_to_File(const char* filepath);
        void Print_Groups();
        void Filter_Groups();
        static LUT_TYPE Get_LUT_Type(const char* filename);
        static LUT_TYPE Get_LUT_Type_From_Content(char* title);
        static char* Get_LUT_Type_String(LUT_TYPE type);
        char mac_addr[20];

private:
        void Filter_For_Variance();
        void Filter_For_SD_Percent_of_Average();
        void Filter_For_Num_Points();
        void Remove_Group_At_Index(int delete_index);
        void Write_LUT_Header(FILE* writefile);
        void Calc_LUT_Metrics();
        LUT_Group** table;  
        int num_groups;
};

#endif
