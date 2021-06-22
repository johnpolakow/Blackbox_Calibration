


#include "TimeH.h"

/*
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


// stores current system time into time_object
void BK_5335::Get_Time_Now(Time* time_object)
{
   char target_string[50];
   Erase_String_Contents( target_string);


	 strftime (target_string,50,"%H",timeinfo);
   time_object->hour = String_Helper::Parse_Int(target_string);

   Erase_String_Contents( target_string);
   strftime (target_string,50,"%M",timeinfo);
   time_object->minute = String_Helper::Parse_Int(target_string);

   Erase_String_Contents( target_string);
   strftime (target_string,50,"%S",timeinfo);
   time_object->second = String_Helper::Parse_Int(target_string);
}

// stores current date into date_object
void BK_5335::Get_Date_Now(Date date_object)
{
   char target_string[50];
   Erase_String_Contents( target_string);

	 time_t rawtime;
	 struct tm* timeinfo;
	 time (&rawtime);
	 timeinfo = localtime(&rawtime);
	 strftime (target_string,50,"%d",timeinfo);
   date_object.day = String_Helper::Parse_Int(target_string);

   Erase_String_Contents( target_string);
   strftime (target_string,50,"%m ",timeinfo);
   date_object.month = String_Helper::Parse_Int(target_string);

   Erase_String_Contents( target_string);
   strftime (target_string,50,"%Y ",timeinfo);
   date_object.year = String_Helper::Parse_Int(target_string);
}



bool Timestamp::Equals(Timestamp* other_timestamp)
{
    bool dates_match = this->date.Equals(other_timestamp->date);
    bool times_match = this->time.Equals(other_timestamp->time);

    char* timestamp1 = this->toString();
    char* timestamp2 = other_timestamp->toString();
    printf(" comparing %s to %s\n", timestamp1, timestamp2);
    free(timestamp1);
    free(timestamp2);

    if( dates_match && times_match)
        return true;
    else if( dates_match && !times_match)
    {
        char* date1 = this->date.toString();
        char* time1 = this->time.toString();
        char* time2 = other_timestamp->time.toString();

        printf("dates %s match, time1 %s  !=  time2 %s\n", date1, time1, time2);
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

        printf("times %s match, date1 %s  !=  date2 %s\n", time1, date1, date2);
        free(time1);
        free(date1);
        free(date2);
        return false;
    }
    else if( !times_match && !dates_match)
    {
        printf("Both time and date do not match\n");
        return false;
    }
}

char* Timestamp::toString()
{
        char* str_date = this->date.toString();
        char* str_time = this->time.toString();

        char* timestamp_string = (char*)malloc( sizeof(char) * (strlen(str_date) + strlen(str_time) + 3) );
        StringH::Erase_String(timestamp_string);
        sprintf(timestamp_string, "%s %s", str_time, str_date);

        return timestamp_string;
}

*/

Time* Time::Parse_Time(char* time_string)
{
    char *str_hour, *str_minute, *str_second;
    int look_ahead_index = 0;
    int substr_start_index=0, substr_end_index=0;

    // PARSE THE HOUR
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit hour
    while( time_string[look_ahead_index] != ':' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first ':', should be max 2 chars before ':'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit hour

    str_hour = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
    int hour = StringH::Parse_Int( str_hour );     // parse month string to an integer
    free(str_hour);

    // PARSE THE MINUTE
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "MINUTE" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit minute
    while( time_string[look_ahead_index] != ':' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first ':', should be max 2 chars before ':'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit minute

    str_minute = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
    int minute = StringH::Parse_Int( str_minute );     // parse month string to an integer
    free(str_minute);

    // PARSE THE SECOND
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "SECOND" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit SECOND
    substr_end_index = strlen(time_string)-1;          // end of substring containing one or two digit SECOND

    str_second = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
    int second = StringH::Parse_Int( str_second );     // parse month string to an integer
    free(str_second);

    Time* time_ptr = new Time(hour, minute, second);

    return time_ptr;
}

Time* Time::Parse_Timestamp(char* timestamp_string)
{
    char *str_hour, *str_minute, *str_second;
    int look_ahead_index = 0;
    int substr_start_index=0, substr_end_index=0;

    // PARSE THE HOUR
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit hour
    while( timestamp_string[look_ahead_index] != ':' && timestamp_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first ':', should be max 2 chars before ':'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit hour

    str_hour = StringH::Get_Substring(timestamp_string, substr_start_index, substr_end_index);
    int hour = StringH::Parse_Int( str_hour );     // parse month string to an integer
    free(str_hour);

    // PARSE THE MINUTE
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "MINUTE" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit minute
    while( timestamp_string[look_ahead_index] != ':' && timestamp_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first ':', should be max 2 chars before ':'
    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit minute

    str_minute = StringH::Get_Substring(timestamp_string, substr_start_index, substr_end_index);
    int minute = StringH::Parse_Int( str_minute );     // parse month string to an integer
    free(str_minute);

    // PARSE THE SECOND
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "SECOND" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit SECOND
    substr_end_index = strlen(timestamp_string)-1;          // end of substring containing one or two digit SECOND

    str_second = StringH::Get_Substring(timestamp_string, substr_start_index, substr_end_index);
    int second = StringH::Parse_Int( str_second );     // parse month string to an integer
    free(str_second);

    Time* time_ptr = new Time(hour, minute, second);
    return time_ptr;
}

Date* Date::Parse_Date(char* date_string)
{
    StringH::Trim_WhiteSpace(date_string);

    int look_ahead_index = 0;
    int substr_start_index, substr_end_index, substring_length;
    char field_delimiter = '-';
    char *str_month, *str_day, *str_year, *str_hour, *str_minute, *str_second;
    int month, day, year, hour, minute, second;

    if( StringH::String_Contains(date_string, "/") && !StringH::String_Contains(date_string, "-") )
        field_delimiter = '/';

    // PARSE MONTH
    if( isspace(date_string[look_ahead_index]) )
    {
        while( isspace(date_string[look_ahead_index]) )
            ++look_ahead_index;
    }
    substr_start_index = look_ahead_index;
    while( date_string[look_ahead_index] != field_delimiter && date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index first '-', should be max 2 chars before '-'
    substr_end_index = look_ahead_index -1;
    substring_length = substr_end_index - substr_start_index;
    if( substring_length > 2 || substring_length < 1)             // max 2 characters for "str_month", but at least one character
        return NULL;
    else
    {

        str_month = StringH::Get_Substring(date_string, substr_start_index, substr_end_index);
        if( !StringH::Contains_Only_Digits(str_month) )
            return NULL;;
        month = StringH::Parse_Int( str_month );     // parse month string to an integer
        if( month < 1 || month > 12)
        {
            printf("Error: month \"%d\" is not in valid month range (1-12)\n", month);
            return NULL;
        }
        free(str_month);
    }
    // PARSE "DAY OF MONTH" PART OF STRING
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "DAY" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit month day
    while( date_string[look_ahead_index] != field_delimiter && date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of second '-', should be max 2 chars before '-'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit day of month
    substring_length = substr_end_index - substr_start_index + 1;
    if( substring_length > 2 || substring_length < 1)             // max 2 characters for "day", but at least one character
        return NULL;
    else
    {
        str_day = StringH::Get_Substring(date_string, substr_start_index, substr_end_index);
        if( !StringH::Contains_Only_Digits(str_day) )
            return NULL;
        day = StringH::Parse_Int( str_day );     // parse month string to an integer
        if( day < 1 || day > 31)
        {
            printf("Error: day \"%d\" is not in valid day range (1-12)\n", day);
            return NULL;
        }
        free(str_day);
    }

    // PARSE YEAR PART OF THE STRING
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "YEAR" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing four or two digit YEAR
    while( date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index '_' marking end of year

    substr_end_index = look_ahead_index-1;          // end of substring containing four or two digit year
    substring_length = substr_end_index - substr_start_index+1;
    if( substring_length != 4 && substring_length != 2)             // max 4 characters for "year", but at least two characters
    {
        printf("Error: year in time string \"%s\" is not either 4 or 2 digits\n", date_string);
        printf("calculated length and indices- len: %d   start_ind: %d    end_ind:   %d\n", substring_length, substr_start_index, substr_end_index);
        str_year = StringH::Get_Substring(date_string, substr_start_index, substr_end_index);
        printf("calculated substr:   \"%s\"\n", str_year);
        free(str_year);
        return NULL;
    }
    else
    {
        str_year =StringH::Get_Substring(date_string, substr_start_index, substr_end_index);
        year = StringH::Parse_Int( str_year );     // parse month string to an integer
        if( !StringH::Contains_Only_Digits(str_year) )
        {
            free(str_year);
            return NULL;
        }
        free(str_year);
    }

    Date* date = new Date(month, day, year);
    return date;
}


/*
Date* Date::Parse_Date(char* date_string)
{
    Date* date_ptr = (Date*)malloc( sizeof( Date ) );

    char *str_month, *str_day, *str_year;
    int month, day, year;
    int look_ahead_index = 0;
    int substr_start_index=0, substr_end_index=0;

    // PARSE MONTH PART OF THE STRING
    while( date_string[look_ahead_index] != '/' && date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index first '/', should be max 2 chars before '/'
    substr_end_index = look_ahead_index -1;

    str_month = StringH::Get_Substring(date_string, substr_start_index, substr_end_index);
    month = StringH::Parse_Int( str_month );     // parse month string to an integer
    free(str_month);

    // PARSE "DAY OF MONTH" PART OF STRING
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "DAY" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit month day
    while( date_string[look_ahead_index] != '/' && date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of second '/', should be max 2 chars before '/'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit day of month
    str_day = StringH::Get_Substring(date_string, substr_start_index, substr_end_index);
    day = StringH::Parse_Int( str_day );     // parse month string to an integer
    free(str_day);

    // PARSE YEAR PART OF THE STRING
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "YEAR" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing four or two digit YEAR
    while( date_string[look_ahead_index] != '_' && date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index '_' marking end of year

    substr_end_index = look_ahead_index-1;          // end of substring containing four or two digit year
    int substring_length = substr_end_index - substr_start_index + 1;
    if( substring_length != 4 && substring_length != 2)             // max 4 characters for "year", but at least two characters
    {
        printf("Error: year in time string \"%s\" is not either 4 or 2 digits\n", date_string);
    }
    else
    {
        str_year = StringH::Get_Substring(date_string, substr_start_index, substr_end_index);
        year = StringH::Parse_Int( str_year );     // parse month string to an integer

        if( year < 2000 && substring_length == 4)
        {
            printf("Error: 4 digit year \"%d\" is not in range (more recent than 2000)\n", year);
        }
        else if( year < 2000 && substring_length == 2)
            year += 2000;
        free(str_year);
    }

    date_ptr->month = month;
    date_ptr->day = day;
    date_ptr->year = year;

    return date_ptr;
}
*/
