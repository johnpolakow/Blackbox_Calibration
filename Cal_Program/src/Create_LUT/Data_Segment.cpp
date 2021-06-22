
#include "Data_Segment.h"







void Data_Segment::Add_Line(Data_Line* add_line)
{
    if(num_data_lines < DATA_LINES_PER_SEGMENT )
    {
        if( num_data_lines == 0)
        {
            // extract time from line
            Time* data_start_time = add_line->time;
            this->start_timestamp = new TimeStamp(data_start_time, this->date);
            this->Lines[ this->num_data_lines ] = add_line;
            ++(this->num_data_lines);
        }
        else
        {
            this->Lines[ this->num_data_lines ] = add_line;
            ++(this->num_data_lines);
        }
    }
    else
        return;
}

// Data Segment is of the form:
/*
---------------------------------------------------------------------------

01-25-21
timestamp   Pi mA       YK mA                                                 YK AVG VAL    YK STD DEV  YK SPREAD   
16:17:01    [394.32]    [383.60, 383.60, 401.90, 401.90, 401.90, 401.90]    AVG[401.47]   SD[7.157]   SPR[21.70]  
16:17:01    [405.63]    [383.60, 383.60, 401.90, 401.90, 401.90, 401.90]    AVG[401.47]   SD[7.157]   SPR[21.70]  
16:17:01    [406.32]    [383.60, 383.60, 401.90, 401.90, 401.90, 401.90]    AVG[401.47]   SD[7.157]   SPR[21.70]  
16:17:02    [406.72]    [406.20, 406.20, 406.20, 406.20, 406.20, 406.30]    AVG[406.27]   SD[0.057]   SPR[0.20]   
16:17:02    [407.01]    [406.20, 406.20, 406.20, 406.20, 406.20, 406.30]    AVG[406.27]   SD[0.057]   SPR[0.20]   
16:17:02    [407.42]    [406.20, 406.20, 406.20, 406.20, 406.20, 406.30]    AVG[406.27]   SD[0.057]   SPR[0.20]   
16:17:03    [407.92]    [407.40, 407.70, 407.70, 407.70, 407.70, 407.60]    AVG[407.62]   SD[0.075]   SPR[0.30]   
16:17:03    [408.14]    [407.40, 407.70, 407.70, 407.70, 407.70, 407.60]    AVG[407.62]   SD[0.075]   SPR[0.30]   
16:17:03    [408.51]    [407.40, 407.70, 407.70, 407.70, 407.70, 407.60]    AVG[407.62]   SD[0.075]   SPR[0.30]   
16:17:03    [408.99]    [407.40, 407.70, 407.70, 407.70, 407.70, 407.60]    AVG[407.62]   SD[0.075]   SPR[0.30]   
---------------------------------------------------------------------------
*/
Data_Segment* Data_Segment::Scan_Data_Segment(FILE* read_file)
{
    char readline[300];
    char TIMESTAMP[60];

    bool read_date = false;
    bool read_legend = false;
    bool reached_delimiter = false;

    //printf("scan segment\n");
    Data_Segment* segment = new Data_Segment();
    //while( fgets(readline, 256, read_file) != NULL)
    while( !reached_delimiter )
    {
        StringH::Erase_Num_Chars(readline, 300);
        if( fgets(readline, 256, read_file) == NULL )
        {
            //printf("NULL\n");
            delete segment;
            segment = NULL;
            return segment;
        }

        //printf("readline: '%s'\n", readline);

        if( StringH::Line_Is_Whitespace(readline) ) // if whitespace, continue
        {
            //printf("[whitespace]\n");
            continue;
        }

        if( !read_date && StringH::Line_Is_Date(readline) )
        {   // Determine what the line is:
            //printf("[date]\n");
            segment->date = Date::Parse_Date(readline);
            read_date = true;
            continue;
        }

        if( !read_legend && Data_Segment::Line_Is_Column_Legend(readline) )
        {
            //printf("[legend]\n");
            read_legend = true;
            continue;
        }

        if( Data_Line::Is_Data_Line(readline) ) // filters for less than 3 data points
        {
            //printf("[data_line]\n");
            Data_Line* new_data = new Data_Line(readline);
            int num_meter_samples = new_data->Get_Num_Meter_Data_Points();
            if( num_meter_samples < 3 )    // if there are less than 3 samples per second for the meter, data is not meaningful
            {
                delete new_data;
                continue;
            }
            else
            {
                segment->Add_Line(new_data);
                continue;
            }
        }

        if( Data_Segment::Line_Is_Segment_Delimiter(readline) )
        {
            //printf("[delimiter]\n");
            // marking the stop time of the segment
            int last_line_index = segment->Get_Num_Data_Lines() -1 ;
            Data_Line* last_line_added = segment->Lines[last_line_index];
            Time* data_stop_time = last_line_added->time;
            segment->stop_timestamp = new TimeStamp(data_stop_time, segment->date);

            char* start_str = segment->start_timestamp->toString();
            char* stop_str = segment->stop_timestamp->toString();

            free(start_str);
            free(stop_str);

            // Calculate Metrics:
            segment->Filter_Data_Lines();
            reached_delimiter = true;
            break;
        }
    }
    return segment;
}

int Data_Segment::Get_Num_Data_Lines()
{
    return this->num_data_lines;
}



void Data_Segment::Filter_Data_Lines()
{
    // calculate metrics
    this->Get_PI_Average_10sec();
    this->Get_Meter_Average_10sec();
    this->Get_PI_Spread_10sec();
    this->Get_Meter_Spread_10sec();
    this->Get_PI_Std_Dev();
    this->Get_Meter_Std_Dev();


    int pi_num_over_variance_threshold = 0;
    int meter_num_over_variance_threshold = 0;
    for(int i =0; i< this->num_data_lines; ++i)
    {
        Data_Line* line = this->Lines[i];
        // get variance for each line
        double pi_line_val = line->Pi_value;
        double meter_line_val = line->Meter_Avg;

        double pi_line_variance = abs(pi_line_val - this->PI_Avg_10_sec);
        double meter_line_variance = abs(meter_line_val - this->Meter_Avg_10_sec);

        line->Pi_variance = pi_line_variance;
        line->Meter_variance = meter_line_variance;

        double pi_line_percent_variance = pi_line_variance / this->PI_Avg_10_sec;
        double meter_line_percent_variance = meter_line_variance / this->Meter_Avg_10_sec;

        if( pi_line_percent_variance >= 1)
            ++pi_num_over_variance_threshold;

        if( meter_line_percent_variance >= 1)
            ++meter_num_over_variance_threshold;
        
        if( pi_line_percent_variance >= MAX_DATA_POINT_VARIANCE || meter_line_percent_variance >= MAX_DATA_POINT_VARIANCE)
        {
            char* time_str = line->time->toString();
            printf("timestamp: %s. ", time_str);
            free(time_str);

            double max_variance = 0;
            if( pi_line_percent_variance < meter_line_percent_variance)
                max_variance = meter_line_percent_variance;
            else
                max_variance = pi_line_percent_variance;

            this->ignore_segment = true;

            if(pi_line_percent_variance > meter_line_percent_variance)
            {
                printf("Deleting this segment, contains PI data point w/ variance greater than %d%% allowed: %.2f\n", MAX_DATA_POINT_VARIANCE, pi_line_val);
                //printf("\tPI variance: %.2f%% \tPI val: %.2f , PI AVG: %.2f\n", pi_line_percent_variance, pi_line_val, this->PI_Avg_10_sec);
                printf("\t\tPI vals: [ ");
                for(int h =0; h< this->num_data_lines; ++h)
                {
                    printf( COLOR_RED "%.2f,  " COLOR_RESET, this->Lines[h]->Pi_value);
                }
                printf("]\n");
            }
            else
            {
                printf("Deleting this segment, Meter data point w/ variance greater than %d%% : %.2f\n", MAX_DATA_POINT_VARIANCE, meter_line_val);
                printf("\tMeter variance: %.2f%% \tMeter val: %.2f , METER AVG: %.2f\n", meter_line_percent_variance, meter_line_val, this->Meter_Avg_10_sec);
                printf("\t\tMeter vals: [ ");
                for(int h =0; h< this->num_data_lines; ++h)
                {
                    printf( COLOR_RED "%.2f,  " COLOR_RESET, this->Lines[h]->Meter_Avg);
                }
                printf("]\n");
            }
            printf("\n");
            break;
        }
    }
    if(pi_num_over_variance_threshold > 3)
    {
        printf("Ignoring this segment, %d/%d Pi vals with large variance: \n", pi_num_over_variance_threshold, this->num_data_lines);
        this->ignore_segment = true;
    }

    if(meter_num_over_variance_threshold > 3)
    {
        printf("Ignoring segment, %d/%d Meter vals with large variance: \n", meter_num_over_variance_threshold, this->num_data_lines);
        printf("\t\tMeter Vals: [ ");
        for(int k = 0; k < this->num_data_lines; ++k)
        {
            printf( COLOR_RED "%.2f, " COLOR_RESET, this->Lines[k]->Meter_Avg);
        }
        printf("]\n");
        printf("\n");
        this->ignore_segment = true;
    }
}

double Data_Segment::Get_PI_Average_10sec()
{
    double pi_sum = 0;
    for( int i = 0; i < this->num_data_lines; ++i)
    {
        pi_sum += this->Lines[i]->Pi_value;
    }
    this->PI_Avg_10_sec = pi_sum/this->num_data_lines;
    return this->PI_Avg_10_sec;
}

double Data_Segment::Get_Meter_Average_10sec()
{
    double meter_sum_10sec = 0;
    for( int i = 0; i <this->num_data_lines; ++i)
    {
        double meter_sum_1sec = 0;
        int num_meter_points = this->Lines[i]->Get_Num_Meter_Data_Points(); // # of BK samples for this second of data
        for(int j=0; j<num_meter_points; ++j)
        {
            meter_sum_1sec += this->Lines[i]->Meter_Data[j];
        }
        double meter_Avg_1_sec = meter_sum_1sec / (double)num_meter_points;

        meter_sum_10sec += meter_Avg_1_sec;
    }
    this->Meter_Avg_10_sec = meter_sum_10sec/(double)this->num_data_lines;
    return this->Meter_Avg_10_sec;
}

double Data_Segment::Get_PI_Spread_10sec()
{
    double pi_min = this->Lines[0]->Pi_value;
    double pi_max = this->Lines[0]->Pi_value;

    for( int i = 0; i <this->num_data_lines; ++i)
    {
        double pi_val = this->Lines[i]->Pi_value;

        if( pi_val > pi_max)
            pi_max = pi_val;
        else if(pi_val < pi_min)
            pi_min = pi_val;
    }
    this->PI_Spread_10_sec = pi_max - pi_min;
    return this->PI_Spread_10_sec;
}

double Data_Segment::Get_Meter_Spread_10sec()
{
    double meter_min = this->Lines[0]->Meter_Avg;
    double meter_max = this->Lines[0]->Meter_Avg;
    for( int i = 0; i < this->num_data_lines; ++i)
    {
        double meter_val = this->Lines[i]->Meter_Avg;
        if( meter_val > meter_max)
            meter_max = meter_val;
        else if(meter_val < meter_min)
            meter_min = meter_val;
    }
    this->Meter_Spread_10_sec = meter_max - meter_min;
    return this->Meter_Spread_10_sec;
}

double Data_Segment::Get_PI_Std_Dev()
{
    double SD_sum = 0;
    double SD;

    this->PI_Avg_10_sec = this->Get_PI_Average_10sec();
    double pi_avg = this->PI_Avg_10_sec;

    // calc standard deviation
    for(int i = 0; i < this->num_data_lines; ++i)
    {
        double pi_val = this->Lines[i]->Pi_value;
        SD_sum += pow(pi_val-pi_avg, 2);
    }
    SD = sqrt(SD_sum/(double)this->num_data_lines);

    return SD;
}


double Data_Segment::Get_Meter_Std_Dev()
{
    double SD_sum = 0;
    double SD;

    this->Meter_Avg_10_sec = this->Get_Meter_Average_10sec();
    double meter_avg = this->Meter_Avg_10_sec;

    // calc standard deviation
    for(int i = 0; i < this->num_data_lines; ++i)
    {
        double meter_val = this->Lines[i]->Meter_Avg;
        SD_sum += pow(meter_val - meter_avg, 2);
    }
    SD = sqrt(SD_sum/(double)this->num_data_lines);

    return SD;
}

bool Data_Segment::Line_Is_Segment_Delimiter(char* readline)
{

    char ENTRY_DELIM[] = "----------------------------------------------------------------";
    char FILE_DELIM[] = "───────────────────";

    if( StringH::String_Contains(readline, ENTRY_DELIM) )
        return true;
    else if( StringH::String_Contains(readline, FILE_DELIM) )
        return true;  
    else return false;
}



bool Data_Segment::Line_Is_Column_Legend(char* line)
{
        if( !StringH::String_Contains_Ignore_Case(line, "timestamp"))
            return false;

        if( !( StringH::String_Contains_Ignore_Case(line, "Pi")) )
            return false;

        if( !( StringH::String_Contains_Ignore_Case(line, "BK") || StringH::String_Contains_Ignore_Case(line, "HP") || StringH::String_Contains_Ignore_Case(line, "YK") || StringH::String_Contains_Ignore_Case(line, "Yokogawa")) )
            return false;

        if( !StringH::String_Contains_Ignore_Case(line, "AVG"))
            return false;

        if( !StringH::String_Contains_Ignore_Case(line, "STD DEV"))
            return false;

        return true;
}
