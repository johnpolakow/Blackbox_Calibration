
#include "DC_Segment.h"


void DC_Segment::Add_Line(DC_Line* add_line)
{
    if(num_data_lines < DATA_LINES_PER_SEGMENT )
    {
        this->Lines[ this->num_data_lines ] = add_line;
        ++(this->num_data_lines);
    }
    else
        return;
}

DC_Segment* DC_Segment::Scan_DC_Segment(FILE* read_file, LUT_TYPE type)
{
    char readline[300];
    char TIMESTAMP[60];

    bool read_date = false;
    bool read_legend = false;
    bool reached_delimiter = false;



    DC_Segment* segment = new DC_Segment(type);
    while( !reached_delimiter )
    {
        StringH::Erase_Num_Chars(readline, 300);

        if( fgets(readline, 256, read_file) == NULL )
        {
            printf("\n");
            delete segment;
            segment = NULL;
            return segment;
        }
        
        StringH::Trim_WhiteSpace(readline);
        //printf("fgets: %s", readline);

        if( StringH::Line_Is_Whitespace(readline) ) // if whitespace, continue
        {
            //printf("\t\t[whitespace]\n");
            continue;
        }

        if( !read_date && StringH::Line_Is_TimeStamp(readline) )
        {   // Determine what the line is:
            //printf("\t\t[date]\n");
            read_date = true;
            continue;
        }

        if( !read_legend && DC_Segment::Line_Is_Column_Legend(readline) )
        {
            //printf("\t\t[legend]\n");
            read_legend = true;
            continue;
        }

        if( DC_Line::Is_Data_Line(readline) ) // filters for less than 3 data points
        {
            //printf("\t\t[data_line]\n");
            if( type == DAC_COUNT_t || type == LOAD_MA_t)
            {
                DC_Line* new_data = new DC_Line();
                new_data->Parse_DAC_Line(readline);
                segment->Add_Line(new_data);
            }
            else
            {
                DC_Line* new_data = new DC_Line(readline);
                segment->Add_Line(new_data);
            }
            continue;
        }

        if( DC_Line::Is_Data_Average(readline) ) // filters for less than 3 data points
        {
            //printf("\t\t[data_average]\n");
            continue;
        }

        if( DC_Line::Is_Std_Dev(readline) ) // filters for less than 3 data points
        {
            //printf("\t\t[standard deviation]\n");
            continue;
        }


        if( DC_Segment::Line_Is_Segment_Delimiter(readline) )
        {
            // marking the stop time of the segment
            //printf("\t\t[delimiter]\n");
            int last_line_index = segment->num_data_lines -1 ;
            DC_Line* last_line_added = segment->Lines[last_line_index];
            reached_delimiter = true;
            break;
        }
    }
    if(type == DAC_COUNT_t)
    {
        segment->Read_DAC_Count();
        segment->Calc_Metrics();
    }
    else
    {
        segment->Calc_Metrics();
    }

    return segment;
}


void DC_Segment::Calc_Metrics()
{
    // calculate metrics
    if( this->LUT_type == DAC_COUNT_t )
    {
        this->Read_DAC_Count();
    }
    this->Calc_PI_Average();
    this->Calc_Meter_Average();
    this->Calc_PI_Std_Dev();
    this->Calc_Meter_Std_Dev();
}

void DC_Segment::Read_DAC_Count()
{
    int fixed_count;

    if(this->num_data_lines > 0)
    {
        DC_Line* first_line = this->Lines[0];
        fixed_count = first_line->DAC_count;

        for( int i=0; i<this->num_data_lines; ++i)
        {
            if(this->Lines[i]->DAC_count != fixed_count)
            {
                printf("throw out count value due to count mismatch\n");
                printf("count at line %d is %d, fixed count: %d\n", i, this->Lines[i]->DAC_count, fixed_count );
                fixed_count = -1;                       // error flag, count should be the same for all lines in this data segment
                this->ignore_segment = true;
            }

        }
        this->DAC_count = fixed_count;
    }
    else
        return;
}

void DC_Segment::Calc_PI_Average()
{
    double pi_sum = 0;
    for( int i = 0; i < this->num_data_lines; ++i)
    {
        pi_sum += this->Lines[i]->Pi_value;
    }
    this->PI_Avg = pi_sum/this->num_data_lines;
}

void DC_Segment::Calc_Meter_Average()
{
    double meter_sum = 0;
    for( int i = 0; i < this->num_data_lines; ++i)
    {
        meter_sum += this->Lines[i]->Meter_value;
    }
    this->Meter_Avg = meter_sum/(double)this->num_data_lines;
}

void DC_Segment::Calc_PI_Std_Dev()
{
    double SD_sum = 0;
    double SD;

    double pi_avg = this->PI_Avg;

    // calc standard deviation
    for(int i = 0; i < this->num_data_lines; ++i)
    {
        double pi_val = this->Lines[i]->Pi_value;
        SD_sum += pow(pi_val-pi_avg, 2);
    }
    SD = sqrt(SD_sum/(double)this->num_data_lines);
    this->PI_Std_Dev = SD;
}


void DC_Segment::Calc_Meter_Std_Dev()
{
    double SD_sum = 0;
    double SD;

    double meter_avg = this->Meter_Avg;

    // calc standard deviation
    for(int i = 0; i < this->num_data_lines; ++i)
    {
        double meter_val = this->Lines[i]->Meter_value;
        SD_sum += pow(meter_val-meter_avg, 2);
    }
    SD = sqrt(SD_sum/(double)this->num_data_lines);
    this->Meter_Std_Dev = SD;
}

bool DC_Segment::Line_Is_Segment_Delimiter(char* readline)
{

    char ENTRY_DELIM[] = "----------------------------------------------------------------";
    char FILE_DELIM[] = "───────────────────";

    if( StringH::String_Contains(readline, ENTRY_DELIM) )
        return true;
    else if( StringH::String_Contains(readline, FILE_DELIM) )
        return true;  
    else return false;
}


/*  Possible Legend Lines:
	[ Pi meas mA, BK5335 meas mA] cooler ma DC
    [ Pi meas V, HP meas V]         cooler V
    [ Pi meas. V, HP meas. V]       Load V
    [ DAC count, Pi mA, HP mA]      Load mA
*/
bool DC_Segment::Line_Is_Column_Legend(char* line)
{
    bool contains_pi = StringH::String_Contains_Ignore_Case(line, "Pi");
    bool contains_meas = StringH::String_Contains_Ignore_Case(line, "meas");
    bool contains_HP = StringH::String_Contains_Ignore_Case(line, "HP");
    bool contains_BK = StringH::String_Contains_Ignore_Case(line, "BK5335");
    bool contains_DAC = StringH::String_Contains_Ignore_Case(line, "DAC");

    if(contains_pi && contains_meas && contains_BK)
        return true;
    else if( contains_pi && contains_meas && contains_HP)
        return true;
    else if( contains_DAC && contains_HP && contains_pi)
        return true;
    else 
        return false;
}


