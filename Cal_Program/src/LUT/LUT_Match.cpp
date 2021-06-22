


#include "LUT_Match.h"

void LUT_Match::Print_Match(LUT_Match* match)
{
    for(int i = 0; i< match->num_Yokogawa_values; ++i)
    {
        double Avg_Difference = match->pi_value - match->Yokogawa_Avg;
        if( i == 0)
        {
            if( Avg_Difference > 0)
                printf("%s    " COLOR_MAGENTA "%03.3f %s" COLOR_RESET "  %03.3f %s\t\t" COLOR_YELLOW "Yokogawa AVG: %.3f" COLOR_RESET COLOR_BOLD_GREEN"\t\t\t+%.3f\n" COLOR_RESET, \
                match->data_timestamp->time.toString(), match->pi_value, match->units, match->Yokogawa_values[i], match->units, match->Yokogawa_Avg, Avg_Difference );
            if( Avg_Difference < 0)
                printf("%s    " COLOR_MAGENTA "%03.3f %s" COLOR_RESET "  %03.3f %s\t\t" COLOR_YELLOW "Yokogawa AVG: %.3f" COLOR_RESET COLOR_BOLD_RED"\t\t\t%.3f\n" COLOR_RESET, \
                match->data_timestamp->time.toString(), match->pi_value, match->units, match->Yokogawa_values[i], match->units, match->Yokogawa_Avg, Avg_Difference );
        }
        if( i == 1)
            printf("                      \t%.3f %s \t\tYokogawa STD DEV: %.3f\n", match->Yokogawa_values[i], match->units, match->Yokogawa_Std_Dev );
        if( i == 2)
            printf("                      \t%.3f %s \t\tYokogawa SPREAD: %.3f\n", match->Yokogawa_values[i], match->units, match->Yokogawa_spread );
        if(i >= 3)
            printf("                      \t%.3f %s\n", match->Yokogawa_values[i], match->units);
    }
}

char* LUT_Match::toString_Header()
{
    char header_string[200];
    StringH::Erase_Num_Chars( header_string, ARRAY_SIZE(header_string) );

    int write_index = 0;

    StringH::Write_Chars(header_string, &write_index, "timestamp", MAX_TIMESTAMP_CHARS);

    StringH::Advance_to_Index(header_string, &write_index, COL_INDEX_PI_MA);
    StringH::Write_Chars(header_string, &write_index, "Pi mA", MAX_PI_CHARS );

    StringH::Advance_to_Index(header_string, &write_index, COL_INDEX_BK5335_MA);
    StringH::Write_Chars(header_string, &write_index, "YK mA", MAX_BK_CHARS );

    StringH::Advance_to_Index(header_string, &write_index, COL_INDEX_BK_AVG);
    StringH::Write_Chars(header_string, &write_index, "YK AVG VAL", MAX_BK_AVG_CHARS );

    StringH::Advance_to_Index(header_string, &write_index, COL_INDEX_BK_STD_DEV);
    StringH::Write_Chars(header_string, &write_index, "YK STD DEV", MAX_BK_STD_DEV_CHARS );

    StringH::Advance_to_Index(header_string, &write_index, COL_INDEX_BK_SPREAD);
    StringH::Write_Chars(header_string, &write_index, "YK SPREAD", MAX_BK_SPREAD );

    return strdup(header_string);
}

char* LUT_Match::toString(LUT_Match* match)
{
    char timestamp[20];
    char pi_mA_string[TEMP_STRING_SIZE];
    char YK_mA_string[150];
    char YK_avg_string[TEMP_STRING_SIZE];
    char YK_std_dev_string[TEMP_STRING_SIZE];
    char Yokogawa_spread_string[TEMP_STRING_SIZE];
    char temp[TEMP_STRING_SIZE];

    StringH::Erase_Num_Chars( timestamp, 20 );
    char* match_tstamp = match->data_timestamp->time.toString();
    sprintf(timestamp, "%s", match_tstamp );
    free(match_tstamp);

    StringH::Erase_Num_Chars( pi_mA_string, TEMP_STRING_SIZE);
    sprintf(pi_mA_string, "[%.2f]", match->pi_value );

    StringH::Erase_Num_Chars( YK_mA_string, 100);
    StringH::Erase_Num_Chars( temp, TEMP_STRING_SIZE);

    int loop_iterations;
    if(match->num_Yokogawa_values > 8)
        loop_iterations = 8;
    else
    {
        loop_iterations = match->num_Yokogawa_values;
    }
    
    for(int i = 0; i< loop_iterations; ++i)
    {
        if( i == 0 && match->num_Yokogawa_values == 1)
        {
            StringH::Erase_Num_Chars(temp, TEMP_STRING_SIZE );
            sprintf(temp, "[%.2f]", match->Yokogawa_values[i]);
            strcat(YK_mA_string, temp );
            break;
        }
        else if( i == 0 && match->num_Yokogawa_values > 1 )
        {
            StringH::Erase_Num_Chars(temp, TEMP_STRING_SIZE );
            sprintf(temp, "[%.2f, \0", match->Yokogawa_values[i]);
            strcat(YK_mA_string, temp );
        }
        else if( i==(loop_iterations-1) ) // if its the last value, don't add a comma after it
        {
            StringH::Erase_Num_Chars(temp, TEMP_STRING_SIZE );
            sprintf(temp, "%.2f]\0", match->Yokogawa_values[i]);
            strcat(YK_mA_string, temp );
        }
        else
        {
            //printf("match->Yokogawa_values[i]= %.2f \t I: %d\n", match->Yokogawa_values[i], i);
            StringH::Erase_Num_Chars(temp, TEMP_STRING_SIZE );
            sprintf(temp, "%.2f, \0", match->Yokogawa_values[i]);
            strcat(YK_mA_string, temp );
        }
    }
    //printf("YK mA string: %s\n", YK_mA_string);

    StringH::Erase_Num_Chars( YK_avg_string, TEMP_STRING_SIZE );
    StringH::Erase_Num_Chars( YK_std_dev_string, TEMP_STRING_SIZE );
    StringH::Erase_Num_Chars( Yokogawa_spread_string, TEMP_STRING_SIZE );
    sprintf(YK_avg_string, "AVG[%.2f]", match->Yokogawa_Avg );
    sprintf(YK_std_dev_string, "SD[%.3f]", match->Yokogawa_Std_Dev );
    sprintf(Yokogawa_spread_string, "SPR[%.2f]", match->Yokogawa_spread );

    int cur_index = 0;  // write index in the output string
    char* string_object = (char*)malloc( sizeof(char) * 150);   // allocate memory for the output string
    StringH::Erase_Num_Chars( string_object, 150);

    StringH::Write_Chars(string_object, &cur_index, timestamp, MAX_TIMESTAMP_CHARS);

    StringH::Advance_to_Index(string_object, &cur_index, COL_INDEX_PI_MA);
    StringH::Write_Chars(string_object, &cur_index, pi_mA_string, MAX_PI_CHARS );

    StringH::Advance_to_Index(string_object, &cur_index, COL_INDEX_BK5335_MA);
    StringH::Write_Chars(string_object, &cur_index, YK_mA_string, MAX_BK_CHARS );

    StringH::Advance_to_Index(string_object, &cur_index, COL_INDEX_BK_AVG);
    StringH::Write_Chars(string_object, &cur_index, YK_avg_string, MAX_BK_AVG_CHARS );

    StringH::Advance_to_Index(string_object, &cur_index, COL_INDEX_BK_STD_DEV);
    StringH::Write_Chars(string_object, &cur_index, YK_std_dev_string, MAX_BK_STD_DEV_CHARS );

    StringH::Advance_to_Index(string_object, &cur_index, COL_INDEX_BK_SPREAD);
    StringH::Write_Chars(string_object, &cur_index, Yokogawa_spread_string, MAX_BK_SPREAD );

    return (string_object);
}

double LUT_Match::Calc_Yokogawa_Average()
{
    double data_sum = 0;
    int num_samples = 0;

    for(int i=0; i< this->num_Yokogawa_values; ++i)
    {
        if(this->Yokogawa_values[i] != -1)
        {
            data_sum += Yokogawa_values[i];
            ++num_samples;
        }
    }
    this->Yokogawa_Avg = data_sum / num_samples;
    return this->Yokogawa_Avg;
}

double LUT_Match::Calc_Yokogawa_Std_Dev()
{
    double SD_sum = 0;

    // calc standard deviation
    for(int i = 0; i< this->num_Yokogawa_values; ++i)
    {
        SD_sum += pow(this->Yokogawa_values[i]-this->Yokogawa_Avg, 2);
    }
    this->Yokogawa_Std_Dev = sqrt(SD_sum/(float)this->num_Yokogawa_values);
    return this->Yokogawa_Std_Dev;
}

double LUT_Match::Calc_Yokogawa_Spread()
{
    double min_sample_value;
    double max_sample_value;

    if( this->num_Yokogawa_values > 0)
    {
        min_sample_value = this->Yokogawa_values[0];
        max_sample_value = this->Yokogawa_values[0];
    }

    for(int i = 0; i< this->num_Yokogawa_values; ++i)
    {
        if( this->Yokogawa_values[i] < min_sample_value )
            min_sample_value = this->Yokogawa_values[i];
        if( this->Yokogawa_values[i] > max_sample_value )
            max_sample_value = this->Yokogawa_values[i];
    }

    this->Yokogawa_spread = abs(max_sample_value - min_sample_value);
    return this->Yokogawa_spread;
}
