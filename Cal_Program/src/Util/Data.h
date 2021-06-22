
#ifndef _DATA_H
#define _DATA_H


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

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )    // macro to get number of items in an object array
#endif

#define ONE_STD_DEV     1
#define TWO_STD_DEV     2
#define THREE_STD_DEV   3

static const char* LINE_END =   "\n";
#define SAVE_CURSOR_POSITION    "\033[s"
#define RESTORE_CURSOR_POSITION "\033[u"
#define MOVE_CURSOR_FORWARD_40  "\033[40C"

// Data Structure class to hold measurements read from Pi or meters
class Data_Group
{
public:
  Data_Group(const char* descr)
  {
      // initialize values to -1
      // constructor takes as an argument a description of the data
      num_points = 0;
      std_dev    = -1;
      average    = -1;
      strncpy(description, descr, 25);
  }

    float* data_points;
    int num_points;
    float std_dev;
    float average;
    char units[4];
    char description[25];
};

class Data
{
public:
    Data()
    { /* empty constructor, static class */ }

    static float Calc_Average(Data_Group* datagroup);
    static float Calc_Std_Dev(Data_Group* datagroup);
    static void Unit_Convert(float* array, float multiply_factor, int num_elements);
    static void Print_Float_Array(float* data, char* description, int num_elements);
    static void Free_Data_Group_Memory(Data_Group* datagroup);
    static void Print_DataGroup(Data_Group* data);
    static void Print_DataGroup_Compare(Data_Group* data1, Data_Group* data2);
    static Data_Group* Discard_Outlier_Points(Data_Group* data);
    static Data_Group* Discard_Outlier_Points(Data_Group* data, int std_dev_threshold);
    static bool Detect_Outliers(Data_Group* data, int std_dev_threshold);
};

#endif
