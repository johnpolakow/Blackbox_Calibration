


#include "Time2.h"
#include "String_Tokenizer.h"




Time2* Time2::Parse_Time(char* time_string)
{
    //printf("time string: %s\n", time_string);

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

    Time2* time_ptr = new Time2(hour, minute, second);
    return time_ptr;
}

Date2* Date2::Parse_Date(char* date_string)
{
    StringH::Trim_WhiteSpace(date_string);

    //printf("Date String: %s\n", date_string);

    int look_ahead_index = 0;
    int substr_start_index, substr_end_index, substring_length;
    char field_delimiter = '-';
    char *str_month, *str_day, *str_year, *str_hour, *str_minute, *str_second;
    int month, day, year, hour, minute, second;

    if( StringH::String_Contains(date_string, "/") && !StringH::String_Contains(date_string, "-") )
        field_delimiter = '/';


    //printf("delimiter: %c\n", field_delimiter);

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

    //printf("month: %d\n", month);
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

    //printf("day: %d\n", day);

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
    //printf("year: %d\n", year);
    Date2* date = new Date2(month, day, year);
    return date;
}
