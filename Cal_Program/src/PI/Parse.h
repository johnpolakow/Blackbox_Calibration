
#ifndef _PARSE_H
#define _PARSE_H
#include <math.h>

#include "String_Helper.h"
#include "./../Data_Helper.h"
#include "./../Struct_Defs.h"
#include "colors.h"

#ifdef __cplusplus
extern "C" {
#endif

int Parse_Relay(char* relay_string);
char* Parse_MAC(char* line);
char* Parse_IP(char* line);
char* Parse_First_Token(char* line, char delimiter);
char* Parse_First_Socket_Token(char* socket_string);
char* Parse_nTH_Token(char* line, int n, char delimiter);

double Parse_Scientific_Notation( char* notation_string);
int Parse_Int(char* integer_string);
double Parse_Double(char* double_string);

PI_Data_Point** Parse_PI_Data(char* pi_string);
PI_Data_Point* Parse_PI_String(char* pi_string);
void Free_PI_Data_Memory(PI_Data_Point** data);

Data_Group* Extract_Pi_Load_mA(PI_Data_Point** pi_data);
Data_Group* Extract_Pi_Load_V(PI_Data_Point** pi_data);
Data_Group* Extract_Pi_Cool_V(PI_Data_Point** pi_data);
Data_Group* Extract_Pi_Cool_mA(PI_Data_Point** pi_data);

void Free_PI_Load_Memory(PI_Load_Point** data);

Time* Parse_Time(char* time_string);
Date* Parse_Date(char* date_string);

int Index_First_Sample(char* string );
int Index_Second_Sample(char* string );
char* Extract_Value_in_String(char* line);
char* Extract_Key_in_String(char* line);
char* Parse_First_Comma(char* line);

void Trim_Lead_WhiteSpace(char* str);
void Trim_Trail_WhiteSpace(char * str);

void Free_PI_Data_Memory(PI_Data_Point** data)
{
        int i = 0;
        while( data[ i ] != NULL)
        {
            free( data[ i ] );
            data[i] = NULL;
            ++i;
        }
        free(data);
        data = NULL;
}

void Free_PI_Load_Memory(PI_Load_Point** data)
{
        int i = 0;
        while( data[ i ] != NULL)
        {
            free( data[ i ] );
            data[i] = NULL;
            ++i;
        }
        free(data);
        data = NULL;
}

PI_Data_Point** Parse_PI_Data(char* pi_string)
{
    PI_Data_Point** PI_Data_Array = (PI_Data_Point**)malloc( sizeof( PI_Data_Point*) * 100 );
    int pi_index = 0;

    char data_string[ strlen(pi_string) ];
    Erase_String_Contents( data_string );
    strcpy( data_string, pi_string );
    data_string[ strlen( pi_string) ] = '\0';

    while( String_Contains( data_string, "Sample#:" ) )
    {
        int index_first_sample = Index_First_Sample( data_string );
        int index_next_sample = Index_Second_Sample( data_string );
        //printf("index 1st: %d \t index 2nd: %d \n", index_first_sample, index_next_sample);

        if( index_next_sample == -1 || index_next_sample > strlen(data_string) )
        {
            PI_Data_Point* new_data_point;
            new_data_point = Parse_PI_String( data_string);
            PI_Data_Array[pi_index] = new_data_point;
            ++pi_index;
            PI_Data_Array[pi_index] = NULL;
            break;
        }

        char* parse_sample = Get_Substring( data_string, index_first_sample, index_next_sample );
        //printf("parse sample: \n" COLOR_YELLOW "\"%s\"\n\n" COLOR_RESET, parse_sample);
        PI_Data_Point* new_data_point;
        new_data_point = Parse_PI_String( parse_sample );
        PI_Data_Array[pi_index] = new_data_point;
        ++pi_index;
        PI_Data_Array[pi_index] = NULL;
        free(parse_sample);

        char* remaining_string = Get_Substring( data_string, index_next_sample, strlen(data_string)-1 );
        Erase_String_Contents(data_string);
        strcpy(data_string, remaining_string);
        free(remaining_string);
    }

    return PI_Data_Array;
}

PI_Data_Point* Parse_PI_String(char* pi_string)
{
    const char* delim = "\n";
    char* input_string = strdup( pi_string );

    char* token = strtok(input_string, delim);

    PI_Data_Point* new_data_point = (PI_Data_Point*)malloc( sizeof( PI_Data_Point ) );

    while( token != NULL )
    {
        // printf("token: %s\n", token);
        if( String_Contains(token, "Sample#") )
            new_data_point->sample_number = Parse_Int( Extract_Value_in_String(token) );
        if( String_Contains(token, "ADC0_CoolV:") )
        {
            new_data_point->ADC0_CoolV = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC0_CoolV! %f\n" COLOR_RESET, new_data_point->ADC0_CoolV);
        }
        else if( String_Contains(token, "ADC0_CoolA:") )
        {
            new_data_point->ADC0_CoolA = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC0_CoolA! %f\n" COLOR_RESET, new_data_point->ADC0_CoolA);
        }

        else if( String_Contains(token, "ADC1_CoolA:") )
        {
            new_data_point->ADC1_CoolA = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC1_CoolA! %f\n" COLOR_RESET, new_data_point->ADC1_CoolA);
        }
        else if( String_Contains(token, "ADC0_DiodeV:") )
        {
            new_data_point->ADC0_DiodeV = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC0_DiodeV! %f\n" COLOR_RESET, new_data_point->ADC0_DiodeV);
        }
        else if( String_Contains(token, "ADC1_LoadV:") )
        {
            new_data_point->ADC1_LoadV = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC1_LoadV! %f\n" COLOR_RESET, new_data_point->ADC1_LoadV);
        }
        else if( String_Contains(token, "ADC1_LoadmA:") )
        {
            new_data_point->ADC1_LoadmA = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC1_LoadmA! %f\n" COLOR_RESET, new_data_point->ADC1_LoadmA);
        }
        else if( String_Contains(token, "ADC0_CoolV_Raw:") )
        {
            new_data_point->ADC0_CoolV = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC0_CoolV_Raw! %f\n" COLOR_RESET, new_data_point->ADC0_CoolV_raw);
        }
        else if( String_Contains(token, "ADC0_CoolA_Raw:") )
        {
            new_data_point->ADC0_CoolA = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC0_CoolA_Raw! %f\n" COLOR_RESET, new_data_point->ADC0_CoolA_raw);
        }
        else if( String_Contains(token, "ADC1_CoolA_Raw:") )
        {
            new_data_point->ADC1_CoolA = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC1_CoolA_Raw! %f\n" COLOR_RESET, new_data_point->ADC1_CoolA_raw);
        }
        else if( String_Contains(token, "ADC1_LoadV_Raw:") )
        {
            new_data_point->ADC1_LoadV = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC1_LoadV_Raw! %f\n" COLOR_RESET, new_data_point->ADC1_LoadV_raw);
        }
        else if( String_Contains(token, "ADC1_LoadmA_Raw:") )
        {
            new_data_point->ADC1_LoadmA = Parse_Double( Extract_Value_in_String(token) );
            //printf(COLOR_BRIGHT_MAGENTA "\tparsed ADC1_LoadmA_Raw! %f\n" COLOR_RESET, new_data_point->ADC1_LoadmA_raw);
        }
        else if( String_Contains(token, "/202") ) // the data XX/XX/2020
        {
            char* time_string = Get_Substring( token, 0, Index_First_Whitespace(token) );
            char* date_string = Get_Substring(token, Index_First_Whitespace(token), strlen(token) -1 );

            Time* data_time = Parse_Time( time_string );
            free( time_string );

            Date* data_date = Parse_Date( date_string );
            free( date_string );

            new_data_point->time.hour = data_time->hour;
            new_data_point->time.minute = data_time->minute;
            new_data_point->time.second = data_time->second;
            free( data_time );

            new_data_point->date.month = data_date->month;
            new_data_point->date.day   = data_date->day;
            new_data_point->date.year  = data_date->year;
            free( data_date );
        }

        token = strtok(NULL, delim);
    }
    return new_data_point;
}

Data_Group* Extract_Pi_Load_mA(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* LoadmA_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    LoadmA_data->data_points = (float*)malloc( sizeof(float)* 12);
    LoadmA_data->num_points  = 0;
    strcpy(LoadmA_data->units, "mA");
    strcpy(LoadmA_data->description, "PI Load");

    while( pi_data[pi_index] != NULL )
    {
        LoadmA_data->data_points[pi_index] = pi_data[pi_index]->ADC1_LoadmA;
        ++LoadmA_data->num_points;
        ++pi_index;
    }
    LoadmA_data->average = Calc_Average( LoadmA_data );
    LoadmA_data->std_dev = Calc_Std_Dev( LoadmA_data );
    return LoadmA_data;
}

Data_Group* Extract_Pi_Load_V(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* LoadV_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    LoadV_data->data_points = (float*)malloc( sizeof(float)* 12);
    LoadV_data->num_points  = 0;

    strcpy(LoadV_data->units, "V");
    strcpy(LoadV_data->description, "PI Load");

    while( pi_data[pi_index] != NULL )
    {
        LoadV_data->data_points[pi_index] = pi_data[pi_index]->ADC1_LoadV;
        ++LoadV_data->num_points;
        ++pi_index;
    }
    LoadV_data->average = Calc_Average(LoadV_data);
    LoadV_data->std_dev = Calc_Std_Dev(LoadV_data);
    return LoadV_data;
}

Data_Group* Extract_Pi_Cool_V(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* CoolV_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    CoolV_data->data_points = (float*)malloc( sizeof(float)* 12);
    CoolV_data->num_points  = 0;

    strcpy(CoolV_data->units, "V");
    strcpy(CoolV_data->description, "PI COOLER");

    while( pi_data[pi_index] != NULL )
    {
        CoolV_data->data_points[pi_index] = pi_data[pi_index]->ADC0_CoolV;
        ++CoolV_data->num_points;
        ++pi_index;
    }
    CoolV_data->average = Calc_Average(CoolV_data);
    CoolV_data->std_dev = Calc_Std_Dev(CoolV_data);
    return CoolV_data;
}


Data_Group* Extract_Pi_Cool_mA(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* CoolmA_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    CoolmA_data->data_points = (float*)malloc( sizeof(float)* 12);
    CoolmA_data->num_points  = 0;

    strcpy(CoolmA_data->units, "mA");
    strcpy(CoolmA_data->description, "PI COOLER");

    while( pi_data[pi_index] != NULL )
    {
        CoolmA_data->data_points[pi_index] = pi_data[pi_index]->ADC0_CoolA;
        ++CoolmA_data->num_points;
        ++pi_index;
    }
    CoolmA_data->average = Calc_Average(CoolmA_data);
    CoolmA_data->std_dev = Calc_Std_Dev(CoolmA_data);
    return CoolmA_data;
}

Data_Group* Discard_Outlier_Points(Data_Group* data)
{
    // get average of remaining data points
    // get std deviation
    // throwout points more than XXX std deviation

    Data_Group* new_array;  // copy to new array if throwing out data points

    int data_index = 0;
    int num_samples = data->num_points;
    double AVERAGE = Calc_Average(data);
    double STD_DEV = Calc_Std_Dev(data);

    double variance;
    double cur_value;

    // throwout outliers (if variance greater than 1 standard deviation) by setting value to -1 (a flag)
    bool outliers_exist = false;
    for( int i = 0; i<num_samples; ++i)
    {
        cur_value = data->data_points[ i ];
        variance =abs(cur_value - AVERAGE);    // absolute value of difference
        if(variance > STD_DEV)
        {
            printf(COLOR_BOLD_RED "OUTLIER THROWN OUT: %.4f mA\n" COLOR_RESET, data->data_points[i] );
            data->data_points[i] = -1;
            outliers_exist = true;
        }
    }

    if(outliers_exist)
    {
        // copy input array, leaving out outlier points
        new_array = (Data_Group*)malloc( sizeof(Data_Group) ); // only taking 10 samples at a time
        new_array->data_points = (float*)malloc( sizeof(float) * 12);
        new_array->num_points = 0;
        strcpy(new_array->units, data->units);
        strcpy(new_array->description, data->description);
        int new_array_index = 0;

        // deep copy of valid data_points
        for(int i = 0; i<data->num_points; ++i)
        {
            if(data->data_points[i] != -1.0 && data->data_points[i] != -1)
            {
                new_array->data_points[new_array_index] = data->data_points[i];
                ++new_array_index;
                ++new_array->num_points;
            }
        }
        Free_Data_Group_Memory(data);
        new_array->average = Calc_Average(new_array);
        new_array->std_dev = Calc_Std_Dev(new_array);
        return new_array;
    }
    else
    {
        return data;
    }
}



int Index_First_Sample(char* string )
{
    char* first_match_ptr = strstr( string, "Sample#:");

    if( first_match_ptr == NULL )
        return -1;
    else
        return first_match_ptr - string;
}

int Index_Second_Sample(char* string )
{
    int index_first_match = Index_First_Sample( string );

    if( index_first_match == -1 )
        return -1;

    char* remaining_string = Get_Substring(string, index_first_match + strlen("Sample#:"), strlen(string)-1 );
    char* second_match_ptr = strstr( remaining_string, "Sample#:");
    if( second_match_ptr == NULL)
        return -1;
    free(remaining_string);

    int second_match_index = second_match_ptr - remaining_string + index_first_match + strlen("Sample#:");

    return second_match_index;
}

char* Extract_Key_in_String(char* line)
{
   	char read_line[512];

   	if( line == NULL )
    {
        printf("KeyValueError: line argument is NULL\n");
   		  return NULL;
   	}

    Trim_Lead_WhiteSpace(line);
    Trim_Trail_WhiteSpace(line);
    strcpy(read_line, line);
    if(read_line[0] == '#')   // if '#' begins a line in a file, the line is commented out and thus not active in the file
    {
        printf("ERROR: #\n");
        return NULL;
    }
    else if( read_line[0] == '/' && read_line[1] == '/') // if line begins with '//' it is considered commented out
    {
        printf("ERROR: // \n");
        return NULL;
    }

    int start_index = 0;
    int end_index = strlen(read_line) - 1;

    for(int i=0; i< strlen(read_line); ++i)
    {
        if( read_line[i]==':' || isspace(read_line[i]) )  // look for delimiter between key and value, which is either ':' or ' '
        {
            end_index = i-1;
            break;
        }

    }
    char* key = Get_Substring(read_line, start_index, end_index);
    return key;
 }

char* Extract_Value_in_String(char* line)
{
   	char read_line[512];
    bool found = false;

   	if( line == NULL )
    {
        printf("KeyValueError: line argument is NULL\n");
   		  return NULL;
   	}

    Trim_Lead_WhiteSpace(line);
    strcpy(read_line, line);
    if(read_line[0] == '#')   // if '#' begins a line in a file, the line is commented out and thus not active in the file
          return NULL;
    else if( read_line[0] == '/' && read_line[1] == '/') // if line begins with '//' it is considered commented out
          return NULL;

    int start_index = 0;
    int end_index = strlen(read_line) - 1;

    for(int i=0; i< strlen(read_line); ++i)
    {
        if( read_line[i]==':' || isspace(read_line[i]) )  // look for delimiter between key and value, which is either ':' or ' '
        {
            found = true;
            start_index = i+1;
            break;
        }
    }
    if( !found )
        return NULL;

    char* value = Get_Substring(read_line, start_index, end_index);
        return value;
 }

Time* Parse_Time(char* time_string)
{
    Time* time_ptr = (Time*)malloc( sizeof( Time) );

    char *str_hour, *str_minute, *str_second;
    int look_ahead_index = 0;
    int substr_start_index=0, substr_end_index=0;

    // PARSE THE HOUR
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit hour
    while( time_string[look_ahead_index] != ':' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first ':', should be max 2 chars before ':'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit hour

    str_hour = Get_Substring(time_string, substr_start_index, substr_end_index);
    int hour = Parse_Int( str_hour );     // parse month string to an integer
    free(str_hour);

    // PARSE THE MINUTE
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "MINUTE" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit minute
    while( time_string[look_ahead_index] != ':' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first ':', should be max 2 chars before ':'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit minute

    str_minute = Get_Substring(time_string, substr_start_index, substr_end_index);
    int minute = Parse_Int( str_minute );     // parse month string to an integer
    free(str_minute);

    // PARSE THE SECOND
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "SECOND" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit SECOND
    substr_end_index = strlen(time_string)-1;          // end of substring containing one or two digit SECOND

    str_second = Get_Substring(time_string, substr_start_index, substr_end_index);
    int second = Parse_Int( str_second );     // parse month string to an integer
    free(str_second);

    //if( hour < 0 || minute<0 || second<0)
      ///  printf("neg time parse: %d:%d:%d \t %s\n", hour, minute, second, time_string);
    time_ptr->hour = hour;
    time_ptr->minute = minute;
    time_ptr->second = second;
    //printf("hour: %d\n", hour);
    //printf("minute: %d\n", minute);
    //printf("second: %d\n", second);

    return time_ptr;
}

Date* Parse_Date(char* date_string)
{
    Date* date_ptr = (Date*)malloc( sizeof( Date ) );

    char *str_month, *str_day, *str_year;
    int month, day, year;
    int look_ahead_index = 0;
    int substr_start_index=0, substr_end_index=0;

    // PARSE MONTH PART OF THE STRING
    while( date_string[look_ahead_index] != '/' && date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index first '/', should be max 2 chars before '/'
    substr_end_index = look_ahead_index -1;

    str_month = Get_Substring(date_string, substr_start_index, substr_end_index);
    month = Parse_Int( str_month );     // parse month string to an integer
    free(str_month);

    // PARSE "DAY OF MONTH" PART OF STRING
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "DAY" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit month day
    while( date_string[look_ahead_index] != '/' && date_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of second '/', should be max 2 chars before '/'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit day of month
    str_day = Get_Substring(date_string, substr_start_index, substr_end_index);
    day = Parse_Int( str_day );     // parse month string to an integer
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
        str_year = Get_Substring(date_string, substr_start_index, substr_end_index);
        year = Parse_Int( str_year );     // parse month string to an integer

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

    //printf("month: %d\n", month);
    //printf("day: %d\n", day);
    //printf("year: %d\n", year);

    return date_ptr;
}

double Parse_Scientific_Notation( char* notation_string)
{
    // if contains 'E'
        // get index of e
        // get substring after e
        // parse substring to integer exponent
        // parse substring before e to double
        // answer = leading double * 10^exponent

    int E_index = Index_of_E( notation_string );
    char* double_string = Get_Substring(notation_string, 0, E_index);
    double base_val = Parse_Double( double_string );
    free(double_string);

    char* exponent_string = Get_Substring(notation_string, E_index+1, strlen(notation_string)-1);
    int exponent = (int)strtol(exponent_string, NULL, 10);
    free(exponent_string);

    double tens_multiplier = pow(10, exponent);
    double float_value = base_val * tens_multiplier;

    return float_value;
}

int Parse_Int(char* integer_string)
{
   char* end_ptr;
   int parsed_int = strtol(integer_string, &end_ptr, 10);   // parse the substring to integer using stringtolong function
   return parsed_int;
 }

double Parse_Double(char* double_string)
{
    bool is_negative = false;
    Trim_Trail_WhiteSpace(double_string);

    // Get index of start of integer portion. Could be a digit or + or -
    int start_index = 0;
    for(int i = 0; i<strlen(double_string); ++i )
    {
        if(double_string[i] == '+' || double_string[i] == '-')
        {
            if( isdigit(double_string[i+1]) && double_string[i] == '-' )
            {
                start_index = i+1;
                is_negative = true;
                break;
            }
        }
        if( isdigit(double_string[i]) )
        {
            start_index = i;
            break;
        }
    }

    // get index of last digit
    int end_index = start_index;                              // start search of last "digit" character at the beginning of the integer portion
    for(int i=start_index; i<strlen(double_string); ++i )
    {
        if( isdigit( double_string[i] ) )
        {
            end_index = i;
            continue;
        }
        else
        {
            if( double_string[i] == '.' && isdigit( double_string[ i +1 ] ) )
            {
                end_index = i+1;
                continue;
            }
            else
                break;
        }
     }

     char* integer_substring = Get_Substring(double_string, start_index, end_index);
     integer_substring[ strlen(integer_substring) ] = '\0';    // add null terminating character

     char* end_ptr;
     double parsed_double = strtod(integer_substring, &end_ptr);   // parse the substring to integer using stringtolong function
     if( is_negative )
        parsed_double *= -1;
    free(integer_substring);

     return parsed_double;
 }

char* Parse_First_Token(char* line, char delimiter)
{
    Trim_Lead_WhiteSpace(line);
    int start_index = 0;
    int index_first_delim = Index_First_Delimiter( line, delimiter );   // first character of whitespace after start of token

    return Get_Substring( line, start_index, index_first_delim-1);
}

char* Parse_First_Socket_Token(char* socket_string)
{
    Trim_Lead_WhiteSpace(socket_string);
    int start_index = 0;
    if( socket_string[ start_index ] == '>' || socket_string[ start_index ] == '?')             // skip first character (0 index), which should be a '>' or a '?' to indicate a command or query
        start_index += 1;

    int index_first_whitespace = Index_First_Whitespace( socket_string );   // first character of whitespace after start of token

    return Get_Substring( socket_string, start_index, index_first_whitespace);
}


char* Parse_nTH_Token(char* line, int n, char delimiter)
{
    Trim_Lead_WhiteSpace(line);
    int start_index = 0;
    int index_delim_n_previous = Index_nTH_Delimiter( line, n-1, delimiter );   // first character of whitespace after start of token
    int index_delim_n          = Index_nTH_Delimiter( line, n, delimiter );              // first character of whitespace after start of token

    return Get_Substring( line, index_delim_n_previous+1, index_delim_n-1 );
}


 // trim leading whitespace
 // get first char, may be k,K, or 1-9
 // if k or K, skip it
 // consume chars until whitespace or end of string
 // after 1st digit, verify only one additional digit
 // if it is valid 1 or two digits, parse to int
 // if int is between 1 and 15, return it, else return -1
 // print error message if invalid input
int Parse_Relay(char* relay_string)
{
     Trim_Lead_WhiteSpace( relay_string );
     Trim_Trail_WhiteSpace( relay_string );

     //printf("Parse string is: %s\n", relay_string);
     int cur_index = 0;
     int first_digit_index = -1;  //

     char cur_char = relay_string[cur_index];
     while(cur_char != '\0')                     // loop through until end of string
     {
        if( isspace(cur_char) )
        {
            if( first_digit_index != -1 )       // have seen a digit already, so break out of loop
                break;
            else
            {
                ++cur_index;
                cur_char = relay_string[cur_index];        // update current character being examined
            }
        }

        if( first_digit_index == -1)
        {
            if( (cur_char == 'k' || cur_char == 'K') && isdigit( relay_string[cur_index + 1] ) )
            {
                first_digit_index = cur_index+1;
                cur_index+=2;
                cur_char = relay_string[cur_index];        // update current character being examined
                continue;
            }
            else if( isdigit(cur_char) )
            {
                first_digit_index = cur_index;
                ++cur_index;
                cur_char = relay_string[cur_index];        // update current character being examined
                continue;
            }
            else return -1;     // Error, parse string is not of the form K15 or 15
        }

        if( isdigit( cur_char ) )
            ++cur_index;
        else
            break;

        cur_char = relay_string[cur_index];        // update current character being examined
     }
     int end_index = cur_index;
     char* int_substring = Get_Substring(relay_string, first_digit_index, end_index);
     //printf("substring is: %s\n", int_substring);

     int relay_number = Parse_Int( int_substring );
     free( int_substring );
     //printf("parsed int is: %d\n", relay_number);

     if( (relay_number < 1) || (relay_number > 17) )    // outside of valide range
            return -1;

     return relay_number;
  }


char* Parse_First_Comma(char* line)
{
    Trim_Lead_WhiteSpace(line);
    int start_index = 0;
    int index_first_comma = Index_First_Delimiter( line, ',' );   // first character of whitespace after start of token

    return Get_Substring( line, start_index, index_first_comma);
}



char* Parse_MAC(char* line)
{
    int index_first_space = Index_nTH_Delimiter( line, 1, ' ' );
    int index_second_space = Index_nTH_Delimiter( line, 2, ' ' );
    char* mac_addr = Get_Substring(line, index_first_space, index_second_space);
    return mac_addr;
}


char* Parse_IP(char* line)
{
    int index_first_space = Index_nTH_Delimiter( line, 1, ' ' );
    char* ip_addr = Get_Substring(line, 0, index_first_space);
    return ip_addr;
}


void Trim_Trail_WhiteSpace(char * str)                              // removes trailing whitespace (spaces and newlines)
{

   int stringLen = strlen(str);
   while(stringLen > 0 && isspace(str[stringLen -1]) )
   {
      stringLen--;
   }
   str[stringLen] = '\0';
}

void Trim_Lead_WhiteSpace(char* str)
{
    int start_index=0, new_index=0, old_index=0;

    // find index of first non-whitespace character
    while(str[start_index] == ' ' || str[start_index] == '\t' || str[start_index] == '\n')
    { ++start_index;  }

    if(start_index != 0)
    {
        // copy all characters over to the left
        old_index = start_index;
        while( str[ old_index ] != '\0' )
        {
            str[ new_index ] = str[ old_index ];
            ++new_index;
            ++old_index;
        }
        str[new_index] = '\0'; // Make sure that string is NULL terminated
    }
}

#ifdef __cplusplus
}
#endif

#endif
