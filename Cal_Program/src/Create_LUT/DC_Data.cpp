

#include "DC_Data.h"



    // it is assumed the file pointer is directly after the file header
    // continue to scan data until EOF
    // each data segment is in 10 second intervals, and takes the format: (one data segment shown below)
/*
15:27:07  11-25-20
	[ Pi meas. V, HP meas. V]
	[ 10.3174, 10.3231 ]
	[ 10.3172, 10.3231 ]
	[ 10.3172, 10.3231 ]
	[ 10.3173, 10.3233 ]
	[ 10.3173, 10.3232 ]
	[ 10.3173, 10.3232 ]
	[ 10.3172, 10.3233 ]
	[ 10.3172, 10.3231 ]
	[ 10.3174, 10.3231 ]
	[ 10.3173, 10.3233 ]
	AVG[ 10.3173, 10.3232]
	STD DEV[ 0.0001, 0.0001]
    ---------------------------------------------------------------------------
*/
void DC_Data::Scan_Data(char* filepath)
{
    printf("Scanning Data ....  \n");
    FILE* orig_file = FileH::Open_Existing_File(filepath);
    if( orig_file == NULL)
    {
        printf("Error: file '%s' was unable to be opened\n", filepath);
        return;
    }

    FileH::Read_File_Header(orig_file, this->mac_addr);
    //FileH::Read_File_Header(orig_file);
    //printf("DC mac addr: %s\n", this->mac_addr);
    
    bool End_of_File = false;
    int count_added = 0;

    this->LUT_type   = LC_LUT::Get_LUT_Type(filepath);
    char* typestring = LC_LUT::Get_LUT_Type_String(this->LUT_type);
    //printf("Scan Data (this->LUT_type): %s\n", typestring);
    free(typestring);

    while( !End_of_File )
    {
        DC_Segment* data_seg = DC_Segment::Scan_DC_Segment(orig_file, this->LUT_type);
        if(data_seg == NULL)
        {
            //printf("Data Seg is NULL\n");
            End_of_File = true;
            break;
        }
        else
        {
            this->Add_New_Data_Segment(data_seg);
            ++count_added;
        }
    }
    printf("\t%d segments scanned\n", count_added);
    FileH::Close_File(orig_file);
    this->Print_Data();
}

void DC_Data::Scan_DAC_Data(char* filepath, LUT_TYPE type)
{
    printf("Scanning Data ....  \n");
    FILE* orig_file = FileH::Open_Existing_File(filepath);
    FileH::Read_File_Header(orig_file, this->mac_addr);
    //FileH::Read_File_Header(orig_file);
    bool End_of_File = false;
    int count_added = 0;

    this->LUT_type = type;

    while( !End_of_File )
    {
        DC_Segment* data_seg = DC_Segment::Scan_DC_Segment(orig_file, this->LUT_type);
        if(data_seg == NULL)
        {
            //printf("Data Seg is NULL\n");
            End_of_File = true;
            break;
        }
        else
        {
            this->Add_New_Data_Segment(data_seg);
            ++count_added;
        }
    }
    printf("\t%d segments scanned\n", count_added);
    FileH::Close_File(orig_file);
    this->Print_Data();
}

void DC_Data::Print_Data()
{
    char typestring[20];
    int digit_precision = 3;
    switch(this->LUT_type)
    {
        case COOLER_MA_t:
            strcpy(typestring, "COOLER_mA" ); // "LOAD_mA_LUT.h"
            printf(" DATA:  [ " COLOR_YELLOW "%s" COLOR_RESET " ]\n", typestring );
            printf("\tPi mA\t Meter mA\n");
            for(int i = 0; i<this->num_segments; ++i)
            {
                double pi_val = data_array[i]->PI_Avg;
                double meter_val = data_array[i]->Meter_Avg;
                printf("\t%.3f\t%.3f\n", pi_val, meter_val);
            }
            break;
        case COOLER_V_t:
            strcpy(typestring, "COOLER_V" );    // "LOAD_V_LUT.h"
            printf(" DATA:  [ " COLOR_YELLOW "%s" COLOR_RESET " ]\n", typestring );
            printf("\tPi V\t  Meter V\n");
            for(int i = 0; i<this->num_segments; ++i)
            {
                double pi_val = data_array[i]->PI_Avg;
                double meter_val = data_array[i]->Meter_Avg;
                printf("\t%.4f\t%.4f\n", pi_val, meter_val);
            }
            break;
        case LOAD_MA_t:
            strcpy(typestring, "LOAD_mA" );   // "LOAD_MA_LUT.h"
            printf(" DATA:  [ " COLOR_YELLOW "%s" COLOR_RESET " ]\n", typestring );
            printf("\tPi mA\t Meter mA\n");
            for(int i = 0; i<this->num_segments; ++i)
            {
                double pi_val = data_array[i]->PI_Avg;
                double meter_val = data_array[i]->Meter_Avg;
                printf("\t%.3f\t%.3f\n", pi_val, meter_val);
            }
            break;
        case LOAD_V_t:
            strcpy(typestring, "LOAD_V" );   //  "DIODE_V_LUT.h"
            printf(" DATA:  [ " COLOR_YELLOW "%s" COLOR_RESET " ]\n", typestring );
            printf("\tPi V\t Meter V\n");
            for(int i = 0; i<this->num_segments; ++i)
            {
                double pi_val = data_array[i]->PI_Avg;
                double meter_val = data_array[i]->Meter_Avg;
                printf("\t%.3f\t%.3f\n", pi_val, meter_val);
            }
            break;
        case DIODE_V_t:
            strcpy(typestring, "DIODE_V" );    //  "COOL_V_LUT.h"
            printf(" DATA:  [ " COLOR_YELLOW "%s" COLOR_RESET " ]\n", typestring );
            printf("\tPi V \t\t Meter V\n");
            for(int i = 0; i<this->num_segments; ++i)
            {
                double pi_val = data_array[i]->PI_Avg;
                double meter_val = data_array[i]->Meter_Avg;
                printf("\t%.5f\t\t%.5f\n", pi_val, meter_val);
            }
            break;
        case DAC_COUNT_t:
            strcpy(typestring, "DAC_COUNT" );    //  "COOL_V_LUT.h"
            printf(" DATA:  [ " COLOR_YELLOW "%s" COLOR_RESET " ]\n", typestring );

            printf("\tcount \tPi mA\t meter mA\n");
            for(int i = 0; i < this->num_segments; ++i)
            {
                int dac_count = data_array[i]->DAC_count;
                double pi_val = data_array[i]->PI_Avg;
                double meter_val = data_array[i]->Meter_Avg;
                printf("\t%d\t%.3f  %.3f\n", dac_count, pi_val, meter_val);
            }
            break;
    }
}

void DC_Data::Filter_Data()
{   
    this->Order_Segments_By_PI_Value();
    this->Remove_Segments_Meter_Disconnected();
    this->Remove_Segments_Large_Correction_Value();
    this->Delete_Flagged_Segments();
}



int DC_Data::Get_Index_Next_Valid_Segment(int start_index)
{
    DC_Segment* cur_segment;
    for(int i=start_index; i< this->num_segments; ++i)
    {
        cur_segment = this->data_array[i];
        if(cur_segment != NULL)
            if(cur_segment->ignore_segment == true)
                continue;
        else
            return i;
    }
    return -1;
}

void DC_Data::Delete_Flagged_Segments()
{
    DC_Segment* cur_segment;
    DC_Segment* next_segment;

    int num_good_segments = this->num_segments;
    for(int cur_index =0; cur_index < this->num_segments; ++cur_index) 
    {
        cur_segment      = this->data_array[cur_index];
        if( cur_segment->ignore_segment == true )  // segment is flagged as having unusable data
        {
            delete this->data_array[cur_index];
            this->data_array[cur_index] = NULL;
            --num_good_segments;
        }
    }

    for(int cur_index =0; cur_index < this->num_segments; ++cur_index) 
    {
        cur_segment = this->data_array[cur_index];
       
        if( cur_segment == NULL ) 
        {
            int from_index = this->Get_Index_Next_Valid_Segment(cur_index+1);
            if(from_index > 0)
            {
                this->data_array[cur_index] = this->data_array[from_index];
                this->data_array[from_index] = NULL;
            }
            else
            {
                break;
            }
        }
    }
    this->num_segments = num_good_segments;
}

void DC_Data::Remove_Segments_Large_Correction_Value()
{
    bool print_msg = false;

    for(int i = 0; i<this->num_segments; ++i)
    {
        DC_Segment* cur_segment = this->data_array[i];
        if(cur_segment->ignore_segment == false)
        {
            double pi_val = cur_segment->PI_Avg;
            double meter_val = cur_segment->Meter_Avg;
            double error = pi_val - meter_val;
            double percent_error = abs((error / meter_val)*100);
            if(percent_error > 15)
            {
                if( !print_msg)
                {
                    printf("\nRemoving Segments with too much correction to PI value:\n");
                    print_msg = true;
                }
                printf("\tremove segment: \t\t[" COLOR_RED "%.3f   %.3f" COLOR_RESET "] error is too much (" COLOR_RED "%d%%" COLOR_RESET ")\n", cur_segment->PI_Avg, cur_segment->Meter_Avg, (int)percent_error);
                cur_segment->ignore_segment = true;
            }
        }
    }
    if(print_msg)
        printf("\n");
}

void DC_Data::Remove_Segments_Meter_Disconnected()
{
    const char* title = "\nRemoving Bad Data Points:\t\t\tPi\tMeter\n";
    bool printed_title = false;
    //printf("\nRemoving Bad Data Points:\n");
    for(int i = 0; i<this->num_segments; ++i)
    {
        DC_Segment* cur_segment = this->data_array[i];
        if(cur_segment->ignore_segment == false)
        {
            double pi_val = cur_segment->PI_Avg;
            double meter_val = cur_segment->Meter_Avg;
            if( this->LUT_type == COOLER_V_t)
            {
                if( pi_val > 0.2 && meter_val < 0.05)   // if pi measures over 200 mV and meter measures nothing, meter is not measuring the same thing
                {
                    if(!printed_title)
                    { printf("%s", title); printed_title = true; }

                    printf("\tPi > 200mV, meter < 50mV.\t\t[" COLOR_RED "%.4f  %.4f" COLOR_RESET "]\n", pi_val, meter_val);
                    cur_segment->ignore_segment = true;
                }
                if( pi_val < 0.09 && meter_val < 0.09)   // if pi measures over 200 mV and meter measures nothing, meter is not measuring the same thing
                {
                    if(!printed_title)
                    { printf("%s", title); printed_title = true; }

                    printf("\tPi < 90mV, meter < 90mV.\t\t[" COLOR_RED "%.4f  %.4f" COLOR_RESET "]\n", pi_val, meter_val);
                    cur_segment->ignore_segment = true;
                }
            }

            if( this->LUT_type == LOAD_V_t)
            {
                if( pi_val > 0.15 && meter_val < 0.05)
                {
                    if(!printed_title)
                    { printf("%s", title); printed_title = true; }

                    printf("\tPi > 150mV, meter < 50mV.\t\t[" COLOR_RED "%.4f  %.4f" COLOR_RESET "]\n", pi_val, meter_val);
                    cur_segment->ignore_segment = true;
                }
                if( pi_val < 0.09 && meter_val < 0.09)   // if pi measures over 200 mV and meter measures nothing, meter is not measuring the same thing
                {
                    if(!printed_title)
                    { printf("%s", title); printed_title = true; }

                    printf("\tPi < 90mV, meter < 90mV.\t\t[" COLOR_RED "%.4f  %.4f" COLOR_RESET "]\n", pi_val, meter_val);
                    cur_segment->ignore_segment = true;
                }
            }
            if( this->LUT_type == LOAD_MA_t)
            {
                int dac_count = cur_segment->DAC_count;
                if( dac_count > 5000 && meter_val < 5)  // if count is over 5000 & meter measures less than 1 milliAmp, something is wrong
                {
                    if(!printed_title)
                    { printf("%s", title); printed_title = true; }

                    printf("\tDAC count > 5000, meter < 5 mA.\t\t[" COLOR_RED "%.4f  %.4f" COLOR_RESET "]\n", pi_val, meter_val);
                    cur_segment->ignore_segment = true;
                }
                if( pi_val > 1 && meter_val < .01)  // if count is over 5000 & meter measures less than 1 milliAmp, something is wrong
                {
                    if(!printed_title)
                    { printf("%s", title); printed_title = true; }

                    printf("\tPi > 1mA, meter < .01 mA.\t\t[" COLOR_RED "%.4f  %.4f" COLOR_RESET "]\n", pi_val, meter_val);
                    cur_segment->ignore_segment = true;
                }
                if( pi_val < 0.9 || meter_val < .9)  // if value measured by either PI or HP34401A is < 0.95 mA, the measurement is useless
                {
                    if(!printed_title)
                    { printf("%s", title); printed_title = true; }

                    printf("\tPi < 0.95mA or meter < .95 mA.\t\t[" COLOR_RED "%.4f  %.4f" COLOR_RESET "]\n", pi_val, meter_val);
                    cur_segment->ignore_segment = true;
                }
                
            }
            if( this->LUT_type == DIODE_V_t)
            {
                // to do
                
            }
        }
    }
    if(printed_title)
        printf("\n");
}

void DC_Data::Add_New_Data_Segment(DC_Segment* seg)
{
    if(this->num_segments < MAX_DC_SEGMENTS)
    {
        this->data_array[this->num_segments] = seg;
        ++(this->num_segments);
        ++(this->total_segments);
    }
}

int DC_Data::Index_Next_Smallest_PI_Value(int start_index)
{
    double next_lowest_value = this->data_array[start_index]->PI_Avg;
    int index_next_lowest_value = start_index;

    DC_Segment* cur_segment;
    // Iteratively order from smallest to largest
    for(int i = start_index; i< this->num_segments; ++i)
    {
        cur_segment = this->data_array[i];
        double current_val = cur_segment->PI_Avg;

        if(current_val < next_lowest_value)
        {
            next_lowest_value = current_val;
            index_next_lowest_value = i;
        }
    }
    return index_next_lowest_value;
}

void DC_Data::Order_Segments_By_PI_Value()
{
    // start at first spot in array
    // find index of smallest pi value
    // swap pointers
    // go to next index in array
    DC_Segment* cur_seg;
    DC_Segment* swap_seg;

    // Iteratively order from smallest to largest
    //printf("\n\nData Segments:\n");
    //printf("\t PI val   Meter val\t %%correction\n");
    for(int i =0; i< this->num_segments; ++i)
    {
        int cur_index = i;
        int index_smallest_val = this->Index_Next_Smallest_PI_Value(i);

        if(cur_index == index_smallest_val)
            continue;
        
        cur_seg = this->data_array[cur_index];              // get pointers of the two segments
        swap_seg = this->data_array[index_smallest_val];

        this->data_array[cur_index] = swap_seg;             // swap the pointers
        this->data_array[index_smallest_val] = cur_seg;

        //printf("\t %.3f   %.3f\t%.2f\n", this->data_array[cur_index]->PI_Avg, this->data_array[cur_index]->Meter_Avg, 100*(this->data_array[cur_index]->PI_Avg - this->data_array[cur_index]->Meter_Avg)/this->data_array[cur_index]->Meter_Avg);
    }
}

