


#include "TStamp.h"
#


Timestamp::Timestamp()
{

    char target_string[10];
    StringH::Erase_String( target_string);

    time_t rawtime = std::time(0);
    struct tm* timeinfo;
    timeinfo = localtime(&rawtime);

    timeinfo = localtime(&rawtime);
    strftime (target_string, 10,"%H",timeinfo);
    this->time.hour = StringH::Parse_Int(target_string);

    StringH::Erase_String( target_string);
    strftime (target_string, 10,"%M",timeinfo);
    this->time.minute = StringH::Parse_Int(target_string);

    StringH::Erase_String( target_string);
    strftime (target_string, 10,"%S",timeinfo);
    this->time.second = StringH::Parse_Int(target_string);

	 strftime (target_string,10,"%d", timeinfo);
   this->date.day = StringH::Parse_Int(target_string);

   StringH::Erase_String( target_string);
   strftime (target_string,10,"%m ", timeinfo);
   this->date.month = StringH::Parse_Int(target_string);

   StringH::Erase_String( target_string);
   strftime (target_string,10,"%Y ", timeinfo);
   this->date.year = StringH::Parse_Int(target_string);


}

char* Timestamp::Get_Timestamp( )
{
    char target_string[50];
    StringH::Erase_String(target_string);
    time_t rawtime = std::time(0);
    struct tm* timeinfo;
    timeinfo = localtime(&rawtime);
    strftime (target_string,50,"%H:%M:%S  %m/%d/%Y ",timeinfo);
    return strdup(target_string);
}

bool Timestamp::Equals(Timestamp* other_timestamp)
{

    bool dates_match = this->date.Equals(other_timestamp->date);
    bool times_match = this->time.Equals(other_timestamp->time);

    //char* timestamp1 = this->toString();
    //char* timestamp2 = other_timestamp->toString();
    //printf(" comparing %s to %s ....\t", timestamp1, timestamp2);
    //free(timestamp1);
    //free(timestamp2);

    if( dates_match && times_match)
    {
        //printf(" Match\n");
        return true;
    }
    else if( dates_match && !times_match)
    {
        char* date1 = this->date.toString();
        char* time1 = this->time.toString();
        char* time2 = other_timestamp->time.toString();

        //printf("time1 %s  !=  time2 %s\n", date1, time1, time2);
        free(date1);
        free(time1);
        free(time2);
        return false;
    }
    else if( times_match && !dates_match)
    {
        char* time1 = this->time.toString();
        char* date1 = this->date.toString();
        char* date2 = other_timestamp->date.toString();

        //printf("date1 %s  !=  date2 %s\n", time1, date1, date2);
        free(time1);
        free(date1);
        free(date2);
        return false;
    }
    else if( !times_match && !dates_match)
    {
        //printf("Both time and date do not match\n");
        return false;
    }
    return false;

}

char* Timestamp::toString()
{

        char* str_date = this->date.toString();
        char* str_time = this->time.toString();

        char* timestamp_string = (char*)malloc( sizeof(char) * (strlen(str_date) + strlen(str_time) + 15) );
        StringH::Erase_String(timestamp_string);
        sprintf(timestamp_string, "%s    %s", str_time, str_date);
        free(str_date);
        free(str_time);

        return timestamp_string;
}
