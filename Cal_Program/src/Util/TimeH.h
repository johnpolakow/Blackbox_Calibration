#ifndef _TIME_H_
#define _TIME_H_

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

class Time
{
public:
    Time() // constructor
    {
        hour = 0;
        minute = 0;
        second = 0;
    }

    Time(int new_hr, int new_min, int new_sec) // constructor
    {
        this->hour = new_hr;
        this->minute = new_min;
        this->second = new_sec;
    }

    Time(Time* copy_time) // constructor
    {
        this->hour = copy_time->hour;
        this->minute = copy_time->minute;
        this->second = copy_time->second;
    }

    ~Time(){}

    bool Equals(Time other_time)
    {
        if( (this->hour == other_time.hour) && (this->minute == other_time.minute) && (this->second == other_time.second) )
            return true;
        else
            return false;
    }

    bool Equals(Time* other_time)
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

    static Time* Parse_Time(char* time_string);
    static Time* Parse_Timestamp(char* timestamp_string);

    int hour;
    int minute;
    int second;
};

class Date
{
public:
    Date()
    {
        month = 0;
        day   = 0;
        year  = 0;
    }
    Date(int new_month, int new_day, int new_year)
    {
        month = new_month;
        day   = new_day;
        year  = new_year;
    }

    Date(Date* copy_date)
    {
        month = copy_date->month;
        day   = copy_date->day;
        year  = copy_date->year;
    }

    bool Equals(Date other_date)
    {
        if( (this->month == other_date.month) && (this->day ==other_date.day) && (this->year == other_date.year) )
            return true;
        else
            return false;
    }

    bool Equals(Date* other_date)
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

    static Date* Parse_Date(char* date_string);

    int month;
    int day;
    int year;
};

#endif
