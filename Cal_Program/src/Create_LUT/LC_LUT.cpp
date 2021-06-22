
#include "LC_LUT.h"
#include "Cal_Parameters.h"

// rules for LUT

// Grouping:
    // divide into groups. Each point in the group must be max 2.5% away from the average value. 
    // if point is on its own, thats ok


    // get first item
        // get next item
        // take average
        // get % variance from avg
        // if % variance > 2.5, remove the item just added
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
            // throwout item w/ largest power meter variance

    // for each group
        // Calc STD Dev, calc variance for each point
        // make threshold value of 1.8* STD DEV
        // if single point has greater variance than 1.8*STD DEV, throw out point with most 
            // then recalc the metrics and repeat process until all points within threshold

    // for each group
        // if STD DEV % of Avg Val of group is > 1%, throwout entire group


AC_LUT_HEADER::AC_LUT_HEADER(LUT_TYPE type, int num_elements, char* MAC)
{
    switch(type)
    {
        case COOLER_MA_t:
            strcpy(this->typestring, "COOLER_mA_LUT" ); // "LOAD_mA_LUT.h"
            sprintf(this->comment, "\n// \t[ pi val ][ meter val ]");
            break;
        case COOLER_V_t:
            strcpy(this->typestring, "COOLER_V_LUT" );    // "LOAD_V_LUT.h"
            break;
        case LOAD_MA_t:
            strcpy(this->typestring, "LOAD_mA_LUT" );   // "LOAD_MA_LUT.h"
            break;
        case LOAD_V_t:
            strcpy(this->typestring, "LOAD_V_LUT" );   //  "DIODE_V_LUT.h"
            break;
        case DIODE_V_t:
            strcpy(this->typestring, "DIODE_V_LUT" );    //  "COOL_V_LUT.h"
            break;
        case DAC_COUNT_t:
            strcpy(this->typestring, "DAC_COUNT_LUT" );    //  "COOL_V_LUT.h"
            break;
    }

    if( MAC != NULL)
    {
        sprintf(this->mac_addr_line, "// LUT for Pi w/ MAC Address: %s", MAC);
    }
    // strip ".h" off typestring
    int end_index = StringH::Index_First_Period(typestring);
    typestring[end_index] = '\0';   // truncate the ".h" by turning it into string termination

    sprintf(this->ifndef, "#ifndef %s_H", typestring);
    sprintf(this->define, "#define %s_H", typestring);
    sprintf(this->dimension1, "#define PI_%s\t0  \t// index in the array", typestring);
    sprintf(this->dimension2, "#define METER_%s\t1 \t// index in the array", typestring);
    sprintf(this->num_elements, "#define NUM_%s_ELEMENTS %d", typestring, num_elements);
    sprintf(this->array_declaration, "float %s[%d][2] = {", typestring, num_elements);
}

char* AC_LUT_HEADER::toString()
{
    int length = strlen(this->mac_addr_line) + strlen(ifndef) + strlen(define) + strlen(dimension1) + strlen(dimension2) + strlen(num_elements) + strlen(this->comment) + strlen(array_declaration) + 12;
    char* str = (char*)malloc(sizeof(char) * length);
    StringH::Erase_Num_Chars(str, length);
    sprintf(str, "%s\n%s\n%s\n\n%s\n%s\n%s\n\n%s\n%s\n", this->mac_addr_line, this->ifndef, this->define, this->dimension1, this->dimension2, this->num_elements, this->comment,this->array_declaration );
    return str;
}

void LC_LUT::Add_LUT_Data(LC_Data* data)
{
    if(data->mac_addr != NULL)
    {
        strncpy(this->mac_addr, data->mac_addr, 20);
    }

    Data_Segment** data_array = data->Get_Data();
    int num_points = data->Get_Num_Data_Points();

    LUT_Group* data_group = new LUT_Group();
    LUT_POINT* point = NULL;
    for( int i = 0; i< num_points; ++i)
    {

        point = new LUT_POINT( data_array[i] );

        bool result = data_group->Add_Point( point );   // find out whether new point is valid in group
        if(result)
            continue;
        else                                    // if not valid, save the current group and create a new one
        {
            this->Add_Group(data_group);        // save current group   
            data_group = new LUT_Group();       // create new group
            data_group->Add_Point( point );     // add point to new group
            continue;
        }
    }
    this->Add_Group(data_group);  
    this->Calc_LUT_Metrics();

    this->Filter_Groups();
    //this->Print_Groups();
}

void LC_LUT::Calc_LUT_Metrics()
{
    LUT_Group* cur_group;
    for(int i =0; i<this->num_groups; ++i)
    {
        cur_group = table[i];
        cur_group->Calc_Metrics();
    }
}




bool LUT_Group::Add_Point(LUT_POINT* new_point)
{
    // if current number of points == 0, just add it
    // else add it, then recalc all PI Metrics

    //printf("processing LUT point: %f\n", new_point->PI_value);

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
        
        this->Calc_Metrics();  // recalc metrics
        if( this->num_points > 2 && this->pi_span_percent_of_average > MAX_SPAN_PERCENT_AVERAGE)
        {
            //printf("\tpi val %f exceeds max span percentage of average. span percentage = %f\n", new_point->PI_value, this->pi_span_percent_of_average);
            // remove the point
            --(this->num_points);
            this->points[ this->num_points ] = NULL;
            return false;

        }
        else if(this->num_points <= 2 && this->pi_span_percent_of_average > (MAX_SPAN_PERCENT_AVERAGE+2) )
        {
            //printf("\tpi val %f exceeds max span percentage of average. span percentage = %f\n", new_point->PI_value, this->pi_span_percent_of_average);
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

double LUT_POINT::Calc_PI_Variance(double group_average)
{
    return abs(this->PI_value - group_average);
}

void LUT_Group::Calc_Metrics()
{
    this->Calc_Average();
    this->Calc_Span();
    this->Calc_Std_Dev();
    this->Calc_Span_Percent_of_Average();
    this->STD_DEV_THRESHOLD = 1.8*this->pi_std_dev;
}

double LUT_Group::Calc_Average()
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

double LUT_Group::Calc_Span()
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

double LUT_Group::Calc_Std_Dev()
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


double LUT_Group::Calc_Span_Percent_of_Average()
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

void LC_LUT::Print_Groups()
{
    printf("\n DATA GROUPS (of PI values)\n");
    LUT_Group* cur_group;
    printf("\n");
    for(int i =0; i<this->num_groups; ++i)
    {
        cur_group = this->table[i];
        cur_group->Calc_Metrics();
        //double max_variance = cur_group->STD_DEV_THRESHOLD;
        //printf("Group%d:\n", i);
        printf("\n");
        for(int j=0; j<cur_group->Get_Num_Points(); ++j)
        {

            LUT_POINT* point = cur_group->Get_Points()[j];
            //double point_variance = point->Calc_PI_Variance(cur_group->pi_average);
            printf("\t%.3f\n", cur_group->Get_Points()[j]->PI_value );
        }
    }
}

void LC_LUT::Add_Group(LUT_Group* group)
{
    if( this->num_groups < MAX_LUT_POINTS)
    {
        this->table[this->num_groups] = group;
        ++(this->num_groups);
    }
}

char* LC_LUT::Get_LUT_Type_String(LUT_TYPE type)
{
    char* return_string;
    switch(type)
    {
        case COOLER_MA_t:
            return_string = (char*)malloc( sizeof(char*) * (strlen("COOLER_MA_t")+1) );
            strcpy(return_string, "COOLER_MA_t");
            break;
        case COOLER_V_t:
            return_string = (char*)malloc( sizeof(char*) * (strlen("COOLER_V_t")+1) );
            strcpy(return_string, "COOLER_V_t");
            break;
        case LOAD_MA_t:
            return_string = (char*)malloc( sizeof(char*) * (strlen("LOAD_MA_t")+1) );
            strcpy(return_string, "LOAD_MA_t");
            break;
        case LOAD_V_t:
            return_string = (char*)malloc( sizeof(char*) * (strlen("LOAD_V_t")+1) );
            strcpy(return_string, "LOAD_V_t");
            break;
        case DIODE_V_t:
            return_string = (char*)malloc( sizeof(char*) * (strlen("DIODE_V_t")+1) );
            strcpy(return_string, "DIODE_V_t");
            break;
        case DAC_COUNT_t:
            return_string = (char*)malloc( sizeof(char*) * (strlen("DAC_COUNT_t")+1) );
            strcpy(return_string, "DAC_COUNT_t");
            break;
        case REF100_t:
            return_string = (char*)malloc( sizeof(char*) * (strlen("REF100_t")+1) );
            strcpy(return_string, "REF100_t");
            break;
        case NONE:
            return_string = (char*)malloc( sizeof(char*) * (strlen("NONE")+1) );
            strcpy(return_string, "NONE");
            break;
    }
    return return_string;
}

void LC_LUT::Filter_Groups()
{
    this->Filter_For_Variance();
    this->Filter_For_SD_Percent_of_Average();
    this->Filter_For_Num_Points();
}

void LC_LUT::Filter_For_Num_Points()
{
    for(int i =0; i<this->num_groups; ++i)
    {
        LUT_Group* group = table[i];

        if(group->Get_Num_Points() == 1)        // remove any data group that has only one point, we want to average values for improved accuracy
            this->Remove_Group_At_Index(i);
        
        while( group->Has_Point_Over_Threshold() )
        {
            int throwout_index = group->Get_Index_Largest_Variance();
            group->Throwout_Value_at_Index( throwout_index );
        }
    }
}

void LC_LUT::Filter_For_Variance()
{
    for(int i =0; i<this->num_groups; ++i)
    {
        LUT_Group* group = table[i];
        group->Calc_Metrics();

        while( group->Has_Point_Over_Threshold() )
        {
            int throwout_index = group->Get_Index_Largest_Variance();
            group->Throwout_Value_at_Index( throwout_index );
            group->Calc_Metrics();
        }
    }
}

bool LUT_Group::Has_Point_Over_Threshold()
{
    this->Calc_Metrics();

    for(int j=0; j<this->num_points; ++j)
    {
        LUT_POINT* point = this->points[j];
        double point_variance = point->Calc_PI_Variance(this->pi_average);
        if( point_variance > this->STD_DEV_THRESHOLD )
        {
            return true;
        }
    }
    return false;
}

int LUT_Group::Get_Index_Largest_Variance()
{
    this->Calc_Metrics();

    double max_variance;
    int index_max_variance = -1;
    for(int j=0; j<this->num_points; ++j)
    {
        LUT_POINT* point = this->points[j];
        double point_variance = point->Calc_PI_Variance(this->pi_average);
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

void LUT_Group::Throwout_Value_at_Index(int delete_index)
{
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

void LC_LUT::Remove_Group_At_Index(int delete_index)
{
    LUT_Group* del_group = this->table[delete_index];

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

void LC_LUT::Filter_For_SD_Percent_of_Average()
{
    // if %SD of avg exceeds 1, throw out group
    for(int i =0; i<this->num_groups; ++i)
    {
        LUT_Group* group = table[i];
        group->Calc_Metrics();

        double std_dev = group->pi_std_dev;
        double avg = group->pi_std_dev_percent_of_average;
        double ratio = std_dev/avg;
        double percent = ratio*100;

        if(percent >= 1)
            this->Remove_Group_At_Index(i);
    }
}

void LC_LUT::Write_to_File(const char* filepath)
{
    FILE* write_file = FileH::Create_New_File(filepath);
    char writeline[300];

    LUT_TYPE type = this->Get_LUT_Type(filepath);

    printf("\n\n");
    this->Write_LUT_Header(write_file);
    for(int i =0; i<this->num_groups; ++i)
    {
        LUT_Group* group = table[i];
        group->Calc_Metrics();
        StringH::Erase_Num_Chars(writeline, 300);

        double pi_val = group->pi_average;
        double meter_val = group->meter_average;

        sprintf(writeline, "\t{%.3f, %.3f},\n", pi_val, meter_val);
        fprintf(write_file, "%s", writeline);
        //printf("%s", writeline);
    }
    fprintf(write_file, "};\n");
    fprintf(write_file, "#endif\n");
    FileH::Close_File(write_file);
}

LUT_TYPE LC_LUT::Get_LUT_Type(const char* filepath)
{

    LUT_TYPE type;
    char* filename = FileH::Extract_Filename(filepath);

    if( StringH::String_Contains_Ignore_Case(filename, "COOLER") )
    {
        if( StringH::String_Contains_Ignore_Case(filename, "mA") )
            if( StringH::String_Contains_Ignore_Case(filename, "DC") )
                type = COOLER_MA_DC_t;
            else
                type = COOLER_MA_t;
        else
            type = COOLER_V_t;

    }
    else if( StringH::String_Contains_Ignore_Case(filename, "LOAD") )
    {
        if( StringH::String_Contains_Ignore_Case(filename, "mA") )
            type = LOAD_MA_t;
        else
            type = LOAD_V_t;
        
    }
    else if( StringH::String_Contains_Ignore_Case(filename, "DIODE") )
    {
        type = DIODE_V_t;
    }
    else if( StringH::String_Contains_Ignore_Case(filename, "REF100") )
    {
        type = REF100_t;
    }
    else if( StringH::String_Contains_Ignore_Case(filename, "DAC"))
    {
        type = DAC_COUNT_t;
    }
    else if( StringH::String_Contains_Ignore_Case(filename, "THERMOCOUPLE"))
    {
        type = THERMOCOUPLE_t;
    }
    else
    {
        type = NONE;
        printf("LUT type not found for '%s'\n", filename);
    }
    free(filename);
    return type;
}

LUT_TYPE LC_LUT::Get_LUT_Type_From_Content(char* filepath)
{
    FILE* LUT_file = FileH::Open_Existing_File(filepath);

    char* file_title = FileH::Get_File_Title(LUT_file);
    //printf("file title: %s\n", file_title);
    fclose(LUT_file);

    char title[strlen(file_title)+1];
    strcpy(title, file_title);
    free(file_title);

    // if contains "power meter" or "cooler" or "cool" or "CCC"
    if( StringH::String_Contains_Ignore_Case(title, "Power Meter") || StringH::String_Contains_Ignore_Case(title, "Cool") || StringH::String_Contains_Ignore_Case(title, "CCC"))
    {
        if( StringH::String_Contains_Ignore_Case(title, "mA") )
            return COOLER_MA_t;

        else if( StringH::String_Contains_Ignore_Case(title, "V"))
            return COOLER_V_t;
    }
    else if( StringH::String_Contains_Ignore_Case(title, "DIODE") )
    {
        return DIODE_V_t;
    }
    else if( StringH::String_Contains_Ignore_Case(title, "REF100") )
    {
        return REF100_t;
    }
    else if( StringH::String_Contains_Ignore_Case(title, "LOAD") )
    {
        if( StringH::String_Contains_Ignore_Case(title, "mA") )
            return LOAD_MA_t;
        if( StringH::String_Contains_Ignore_Case(title, "V") )
            return LOAD_V_t;
    }
    else if( StringH::String_Contains_Ignore_Case(title, "THERMOCOUPLE") )
    {
            return THERMOCOUPLE_t;
    }
    return NONE;
}

void LC_LUT::Write_LUT_Header(FILE* writefile)
{
    AC_LUT_HEADER* header = new AC_LUT_HEADER(COOLER_MA_t, this->num_groups, this->mac_addr);

    char* header_str = header->toString();

    fprintf(writefile, "%s", header_str);
    free(header_str);


    delete header;
}