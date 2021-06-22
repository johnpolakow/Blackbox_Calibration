#ifndef _TIME2_H_
#define _TIME2_H_

#include <ctype.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <ctime>

#include "StringH.h"


class Time2
{
public:
    Time2() // constructor
    {
        hour = 0;
        minute = 0;
        second = 0;
    }

    Time2(int new_hr, int new_min, int new_sec) // constructor
    {
        this->hour = new_hr;
        this->minute = new_min;
        this->second = new_sec;
    }

    Time2(Time2* copy_time) // constructor
    {
        this->hour = copy_time->hour;
        this->minute = copy_time->minute;
        this->second = copy_time->second;
    }

    ~Time2(){}

    bool Equals(Time2 other_time)
    {
        if( (this->hour == other_time.hour) && (this->minute == other_time.minute) && (this->second == other_time.second) )
            return true;
        else
            return false;
    }

    bool Equals(Time2* other_time)
    {
        if( (this->hour == other_time->hour) && (this->minute == other_time->minute) && (this->second == other_time->second) )
            return true;
        else
            return false;
    }

    char* toString()
    {
        char* time_string = (char*)malloc( sizeof(char) * strlen("XX:XX:XX") + 1);
        if(time_string == NULL)
            printf("\nNO MEMORY HAS BEEN ALLOCATED FOR TIME STRING\n");
        sprintf(time_string, "%02d:%02d:%02d", hour, minute, second);

        return time_string;
    }

    static Time2* Parse_Time(char* time_string);

    int hour;
    int minute;
    int second;
};

class Date2
{
public:
    Date2()
    {
        month = 0;
        day   = 0;
        year  = 0;
    }
    Date2(int new_month, int new_day, int new_year)
    {
        month = new_month;
        day   = new_day;
        year  = new_year;
    }

    Date2(Date2* copy_date)
    {
        month = copy_date->month;
        day   = copy_date->day;
        year  = copy_date->year;
    }

    bool Equals(Date2 other_date)
    {
        if( (this->month == other_date.month) && (this->day ==other_date.day) && (this->year == other_date.year) )
            return true;
        else
            return false;
    }

    bool Equals(Date2* other_date)
    {
        if( (this->month == other_date->month) && (this->day ==other_date->day) && (this->year == other_date->year) )
            return true;
        else
            return false;
    }
    
    char* toString()
    {
        char* date_string = (char*)malloc( sizeof(char) * strlen("XX/XX/XXXX") + 1);
        if(this->year < 100 )   // if only a 2 digit year
            sprintf(date_string, "%02d/%02d/%02d", this->month, this->day, this->year);
        else
            sprintf(date_string, "%02d/%02d/%04d", this->month, this->day, this->year);
        return date_string;
    }

    static Date2* Parse_Date(char* date_string);

    int month;
    int day;
    int year;
};

#endif