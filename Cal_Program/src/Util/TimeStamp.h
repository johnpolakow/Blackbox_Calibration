#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

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
#include "TimeH.h"

class TimeStamp
{
public:
    TimeStamp() // constructor
    {
        this->time = new Time();
        this->date = new Date();
    }

    TimeStamp(Time* new_time, Date* new_date)
    {
        this->time = new Time(new_time);
        this->date = new Date(new_date);
    }

    ~TimeStamp() // constructor
    {
        if(this->time != NULL)
            delete(this->time);

        if(this->date != NULL)
            delete(this->date);
    }

    static double Time_Difference_Seconds(TimeStamp* T1, TimeStamp* T2);
    
    bool Equals(TimeStamp* other_timestamp)
    {
        if( this->time->Equals( other_timestamp->Get_Time() ) && this->date->Equals( other_timestamp->Get_Date() ) )
            return true;
        else
            return false;
    }

    Time* Get_Time(){ return this->time; }
    Date* Get_Date(){ return this->date; }

    void Set_Time(Time* new_time)
    {
        delete this->time;
        this->time = new_time;
    }

    void Set_Date(Date* new_date)
    {
        delete this->date;
        this->date = new_date;
    }

    char* toString()
    {
        char* timestamp_string = (char*)malloc( sizeof(char) * strlen("XX:XX:XX  XX/XX/XXXX") + 1);
        if(timestamp_string == NULL)
            printf("\nNO MEMORY HAS BEEN ALLOCATED FOR TIMESTAMP\n");
        char* time_string = time->toString();
        char* date_string = date->toString();
        sprintf(timestamp_string, "%s %s", time_string, date_string);
        free(time_string);
        free(date_string);
        return timestamp_string;
    }

    static TimeStamp* Parse_TimeStamp(char* timestamp_string);

private:
    Time* time;
    Date* date;
};
#endif
