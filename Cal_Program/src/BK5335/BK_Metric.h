
//#include "TimeH.h"

#ifndef BK_METRIC_H
#define BK_METRIC_H

#include "TStamp.h"

#define BK5335_MAX_DATA_POINTS  1000
#define MAX_SAMPLES_PER_SECOND  10


typedef enum BK5335_meas_t{       DC_VOLTS, AC_VOLTS, RMS_VOLTS, MEAN_VOLTS, PEAK_CURRENT, DC_CURRENT,
                                  AC_CURRENT, RMS_CURRENT, POWER_FACTOR, ACTIVE_POWER, REACTIVE_POWER,
                                  APPARENT_POWER, PHASE_ANGLE } BK5335_meas_t;

// this base class holds any measurement samples, at any timestamp
class BK5335_sample_t
{
public:
    BK5335_sample_t()
    {
        value = 0;
    }
    virtual ~BK5335_sample_t();
    static void BK5335_Print_Sample(BK5335_sample_t* sample);

    Timestamp* timestamp;
    double value;
    BK5335_meas_t measurement_type;
};

// data point for each second as time progresses
class BK5335_Data_Point_t
{
public:
    BK5335_Data_Point_t()
    {
        DC_Volt = -1;
        AC_Volt = -1;
        RMS_Volt = -1;

        AC_Current = -1;
        DC_Current = -1;
        RMS_Current = -1;

        Active_Power = -1;
        Reactive_Power = -1;
        Apparent_Power = -1;
        Power_Factor = -1;
    }
    virtual ~BK5335_Data_Point_t()
    { delete this->timestamp; }

    static void Print_Data_Point(BK5335_Data_Point_t* point);

    double DC_Volt;
    double AC_Volt;
    double RMS_Volt;

    double AC_Current;
    double DC_Current;
    double RMS_Current;

    double Active_Power;
    double Reactive_Power;
    double Apparent_Power;
    double Power_Factor;

    Timestamp* timestamp;
};

class BK5335_metric_t
{
public:
    BK5335_metric_t()
    {
        DC_Volt_samples = NULL;
        AC_Volt_samples = NULL;
        RMS_Volt_samples = NULL;
        AC_Current_samples  = NULL;
        DC_Current_samples = NULL;
        RMS_Current_samples = NULL;
        Active_Power_samples = NULL;
        Apparent_Power_samples = NULL;
        Reactive_Power_samples = NULL;
        Power_Factor_samples = NULL;
        logging_timestamp = NULL;

        Data_Array = (BK5335_Data_Point_t**)(malloc( sizeof(BK5335_Data_Point_t*) * BK5335_MAX_DATA_POINTS ) );
        for( int i=0; i< BK5335_MAX_DATA_POINTS; ++i)
            Data_Array[i] = NULL;   // initialize memory locations to NULL
    }
    virtual ~BK5335_metric_t();

    void Add_Sample(BK5335_sample_t* new_sample);

    void Average_Collected_Samples(BK5335_Data_Point_t* data_point);
    double Average_Samples(BK5335_sample_t** sample_array, int max_index);
    void Clear_Per_Second_Samples(BK5335_sample_t** sample_array, int* max_index);
    void Clear_Data_Array();
    void Clear_All_Data();
    void Print_Data();
    void Print_RMS_Current_Value( BK5335_Data_Point_t* cur_data_point );
    void Print_RMS_Current_Data();
    BK5335_Data_Point_t** Get_Data();

    BK5335_Data_Point_t** Data_Array;
    int Data_Array_index = 0;

    Timestamp* logging_timestamp;
    BK5335_Data_Point_t** Meter_Data;

    BK5335_sample_t** DC_Volt_samples;
    int DC_Volt_index = 0;

    BK5335_sample_t** AC_Volt_samples;
    int AC_Volt_index = 0;

    BK5335_sample_t** RMS_Volt_samples;
    int RMS_Volt_index = 0;

    BK5335_sample_t** AC_Current_samples;
    int AC_Current_index = 0;

    BK5335_sample_t** DC_Current_samples;
    int DC_Current_index = 0;

    BK5335_sample_t** RMS_Current_samples;
    int RMS_Current_index = 0;

    BK5335_sample_t** Active_Power_samples;
    int Active_Power_index = 0;

    BK5335_sample_t** Apparent_Power_samples;
    int Apparent_Power_index = 0;

    BK5335_sample_t** Reactive_Power_samples;
    int Reactive_Power_index = 0;

    BK5335_sample_t** Power_Factor_samples;
    int Power_Factor_index = 0;
};

#endif
