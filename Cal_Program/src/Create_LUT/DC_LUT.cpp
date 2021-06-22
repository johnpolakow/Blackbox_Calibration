
#include "DC_LUT.h"

// rules for LUT

// Grouping:
    // divide into groups. Each point in the group must be max 1% away from the average value of the group. 
    // if point is by itself, thats ok


    // get first item
        // get next item
        // take average
        // get % variance from avg
        // if % variance > 1, remove the item just added
            // current items then go into a group
    // continue until all items are grouped

    // for each group
        // calc the span, avg, & the span percentage of the average
        // if the span is greater than 2.5% of the average, iteratively, one val at a time:
            // throw out val with highest variance
            // recalc avg & span & %span
            // repeat until span is less than 2.5% of avg val


    // after all items have been grouped:
        // for each group
            // throwout item w/ largest pi variance
            // throwout item w/ largest BK variance

    // for each group
        // Calc STD Dev, calc variance for each point
        // make threshold value of 1.8* STD DEV
        // if single point has greater variance than 1.8*STD DEV, throw out point with most 
            // then recalc the metrics and repeat process until all points within threshold

    // for each group
        // if STD DEV % of Avg Val of group is > 1%, throwout entire group


// creates the LUT Header to be written to .h file
DC_LUT_HEADER::DC_LUT_HEADER(LUT_TYPE type, int num_elements, char* MAC)
{
    StringH::Erase_Num_Chars(this->typestring, 50);
    switch(type)
    {
        case COOLER_MA_t:
            strcpy(this->typestring, "COOLER_mA_LUT" ); // "COOLER_mA_LUT.h"
            sprintf(this->comment, "\n// [ pi meas. mA ][ meter mA ]\n");
            break;
        case COOLER_V_t:
            strcpy(this->typestring, "COOLER_V_LUT" );    // "COOLER_V_LUT.h"
            sprintf(this->comment, "\n// [ pi meas. V ][ meter V ]\n");
            break;
        case LOAD_MA_t:
            strcpy(this->typestring, "LOAD_mA_LUT" );   // "LOAD_MA_LUT.h"
            sprintf(this->comment, "\n// [ pi meas. mA ][ meter mA ]\n");
            break;
        case LOAD_V_t:
            strcpy(this->typestring, "LOAD_V_LUT" );   //  "LOAD_V_LUT.h"
            sprintf(this->comment, "\n// [ pi meas. V ][ meter V ]\n");
            break;
        case DIODE_V_t:
            strcpy(this->typestring, "DIODE_V_LUT" );    //  "DIODE_V_LUT.h"
            sprintf(this->comment, "\n// [ pi diode V ][ meter diode V ]\n");
            break;
        case DAC_COUNT_t:
            strcpy(this->typestring, "DAC_COUNT_LUT" );    //  "DAC_COUNT_LUT.h"
            sprintf(this->comment, "// [ dac count ][ meter mA ]\n");
            break;
    }

    if( MAC != NULL)
    {
        //printf(" DC LUT Header, MAC: %s\n", MAC);
        sprintf(this->mac_addr_line, "// LUT for Pi w/ MAC Address: %s", MAC);
    }
    sprintf(this->ifndef, "#ifndef %s_H", typestring);
    sprintf(this->define, "#define %s_H", typestring);
    sprintf(this->dimension1, "#define PI_%s   \t0  \t// index in the array", typestring);
    sprintf(this->dimension2, "#define METER_%s\t1 \t// index in the array", typestring);
    sprintf(this->num_elements, "#define NUM_%s_ELEMENTS %d", typestring, num_elements);
    sprintf(this->array_declaration, "float %s[%d][2] = {", typestring, num_elements);
}

char* DC_LUT_HEADER::toString()
{
    int length = strlen(mac_addr_line) + strlen(ifndef) + strlen(define) + strlen(dimension1) + strlen(dimension2) + strlen(num_elements) + strlen(array_declaration) + strlen(comment) + 12;
    char* str = (char*)malloc(sizeof(char) * length);
    StringH::Erase_Num_Chars(str, length);
    sprintf(str, "%s\n%s\n%s\n\n%s\n%s\n%s\n\n%s%s\n", mac_addr_line, ifndef, define, dimension1, dimension2, num_elements, comment, array_declaration );
    //printf("%s\n", str);
    return str;
}

void DC_LUT::Add_LUT_Data(DC_Data* data)
{
    //printf("add LUT data, MAC %s\n", data->mac_addr);
    if(data->mac_addr != NULL)
    {
        strncpy(this->mac_addr, data->mac_addr, 20);
    }

    DC_Segment** data_array = data->Get_Data();
    int num_points = data->Get_Num_Data_Points();

    DC_Group* data_group = new DC_Group();
    data_group->type = data->LUT_type;
    DC_LUT_POINT* point = NULL;
    for( int i = 0; i< num_points; ++i)
    {
        point = new DC_LUT_POINT( data_array[i] );
        bool result = data_group->Add_Point( point, data->LUT_type );   // find out whether new point is valid in group
        if(result)
            continue;
        else                                    // if not valid, save the current group and create a new one
        {
            this->Add_Group(data_group);        // save current group   
            data_group = new DC_Group();       // create new group
            data_group->Add_Point( point, data->LUT_type );     // add point to new group
            data_group->type = data->LUT_type;
            continue;
        }
    }
    this->Add_Group(data_group);  
    this->Filter_Groups();
    // this->Print_Groups();
}

void DC_LUT::Add_DAC_LUT_Data(DC_Data* data, LUT_TYPE type)
{
    if(data->mac_addr != NULL)
    {
        strncpy(this->mac_addr, data->mac_addr, 20);
    }

    DC_Segment** data_array = data->Get_Data();
    int num_points = data->Get_Num_Data_Points();

    DC_Group* data_group = new DC_Group();
    data_group->type = type;
    DC_LUT_POINT* point = NULL;
    for( int i = 0; i< num_points; ++i)
    {
        point = new DC_LUT_POINT( data_array[i], type );
        bool result;
        if( type == DAC_COUNT_t)
            result = data_group->Add_DAC_Point( point );   // find out whether new point is valid in group
        else
        {
            result = data_group->Add_Point( point, type);   // find out whether new point is valid in group
        }
        
        if(result)
            continue;
        else                                    // if not valid, save the current group and create a new one
        {
            this->Add_Group(data_group);         // save current group   
            data_group = new DC_Group();         // create new group
            data_group->Add_Point( point, type );     // add point to new group
            data_group->type = this->LUT_type;  
            continue;
        }
    }
    this->Add_Group(data_group);  
    this->Filter_Groups();
    //this->Print_Groups();
}

void DC_LUT::Print_Groups()
{
    DC_Group* cur_group;
    printf("\n");
    
    for(int i =0; i<this->num_groups; ++i)
    {
        cur_group = table[i];

        if( cur_group->type == DAC_COUNT_t)
        {
            printf("Group%d: Count  Meter mA\n", i);
            for(int j=0; j<cur_group->Get_Num_Points(); ++j)
            {

                DC_LUT_POINT* point = cur_group->Get_Points()[j];
                int count = point->DAC_count;
                double meter_val = point->Meter_value;
                printf("\t%d\t%.3f\n", count, meter_val );
            }
        }
        else if(cur_group->type == DIODE_V_t)
        {
            cur_group->Calc_Metrics();
            double max_variance = cur_group->STD_DEV_THRESHOLD;
            printf("Group%d:   PI             Meter \n", i);
            for(int j=0; j<cur_group->Get_Num_Points(); ++j)
            {

                DC_LUT_POINT* point = cur_group->Get_Points()[j];
                double meter_val = point->Meter_value;
                double point_variance = cur_group->Calc_PI_Variance(j);
                printf("\t%.5f\t\t%.5f\n", point->PI_value, meter_val );
            }
            printf("\n");
        }
        else
        {
            cur_group->Calc_Metrics();
            double max_variance = cur_group->STD_DEV_THRESHOLD;
            printf("Group%d:  PI     Meter \n", i);
            for(int j=0; j<cur_group->Get_Num_Points(); ++j)
            {

                DC_LUT_POINT* point = cur_group->Get_Points()[j];
                double meter_val = point->Meter_value;
                double point_variance = cur_group->Calc_PI_Variance(j);
                printf("\t%.3f\t%.3f\n", point->PI_value, meter_val );
            }
            printf("\n");
        }
    }
}

void DC_LUT::Add_Group(DC_Group* group)
{
    if( this->num_groups < MAX_DC_LUT_POINTS)
    {
        if(group->type == DAC_COUNT_t)
        {
            group->Calc_Counts();
        }
        this->table[this->num_groups] = group;
        ++(this->num_groups);
    }
}


void DC_LUT::Filter_Groups()
{
    this->Filter_For_Variance();
    this->Filter_For_SD_Percent_of_Average();
}

void DC_LUT::Filter_For_Variance()
{
    bool printed_heading = false;
    for(int i =0; i<this->num_groups; ++i)
    {
        DC_Group* group = table[i];
        group->Calc_Metrics();

        if(group->Get_Num_Points() > 2)
        {
            while( group->Has_Point_Over_Threshold() )
            {
                if( !printed_heading )
                {
                    printf("\nRemoving Points from group with Largest Variance:\n");
                    printed_heading = true;

                }
                int throwout_index = group->Get_Index_Largest_Variance();
                group->Throwout_Value_at_Index( throwout_index, "most var" );
                group->Calc_Metrics();
            }
        }
    }
}

void DC_LUT::Filter_For_SD_Percent_of_Average()
{
    // if %SD of avg exceeds 1, throw out group
    for(int i =0; i<this->num_groups; ++i)
    {
        DC_Group* group = table[i];
        group->Calc_Metrics();

        double std_dev = group->pi_std_dev;
        double avg = group->pi_average;
        double ratio = std_dev/avg;
        double percent = ratio*100;

        if(percent >= 1 && group->Get_Num_Points() > 1)
        {
            printf("--removing group at index %d\n", i);
            group->Print_Group();
            //printf("\tSD: %.3f\n", std_dev);
            //printf("\tAVG: %.3f\n", avg);
            //printf("\tRATIO: %.3f\n", ratio);
            //printf("\t%%AVG %.2f\n", percent);

            this->Remove_Group_At_Index(i);
            printf("\n\n");
        }
    }
}

bool DC_Group::Has_Point_Over_Threshold()
{
    this->Calc_Metrics();

    for(int j=0; j<this->num_points; ++j)
    {
        DC_LUT_POINT* point = this->points[j];
        double point_variance = this->Calc_PI_Variance(j);
        if( point_variance > this->STD_DEV_THRESHOLD )
        {
            return true;
        }
    }
    return false;
}


int DC_Group::Get_Index_Largest_Variance()
{
    this->Calc_Metrics();

    double max_variance;
    int index_max_variance = -1;
    for(int j=0; j<this->num_points; ++j)
    {
        DC_LUT_POINT* point = this->points[j];
        double point_variance = this->Calc_PI_Variance(j);
        if( point_variance > this->STD_DEV_THRESHOLD )
        {
            if(index_max_variance == -1)
            {
                index_max_variance = j;
                max_variance = point_variance;
            }
        }
    }
    return index_max_variance;
}

void DC_Group::Throwout_Value_at_Index(int delete_index, const char* msg)
{
    //printf("\t[%s] rm %.4f @ index %d: ", msg, this->points[delete_index]->PI_value, delete_index );
    printf("\trm %.4f @ index %d: ", this->points[delete_index]->PI_value, delete_index );
    this->Print_Group_Highlight_Index(delete_index);
    //this->Print_Group_Highlight_Index(delete_index)
    delete this->points[delete_index];
    int from_index;
    int dest_index = delete_index;
    for(dest_index = delete_index; dest_index<(this->num_points-1); ++dest_index )
    {
        from_index = dest_index+1;
        this->points[dest_index] = this->points[from_index];    // shift all points to the left one
    }
    --this->num_points;
    this->points[this->num_points] = NULL;
}

void DC_Group::Print_Group()
{
    if( this->type == DAC_COUNT_t)
    {
        printf(" [");
        for(int i =0; i<(this->num_points); ++i )
        {
            int count = this->points[i]->DAC_count;
            double meter_val = this->points[i]->Meter_value;
            printf(" %d, %.3f ", count, meter_val);
        }
        printf("]\n"); 
    }
    printf(" [");
    for(int i =0; i<(this->num_points); ++i )
    {
        double pi_val = this->points[i]->PI_value;
        double meter_val = this->points[i]->Meter_value;
        printf(" %.5f:%.5f, ", pi_val, meter_val);
    }
    printf("]\n");
}

void DC_Group::Print_Group_Highlight_Index(int hi_index)
{
    if( this->type == DAC_COUNT_t)
    {
        int count;
        if(this->num_points >0)
        {
            count = this->points[0]->DAC_count;
        }
        printf("count = %d [", count);
        for(int i =0; i<(this->num_points); ++i )
        {
            if(i>=10)
            {
                // move cursor back one space
                // print ...
                printf("\033[<1>D" " ...");
                break;
            }

            double meter_val = this->points[i]->Meter_value;
            if( i == hi_index)
            {
                //printf( COLOR_RED " %d, %.3f" COLOR_RESET, count, meter_val);
                printf( COLOR_RED " %.3f" COLOR_RESET ",", meter_val);
            }
            else
            {
                //printf(" %d, %.3f", count, meter_val);
                printf(" %.3f,", meter_val);
            }
        }
        printf("\033[<3>D" " ");
        printf("]\n"); 
    }
    else{
        printf(" [");
        for(int i =0; i<(this->num_points); ++i )
        {
            if(i>=8)
            {
                // move cursor back one space
                printf("\033[<1>D" " ...");
                break;

            }
            double pi_val = this->points[i]->PI_value;
            double meter_val = this->points[i]->Meter_value;

            if( i == hi_index)
            {
                printf( COLOR_RED " %.4f:%.4f" COLOR_RESET ", ", pi_val, meter_val);
            }
            else
            {
                printf(" %.4f:%.4f, ", pi_val, meter_val);
            }
        }
        printf("\033[<2>D" "  ");
        printf("]\n");
    }
}

void DC_Group::Calc_Counts()
{
    this->Calc_DAC_Count_Average();
    this->Calc_DAC_Count_Std_Dev();
}

double DC_Group::Calc_DAC_Count_Average()
{
    double count_sum = 0;
    for(int i=0; i<this->num_points; ++i)
    {
        int count_val = points[i]->DAC_count;
        count_sum += count_val;
    }
    this->count_average = count_sum/this->num_points;
    this->dac_count = this->count_average;
    return this->count_average;
}

double DC_Group::Calc_DAC_Count_Std_Dev()
{
    if(num_points >1)   // can only calculate a std dev with at least 2 points
    {
        double count_average = this->Calc_DAC_Count_Average();
        double SD_sum = 0;
        double SD;

        // calc standard deviation
        for(int i = 0; i < this->num_points; ++i)
        {
            int count_val = this->points[i]->DAC_count;
            SD_sum += pow(count_val-count_average, 2);    // sum the variance squared for each data point
        }
        SD = sqrt(SD_sum/(double)this->num_points);     // take sqrt of variance squared sum to get average deviation

        this->count_std_dev = SD;
        return SD;
    }
    else 
        return 0;
}

void DC_LUT::Remove_Group_At_Index(int delete_index)
{
    printf("\tremoving group at index %d\n", delete_index);
    DC_Group* del_group = this->table[delete_index];

    int from_index;
    int dest_index = delete_index;
    for(dest_index = delete_index; dest_index<(this->num_groups-1); ++dest_index )
    {
        from_index = dest_index+1;
        this->table[dest_index] = this->table[from_index];    // shift all points to the left one
    }
    --this->num_groups;
    this->table[this->num_groups] = NULL;
    //delete del_group;
}



void DC_LUT::Write_to_File(const char* filepath)
{
    FILE* write_file = FileH::Create_New_File(filepath);
    char writeline[300];

    LUT_TYPE type = LC_LUT::Get_LUT_Type(filepath);

    printf("\n\n");
    this->Write_LUT_Header(type, write_file);
    for(int i =0; i<this->num_groups; ++i)
    {
        DC_Group* group = table[i];
        group->Calc_Metrics();
        StringH::Erase_Num_Chars(writeline, 300);

        double pi_val = group->pi_average;
        double meter_val = group->meter_average;
        
        if( type == DIODE_V_t)
        {
            sprintf(writeline, "\t{%.4f, %.4f},\n", pi_val, meter_val);
        }
        else
        {
            sprintf(writeline, "\t{%.3f, %.3f},\n", pi_val, meter_val);
        }
        
        fprintf(write_file, "%s", writeline);
        //printf("%s", writeline);
    }
    fprintf(write_file, "};\n");
    fprintf(write_file, "#endif\n");
    FileH::Close_File(write_file);
}

void DC_LUT::Write_DAC_LUT_to_File(const char* filepath)
{
    FILE* write_file = FileH::Create_New_File(filepath);
    char writeline[300];

    LUT_TYPE type = LC_LUT::Get_LUT_Type(filepath);
    printf("\n\n");
    this->Write_LUT_Header(type, write_file);
    for(int i =0; i<this->num_groups; ++i)
    {
        DC_Group* group = table[i];
        group->Calc_Metrics();
        StringH::Erase_Num_Chars(writeline, 300);

        int DAC_count = group->dac_count;
        double meter_val = group->meter_average;

        sprintf(writeline, "\t{%d, %.3f},\n", DAC_count, meter_val);
        fprintf(write_file, "%s", writeline);
        //printf("%s", writeline);
    }
    fprintf(write_file, "};\n");
    fprintf(write_file, "#endif\n");
    FileH::Close_File(write_file);
}


void DC_LUT::Write_LUT_Header(LUT_TYPE type, FILE* writefile)
{
    DC_LUT_HEADER* header = new DC_LUT_HEADER(type, this->num_groups, this->mac_addr);

    char* header_str = header->toString();
    fprintf(writefile, "%s", header_str);
    free(header_str);

    delete header;
}


bool DC_Group::Add_Point(DC_LUT_POINT* new_point, LUT_TYPE type)
{
    // if current number of points == 0, just add it
    // else add it, then recalc all PI Metrics
    if(this->num_points == 0)
    {
        this->points[ this->num_points ] = new_point;
        ++(this->num_points);
        return true;
    }
    else
    {
        this->points[ this->num_points ] = new_point;   // add the point
        ++(this->num_points);
        
        this->Calc_Metrics();  // recalculate metrics to see if point is acceptable in the group

        // whether the added point is acceptable is dependent on lookup table type
        if(type == LOAD_V_t)
        {
            if( this->pi_span_percent_of_average > MAX_LOAD_V_SPAN_PERCENT_AVERAGE)
            {
                this->Remove_Last_Point();
                return false;
            }
            else
            {
                return true;
            }
            
        }
        else if( type == COOLER_V_t)
        {
            if( this->pi_span_percent_of_average > MAX_COOLER_V_SPAN_PERCENT_AVERAGE)
            {
                this->Remove_Last_Point();
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            if( this->pi_span_percent_of_average > MAX_SPAN_PERCENT_AVERAGE)
            {
                this->Remove_Last_Point();
                return false;
            }
            else
            {
                return true;
            }
        }
    }
}

void DC_Group::Remove_Last_Point()
{
    // remove the point
    --(this->num_points);
    this->points[ this->num_points ] = NULL;
}

bool DC_Group::Add_DAC_Point(DC_LUT_POINT* new_point)
{
    // if current number of points == 0, just add it
    // else add it, then recalc all PI Metrics
    if(this->num_points == 0)
    {
        this->points[ this->num_points ] = new_point;
        ++(this->num_points);
        return true;
    }
    else
    {
        
        this->points[ this->num_points ] = new_point;   // add the point
        ++(this->num_points);
        
        this->Calc_Counts();  // recalc metrics
        if( this->count_std_dev > MAX_COUNT_STD_DEV)
        {
            // remove the point
            --(this->num_points);
            this->points[ this->num_points ] = NULL;
            return false;
        }
        else
        {
            return true;
        }
    }
}



double DC_Group::Calc_PI_Variance(int index)
{
    this->Calc_Metrics();
    double pi_val = points[index]->PI_value;
    double pi_avg = this->pi_average;
    return abs(pi_val - pi_avg);
}


void DC_Group::Calc_Metrics()
{
    this->Calc_Average();
    this->Calc_Span();
    this->Calc_Std_Dev();
    this->Calc_Span_Percent_of_Average();
    this->STD_DEV_THRESHOLD = 1.6*this->pi_std_dev;
}

double DC_Group::Calc_Average()
{

    double pi_sum = 0;
    double meter_sum = 0;
    for(int i=0; i<this->num_points; ++i)
    {
        double pi_val = points[i]->PI_value;
        double meter_val = points[i]->Meter_value;
        pi_sum += pi_val;
        meter_sum += meter_val;
    }
    this->pi_average = pi_sum/this->num_points;
    this->meter_average = meter_sum/this->num_points;
    return this->pi_average;
}

double DC_Group::Calc_Span()
{
    double pi_min, pi_max;
    if(num_points >0)
    {
        pi_min = points[0]->PI_value;
        pi_max = points[0]->PI_value;
    }
    else
        return 0;

    for(int i=0; i<this->num_points; ++i)
    {
        double pi_val = points[i]->PI_value;
        
        if(pi_val < pi_min)
            pi_min = pi_val;
        else if(pi_val > pi_max)
            pi_max = pi_val;
    }
    this->pi_span = pi_max - pi_min;
    return this->pi_span;
}

double DC_Group::Calc_Std_Dev()
{
    if(num_points >1)   // can only calculate a std dev with at least 2 points
    {

        double pi_average = this->Calc_Average();


        double SD_sum = 0;
        double SD;

        // calc standard deviation
        for(int i = 0; i < this->num_points; ++i)
        {
            double pi_val = this->points[i]->PI_value;
            SD_sum += pow(pi_val-pi_average, 2);    // sum the variance squared for each data point
        }
        SD = sqrt(SD_sum/(double)this->num_points);     // take sqrt of variance squared sum to get average deviation

        this->pi_std_dev = SD;
        return SD;
    }
    else 
        return 0;
}


double DC_Group::Calc_Span_Percent_of_Average()
{
    if(num_points >1)   // can only calculate a span with at least 2 points
    {
        double span = this->Calc_Span();
        double average = this->Calc_Average();

        double ratio = span / average;
        double percent = 100*ratio;

        this->pi_span_percent_of_average = percent;
        return this->pi_span_percent_of_average;
    }
    else 
        return -1;
}