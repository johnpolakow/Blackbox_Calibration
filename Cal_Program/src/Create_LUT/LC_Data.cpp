

#include "LC_Data.h"





    // it is assumed the file pointer is directly after the file header
    // continue to scan data until EOF
    // each data segment is in 10 second intervals, and takes the format: (one data segment shown below)
/*
    11-25-20
    timestamp   Pi mA       BK5335 mA                                             BK AVG VAL    BK STD DEV  BK SPREAD
    15:46:21    [2.53]      [563.80, 493.32, 431.66, 377.70, 330.49]              AVG[439.39]   SD[82.597]  SPR[233.31]
    15:46:22    [2.40]      [289.18, 724.28, 724.28, 633.74]                      AVG[592.87]   SD[179.189] SPR[435.10]
    15:46:23    [54.70]     [554.53, 485.21, 424.56, 371.49, 325.05]              AVG[432.17]   SD[81.238]  SPR[229.47]
    15:46:24    [25.29]     [284.42, 724.28, 724.28]                              AVG[577.66]   SD[207.350] SPR[439.86]
    15:46:25    [25.68]     [633.74, 554.53, 485.21, 424.56, 371.49]              AVG[493.90]   SD[92.844]  SPR[262.25]
    15:46:26    [35.10]     [325.05, 284.42, 724.28, 724.28]                      AVG[514.51]   SD[210.262] SPR[439.86]
    15:46:27    [51.58]     [633.74, 554.53, 485.21, 424.56, 371.49]              AVG[493.90]   SD[92.844]  SPR[262.25]
    15:46:28    [73.43]     [325.05, 284.42, 724.28]                              AVG[444.58]   SD[198.468] SPR[439.86]
    15:46:29    [101.41]    [724.28, 633.74, 554.53, 485.21, 424.56, 371.49]      AVG[532.30]   SD[120.641] SPR[352.79]
    15:46:30    [137.10]    [325.05]                                              AVG[325.05]   SD[0.000]   SPR[0.00]
    ---------------------------------------------------------------------------
*/
void LC_Data::Scan_Data(char* filepath)
{
    printf("Scanning Data ....  \n");
    FILE* orig_file = FileH::Open_Existing_File(filepath);
    //FileH::Read_File_Header(orig_file);
    FileH::Read_File_Header(orig_file, this->mac_addr);
    //printf("AC mac addr: %s\n", this->mac_addr);
    bool End_of_File = false;
    int count_added = 0;

    while( !End_of_File )
    {
        Data_Segment* data_seg = Data_Segment::Scan_Data_Segment(orig_file);
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
    printf("End of File Reached, %d segments scanned\n", count_added);
    FileH::Close_File(orig_file);
}


void LC_Data::Trim_Outlier_mA_Data()
{   
    this->Trim_First_20_Seconds_Sampling_Each_Session();
    this->Remove_Segments_Large_mA_Correction_Value();
    this->Delete_Flagged_Segments();
    this->Order_Segments_By_PI_Value();
}

int LC_Data::Get_Index_Next_Valid_Segment(int start_index)
{
    Data_Segment* cur_segment;
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

void LC_Data::Delete_Flagged_Segments()
{
    Data_Segment* cur_segment;
    Data_Segment* next_segment;

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

void LC_Data::Print_mA_Reading(bool reading_valid, float pi_val, float meter_val, float error_val, float percent_error)
{

    if(reading_valid)
    {
        if(pi_val < 100)        // formatting number of leading and trailing spaces in printf to account for leading zeroes
        {
            printf(COLOR_GREEN "  %.2f ", pi_val);
            //printf(COLOR_GREEN "PI:  %.2f ", pi_val);
        }
        else
        {
            printf(COLOR_GREEN " %.2f ", pi_val);
            //printf(COLOR_GREEN "PI: %.2f ", pi_val);
        }

        if( meter_val < 100)
        {
            //printf(COLOR_GREEN " METER   %.2f \t", meter_val);
            printf(COLOR_GREEN "    %.2f \t", meter_val);
        }
        else
        {
            //printf(COLOR_GREEN " METER  %.2f \t", meter_val);
            printf(COLOR_GREEN "   %.2f \t", meter_val);
        }
        //printf(COLOR_GREEN "Correction: %.2f  %.4f%% error\n" COLOR_RESET, error_val, percent_error);
        printf(COLOR_GREEN " \t%.2f  \t%.4f%% \n" COLOR_RESET, error_val, percent_error);
    }
    else
    {
        if(pi_val < 100)
        {
            //printf(COLOR_RED "PI:  %.2f ", pi_val);
            printf(COLOR_RED "  %.2f ", pi_val);
        }
        else
        {
            //printf(COLOR_RED "PI: %.2f ", pi_val);
            printf(COLOR_RED " %.2f ", pi_val);
        }

        if( meter_val < 100)
        {
            //printf(COLOR_RED " METER   %.2f \t", meter_val);
            printf(COLOR_RED "    %.2f \t", meter_val);
        }
        else
        {
            //printf(COLOR_RED " METER  %.2f \t", meter_val);
            printf(COLOR_RED "   %.2f \t", meter_val);
        }

        printf(COLOR_RED " \t%.2f  \t%.4f%% \n" COLOR_RESET, error_val, percent_error);
    }
}

void LC_Data::Remove_Segments_Large_mA_Correction_Value()
{
    printf("Removing Segments with too much correction to PI value. ");
    printf("Initial number of segments: %d\n", this->num_segments);

    printf("  PI \t   METER \t\tERROR\t%%ERROR\n");
    for(int i = 0; i<this->num_segments; ++i)
    {
        Data_Segment* cur_segment = this->data_array[i];
        if(cur_segment->ignore_segment == false)
        {
            double pi_val = cur_segment->PI_Avg_10_sec;
            double meter_val = cur_segment->Meter_Avg_10_sec;
            double error = pi_val - meter_val;
            double percent_error = abs((error / meter_val)*100);
            if(meter_val < 300  && percent_error >= MAX_PERCENT_CORRECTION_UNDER_300_MILLIAMPS)
            {
                LC_Data::Print_mA_Reading(false, pi_val, meter_val, error, percent_error);
                //printf(COLOR_RED "PI: %.2f, METER %.2f\t Correction: %.2f  %.4f%% error\n" COLOR_RESET, pi_val, meter_val, error, percent_error);
                cur_segment->ignore_segment = true;
                continue;

            }
            if(meter_val >= 300  && meter_val < 600 && percent_error >= MAX_PERCENT_CORRECTION_OVER_300_UNDER_600_MILLIAMPS)
            {
                LC_Data::Print_mA_Reading(false, pi_val, meter_val, error, percent_error);
                //printf(COLOR_RED "PI: %.2f, METER %.2f\t Correction: %.2f  %.4f%% error\n" COLOR_RESET, pi_val, meter_val, error, percent_error);
                cur_segment->ignore_segment = true;
                continue;

            }
            else if(meter_val < 1000 && percent_error >= MAX_PERCENT_CORRECTION_UNDER_1000_MILLIAMPS)
            {
                LC_Data::Print_mA_Reading(false, pi_val, meter_val, error, percent_error);
                //printf(COLOR_RED "PI: %.2f, METER %.2f\t Correction: %.2f  %.4f%% error\n" COLOR_RESET, pi_val, meter_val, error, percent_error);
                cur_segment->ignore_segment = true;
                continue;
            }
            else
            {
                LC_Data::Print_mA_Reading(true, pi_val, meter_val, error, percent_error);
                //printf(COLOR_GREEN "PI: %.2f, METER %.2f\t Correction: %.2f  %.4f%% error\n" COLOR_RESET, pi_val, meter_val, error, percent_error);
                continue;
            }
        }
    }
}

void LC_Data::Add_New_Data_Segment(Data_Segment* seg)
{
    if(this->num_segments < MAX_DATA_SEGMENTS)
    {
        this->data_array[this->num_segments] = seg;
        ++(this->num_segments);
        ++(this->total_segments);
    }
}

int LC_Data::Index_Next_Smallest_PI_Value(int start_index)
{
    double next_lowest_value = this->data_array[start_index]->PI_Avg_10_sec;
    int index_next_lowest_value = start_index;

    Data_Segment* cur_segment;
    // Iteratively order from smallest to largest
    for(int i = start_index; i< this->num_segments; ++i)
    {
        cur_segment = this->data_array[i];
        double current_val = cur_segment->PI_Avg_10_sec;

        if(current_val < next_lowest_value)
        {
            next_lowest_value = current_val;
            index_next_lowest_value = i;
        }
    }
    return index_next_lowest_value;
}

void LC_Data::Order_Segments_By_PI_Value()
{
    // start at first spot in array
    // find index of smallest pi value
    // swap pointers
    // go to next index in array
    Data_Segment* cur_seg;
    Data_Segment* swap_seg;

    // Iteratively order from smallest to largest
    printf("\n\nData Points to Process into LUT:\n");
    printf("\t PI         METER  \t Error \t%%error\n");
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

        float error = this->data_array[cur_index]->PI_Avg_10_sec - this->data_array[cur_index]->Meter_Avg_10_sec;
        float percent_error = 100*(this->data_array[cur_index]->PI_Avg_10_sec-this->data_array[cur_index]->Meter_Avg_10_sec)/this->data_array[cur_index]->Meter_Avg_10_sec;
        printf("\t %.3f   %.3f  \t %02.2f \t%.2f\n", this->data_array[cur_index]->PI_Avg_10_sec, this->data_array[cur_index]->Meter_Avg_10_sec, error, percent_error);
    }
}

// This method is to delete the first 20 seconds of data sampling of each sample session. 
// the log file is an aggregation of multiple sample runs, so this method trims off the first 20 seconds of each run.
// this is done because the first 20 seconds of each run, the pi and the meter largely do not agree with each other so 
// data taken from this time should not be used for calibration LUT points.
void LC_Data::Trim_First_20_Seconds_Sampling_Each_Session()
{
   if( this->data_is_trimmed_for_start_times == true)
        return;

   printf("Trim start of each sampling session: (first 20 seconds removed from every 'session')\n");
   TimeStamp* last_sample_timestamp = NULL;
   TimeStamp* this_sample_timestamp;
   Data_Segment* cur_segment; 
   Data_Segment* next_segment;
   double time_diff = 0;
   for(int i=1; i< this->num_segments; ++i) 
   {
       cur_segment      = this->data_array[i];
       next_segment     = this->data_array[i+1];
       this_sample_timestamp   = cur_segment->start_timestamp;
       

       if(last_sample_timestamp != NULL)
            time_diff = TimeStamp::Time_Difference_Seconds(last_sample_timestamp, this_sample_timestamp);
       
       if( time_diff > 12 || last_sample_timestamp == NULL)  // if difference between consecutive samples of data is more than 12 seconds
       {
           // remove 2 segments (20 seconds data), starting with current segment
           int cur_index = i;

           char* tstart = cur_segment->start_timestamp->toString();
           char* tstop = cur_segment->stop_timestamp->toString();
           char seg1_timespan_str[50];
           StringH::Erase_Num_Chars(seg1_timespan_str, 50);
           sprintf(seg1_timespan_str, "[%s -> %s]", tstart, tstop);
           free(tstart);
           free(tstop);

           tstart = next_segment->start_timestamp->toString();
           tstop = next_segment->stop_timestamp->toString();
           char seg2_timespan_str[50];
           StringH::Erase_Num_Chars(seg2_timespan_str, 50);
           sprintf(seg2_timespan_str, "[%s -> %s]", tstart, tstop);
           free(tstart);
           free(tstop);

           delete this->data_array[cur_index];
           delete this->data_array[cur_index+1];
           
           // now shift everything to the left two places
           int from_index, dest_index;
           for( cur_index; (cur_index+2) < this->num_segments; ++cur_index)
           {
               dest_index = cur_index;
               from_index = cur_index+2;

               this->data_array[dest_index] = this->data_array[from_index];
           }

           this->num_segments -= 2;
           this->num_deleted_segments +=2;
           cur_segment = this->data_array[i];
           last_sample_timestamp   = cur_segment->stop_timestamp;   // update last timestamp to moved segments
           printf("\t --- remove first 2 segments %s  %s \n", seg1_timespan_str, seg2_timespan_str);
           continue;
       }
       last_sample_timestamp   = cur_segment->stop_timestamp;   // updated at end of loop
   }
   printf("\n");
   this->data_is_trimmed_for_start_times = true;
}


