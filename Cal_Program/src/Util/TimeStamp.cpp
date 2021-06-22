


#include "TimeStamp.h"
#include "String_Tokenizer.h"






double TimeStamp::Time_Difference_Seconds(TimeStamp* T1, TimeStamp* T2)
{
    struct tm t1, t2;

    Date* date1 = T1->Get_Date();
    Time* time1 = T1->Get_Time();
    Date* date2 = T2->Get_Date();
    Time* time2 = T2->Get_Time();

    t1.tm_year = date1->year;
    t1.tm_mon = date1->month -1;
    t1.tm_mday = date1->day;
    t1.tm_hour = time1->hour;
    t1.tm_min = time1->minute;
    t1.tm_sec = time1->second;

    t2.tm_year = date2->year;
    t2.tm_mon = date2->month -1;
    t2.tm_mday = date2->day;
    t2.tm_hour = time2->hour;
    t2.tm_min = time2->minute;
    t2.tm_sec = time2->second;

    int diff_month = date2->month - date1->month;
    int diff_day = date2->day - date1->day;
    int diff_hour = time2->hour - time1->hour;
    int diff_min = time2->minute - time1->minute;
    int diff_secs = time2->second - time1->second;

    double total_difference = diff_secs + 60*diff_min + 3600*diff_hour + diff_day*86400 +diff_month*2592000;

    return total_difference;
}

TimeStamp* TimeStamp::Parse_TimeStamp(char* timestamp_string)
{

    Tokenizer token(timestamp_string);
    int token_num = 0;
    char* current_token;
    Time* parse_time;
    Date* parse_date;

    while( (current_token = token.Get_Next_Token() ) != NULL )
    {
        if(token_num == 0)
        {
            parse_time = Time::Parse_Time(current_token);
        }
        if(token_num == 1)
        {
            parse_date = Date::Parse_Date(current_token);
        }
        free(current_token);
    }

    TimeStamp* timestamp = new TimeStamp(parse_time, parse_date);
    delete parse_time;
    delete parse_time;
    return timestamp;
}