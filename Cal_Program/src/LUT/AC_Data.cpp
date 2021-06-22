


#include "AC_Data.h"


AC_Data_Group::~AC_Data_Group()
{
    for(int i = 0; i<NUMBER_OF_AC_POINTS; ++i)
    {
        delete data_points[i];
        data_points[i] = NULL;
    }
    data_points = NULL;
    this->num_points = 0;
}

void AC_Data_Group::Print_AC_Data(AC_Data_Group* data)
{
    printf(COLOR_YELLOW "\t\t%s  %s  Data  [%d samples]\n" COLOR_RESET, data->description, data->units, data->num_points);
    for(int i = 0; i< data->num_points; ++i)
    {
        double measurement = data->data_points[i]->value;
        char* timestamp = data->data_points[i]->data_timestamp->toString();
        printf("\t\t%.6f %s\t%s\n", measurement, data->units, timestamp);
    }
}

void AC_Data_Group::Print_AC_Data_Compare(AC_Data_Group* pi_data, AC_Data_Group* yokogawa_data)
{
    // for each pi data point, mark the timestamp
        // print pi data point
        // search through Yokogagwa data points for matching times
            // create data structure for matching timestamps
                // add any matching data to data structure
                // calculate average, and std dev of yokogawa data
                // if too wide of a swing, dont allow it to be used as cal data
                // if points are reasonably close together, add them to a second cal data structure
            // print the contents of the data structure:
            //data points, time (not the date), average, std dev, spread in mA

    printf(COLOR_YELLOW "\t%s \t%s  Timestamp \tYG Avg \tYG Std Dev \tYG spread\n" COLOR_RESET,
                    pi_data->description, yokogawa_data->description);

    for(int i = 0; i< pi_data->num_points; ++i)
    {
        double pi_measurement = pi_data->data_points[i]->value;
        char* pi_data_time = pi_data->data_points[i]->data_timestamp->time.toString();

        printf("\t\t%.6f %s\t", pi_measurement, pi_data->units);
    }
}


float AC_Data_Group::Get_AC_Data_Average(AC_Data_Group* data)
{
    double average;
    double sum = 0;
    int count = 0;
    for(int i = 0; i< data->num_points; ++i)
    {
        double measurement = data->data_points[i]->value;
        sum += measurement;
        ++count;
    }
    average = sum/((float)count);
    return average;
}

int AC_Data_Group::Get_Num_Samples_Below_100(AC_Data_Group* data)
{
    int num_samples_below_100mA = 0;
    for(int i = 0; i< data->num_points; ++i)
    {
        if( data->data_points[i]->value < 100 )
            ++num_samples_below_100mA;
    }
    return num_samples_below_100mA;
}


int AC_Data_Group::Get_Num_Samples_Above_450(AC_Data_Group* data)
{
    int num_samples_above_450mA = 0;
    for(int i = 0; i< data->num_points; ++i)
    {
        if( data->data_points[i]->value > 450 )
            ++num_samples_above_450mA;
    }
    return num_samples_above_450mA;
}


void AC_Data_Group::Add_Sample(Yoko_sample_t* new_sample)
{
    if(this->num_points < NUMBER_OF_AC_POINTS )
    {
        // Convert Yoko_sample_t to AC Data Point, then add AC Data Point
        double value = new_sample->value;
        Timestamp* data_timestamp = new Timestamp();

        // get the timestamp info
        data_timestamp->date.day = new_sample->timestamp->date.day;
        data_timestamp->date.month = new_sample->timestamp->date.month;
        data_timestamp->date.year = new_sample->timestamp->date.year;

        data_timestamp->time.second = new_sample->timestamp->time.second;
        data_timestamp->time.minute = new_sample->timestamp->time.minute;
        data_timestamp->time.hour = new_sample->timestamp->time.hour;

        AC_data_t measurement_type;
        if( new_sample->measurement_type == PEAK_CURRENT ||  new_sample->measurement_type == DC_CURRENT
            || new_sample->measurement_type == AC_CURRENT || new_sample->measurement_type == RMS_CURRENT)
        {
            measurement_type = AC_POWER_MILLIAMPS;
            //value *= 1000;  // convert Amps to milliAmps  (not needed, conversion already done in WT300E.cpp)
        }
        else if( new_sample->measurement_type == DC_VOLTS ||  new_sample->measurement_type == AC_VOLTS
            || new_sample->measurement_type == RMS_VOLTS)
        {
            measurement_type = AC_POWER_VOLTS;
        }
        else
            measurement_type = AC_POWER;

        AC_Data_Point* new_AC_point = new AC_Data_Point(value, data_timestamp, measurement_type);
        data_points[this->num_points] = new_AC_point;
        ++this->num_points;
    }
}
void AC_Data_Group::Clear_All_Data()
{
    for(int i = 0; i<NUMBER_OF_AC_POINTS; ++i)
    {
        delete data_points[i];
        data_points[i] = NULL;
    }
    this->num_points = 0;
}
