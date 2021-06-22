


#include "BK_Metric.h"

#define DEBUG_PRINT false

BK5335_sample_t::~BK5335_sample_t()
{
    delete( this->timestamp );
}

void BK5335_sample_t::BK5335_Print_Sample(BK5335_sample_t* sample)
{
    char units[4];
    if(sample->measurement_type == DC_CURRENT || sample->measurement_type == AC_CURRENT || sample->measurement_type == RMS_CURRENT)
        strcpy(units, "A");
    else if(sample->measurement_type == DC_VOLTS || sample->measurement_type == AC_VOLTS || sample->measurement_type == RMS_VOLTS)
        strcpy(units, "V");

    char* timestamp = sample->timestamp->toString();
    double value = sample->value;
    printf("%.6f %s\t%s\n", value, units, timestamp);

}

BK5335_metric_t::~BK5335_metric_t()
{
    this->Clear_Per_Second_Samples( DC_Volt_samples, &DC_Volt_index);
    this->Clear_Per_Second_Samples( AC_Volt_samples, &AC_Volt_index);
    this->Clear_Per_Second_Samples( RMS_Volt_samples, &RMS_Volt_index);
    this->Clear_Per_Second_Samples( DC_Current_samples, &DC_Current_index);
    this->Clear_Per_Second_Samples( AC_Current_samples, &AC_Current_index);
    this->Clear_Per_Second_Samples( RMS_Current_samples, &RMS_Current_index);
    this->Clear_Per_Second_Samples( Power_Factor_samples, &Power_Factor_index);
    this->Clear_Per_Second_Samples( Apparent_Power_samples, &Apparent_Power_index);
    this->Clear_Per_Second_Samples( Active_Power_samples,   &Active_Power_index);
    this->Clear_Per_Second_Samples( Reactive_Power_samples, &Reactive_Power_index);
    this->Clear_Data_Array();
    delete Data_Array;
}

void BK5335_metric_t::Print_Data()
{
    BK5335_Data_Point_t* cur_data_point;
    printf("\tBK DATA ARRAY:\n");
    for(int i = 0 ; i< this->Data_Array_index; ++i)
    {
      if( this->Data_Array[i] != NULL)
      {
          cur_data_point = this->Data_Array[i];
          BK5335_Data_Point_t::Print_Data_Point( cur_data_point );
      }
    }
}

void BK5335_metric_t::Print_RMS_Current_Data()
{
    BK5335_Data_Point_t* cur_data_point;
    printf("\tBK RMS CURRENT:\n");
    for(int i = 0 ; i< this->Data_Array_index; ++i)
    {
      if( this->Data_Array[i] != NULL)
      {
          cur_data_point = this->Data_Array[i];
          if( cur_data_point->RMS_Current != -1)
            BK5335_metric_t::Print_RMS_Current_Value( cur_data_point );
      }
    }
}

void BK5335_Data_Point_t::Print_Data_Point( BK5335_Data_Point_t* cur_data_point )
{
    char* timestamp = cur_data_point->timestamp->toString();
    printf("\t\t%s\n", timestamp  );
    free(timestamp);
    if( cur_data_point->DC_Volt != -1 )
        printf("\t\t   DC_Volt:  %.6f V\n", cur_data_point->DC_Volt);

    if( cur_data_point->AC_Volt != -1 )
        printf("\t\t   AC_Volt:  %.6f V\n", cur_data_point->AC_Volt);

    if( cur_data_point->RMS_Volt != -1 )
        printf("\t\t   RMS_Volt:   %.6f V\n", cur_data_point->RMS_Volt);

    if( cur_data_point->AC_Current != -1 )
            printf("\t\t   AC_Current:  %.6f A\n", cur_data_point->AC_Current);

    if( cur_data_point->DC_Current != -1 )
            printf("\t\t   DC_Current:  %.6f A\n", cur_data_point->DC_Current);

    if( cur_data_point->RMS_Current != -1 )
            printf("\t\t   RMS_Current:  %.6f A\n", cur_data_point->RMS_Current);

    if( cur_data_point->Active_Power != -1 )
            printf("\t\t   Active_Power:  %.6f W\n", cur_data_point->Active_Power);

    if( cur_data_point->Reactive_Power != -1 )
            printf("\t\t   Reactive_Power:  %.6f W\n", cur_data_point->Reactive_Power);

    if( cur_data_point->Apparent_Power != -1 )
            printf("\t\t   Apparent_Power:  %.6f W\n", cur_data_point->Apparent_Power);

    if( cur_data_point->Power_Factor != -1 )
                printf("\t\t\tPower_Factor:  %.3f\n", cur_data_point->Power_Factor);
}


void BK5335_metric_t::Print_RMS_Current_Value( BK5335_Data_Point_t* cur_data_point )
{
    char* timestamp = cur_data_point->timestamp->toString();


    if( cur_data_point->RMS_Current != -1 )
        printf("\t\t  %.6f A\t  %s\n", cur_data_point->RMS_Current, timestamp);

    if( timestamp != NULL)
        free(timestamp);
}

void BK5335_metric_t::Add_Sample(BK5335_sample_t* new_sample)
{
    BK5335_meas_t new_meas_type = new_sample->measurement_type;
    Timestamp* time_now = new Timestamp();
    if( this->logging_timestamp == NULL)
    {
        this->logging_timestamp = new Timestamp();                      // it is only NULL in the case of the first sample
    }
    else if( !( time_now->Equals(this->logging_timestamp) ) )           // if current second has updated to a new second,
    {                                                                   // store existing data to "new_data_point"
        BK5335_Data_Point_t* new_data_point = new BK5335_Data_Point_t();
        new_data_point->timestamp = this->logging_timestamp;            // assign timestamp data was logged at to new_data_point
        this->Average_Collected_Samples( new_data_point );              // Average existing samples ( and free associated memory )

        this->Data_Array[ this->Data_Array_index ] = new_data_point;    // insert data point into new storage
        ++Data_Array_index;
        this->logging_timestamp = time_now;                             // logging time gets current time
    }

    switch( new_meas_type )
    {
        case DC_VOLTS:
            if( DC_Volt_samples == NULL )
            {
                printf("Allocating mem for DCV array \n");
                DC_Volt_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            }
            this->DC_Volt_samples[ this->DC_Volt_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tDCV add %f at index: %d\n", this->DC_Volt_samples[ this->DC_Volt_index]->value, this->DC_Volt_index);
            ++this->DC_Volt_index;

            break;
        case RMS_VOLTS:
            if( RMS_Volt_samples == NULL )
            {
                printf("Allocating mem for RMSV array \n");
                RMS_Volt_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            }
            this->RMS_Volt_samples[ this->RMS_Volt_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tRMSV add %f at index: %d\n", this->RMS_Volt_samples[ this->RMS_Volt_index ]->value, this->RMS_Volt_index );
            ++RMS_Volt_index;
            break;

        case DC_CURRENT:
            if( this->DC_Current_samples == NULL )
                this->DC_Current_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            this->DC_Current_samples[ this->DC_Current_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tDCI add %f at index: %d\n", this->DC_Current_samples[ this->DC_Current_index ]->value, this->DC_Current_index);
            ++DC_Current_index;
            //printf("DC Current Index: %d\n", DC_Current_index);
            break;

        case RMS_CURRENT:
            if( RMS_Current_samples == NULL )
                RMS_Current_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            this->RMS_Current_samples[ this->RMS_Current_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tRMSI  add %f at index: %d\n", this->RMS_Current_samples[ this->RMS_Current_index ]->value, this->RMS_Current_index);
            ++RMS_Current_index;
            break;

        case POWER_FACTOR:
            if( Power_Factor_samples == NULL )
                Power_Factor_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            this->Power_Factor_samples[ this->Power_Factor_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tPF add %f at index: %d\n", this->Power_Factor_samples[ this->Power_Factor_index ]->value , this->Power_Factor_index );
            ++Power_Factor_index;
            break;

        case ACTIVE_POWER:
            if( Active_Power_samples == NULL )
                Active_Power_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            this->Active_Power_samples[ this->Active_Power_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tACTP add %f at index: %d\n", this->Active_Power_samples[ this->Active_Power_index ]->value, this->Active_Power_index);
            ++Active_Power_index;
            break;

        case REACTIVE_POWER:
            if( Reactive_Power_samples == NULL )
                Reactive_Power_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            this->Reactive_Power_samples[ this->Reactive_Power_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tRP add %f at index: %d\n", this->Reactive_Power_samples[ this->Reactive_Power_index ]->value , this->Reactive_Power_index );
            ++Reactive_Power_index;
            break;

        case APPARENT_POWER:
            if( Apparent_Power_samples == NULL )
                Apparent_Power_samples = (BK5335_sample_t**)malloc( sizeof(BK5335_sample_t*) * MAX_SAMPLES_PER_SECOND );
            this->Apparent_Power_samples[ this->Apparent_Power_index ] = new_sample;
            if(DEBUG_PRINT) printf("\tAPP add %f at index: %d\n", this->Apparent_Power_samples[ this->Apparent_Power_index ]->value, this->Apparent_Power_index );
            ++Apparent_Power_index;
            break;
    }
}

// Assign average values of each measurement to data point
void BK5335_metric_t::Average_Collected_Samples(BK5335_Data_Point_t* data_point)
{
    if( DC_Volt_index > 0)
    {
        data_point->DC_Volt = BK5335_metric_t::Average_Samples( DC_Volt_samples, DC_Volt_index);
        this->Clear_Per_Second_Samples( DC_Volt_samples, &DC_Volt_index);
    }
    if( AC_Volt_index > 0)
    {
        data_point->AC_Volt = BK5335_metric_t::Average_Samples( AC_Volt_samples, AC_Volt_index);
        this->Clear_Per_Second_Samples( AC_Volt_samples, &AC_Volt_index);
    }
    if( RMS_Volt_index > 0)
    {
        data_point->RMS_Volt = BK5335_metric_t::Average_Samples( RMS_Volt_samples, RMS_Volt_index);
        this->Clear_Per_Second_Samples( RMS_Volt_samples, &RMS_Volt_index);
    }
    if( DC_Current_index > 0)
    {
        data_point->DC_Current = BK5335_metric_t::Average_Samples( DC_Current_samples, DC_Current_index);
        this->Clear_Per_Second_Samples( DC_Current_samples, &DC_Current_index);
    }
    if( AC_Current_index > 0)
    {
        data_point->AC_Current = BK5335_metric_t::Average_Samples( AC_Current_samples, AC_Current_index);
        this->Clear_Per_Second_Samples( AC_Current_samples, &AC_Current_index);
    }
    if( RMS_Current_index > 0)
    {
        data_point->RMS_Current = BK5335_metric_t::Average_Samples( RMS_Current_samples, RMS_Current_index);
        this->Clear_Per_Second_Samples( RMS_Current_samples, &RMS_Current_index);
    }
    if( Power_Factor_index > 0)
    {
        data_point->Power_Factor = BK5335_metric_t::Average_Samples( Power_Factor_samples, Power_Factor_index);
        this->Clear_Per_Second_Samples( Power_Factor_samples, &Power_Factor_index);
    }
    if( Apparent_Power_index > 0)
    {
        data_point->Apparent_Power = BK5335_metric_t::Average_Samples( Apparent_Power_samples, Apparent_Power_index);
        this->Clear_Per_Second_Samples( Apparent_Power_samples, &Apparent_Power_index);
    }
    if( Active_Power_index > 0)
    {
        data_point->Active_Power = BK5335_metric_t::Average_Samples( Active_Power_samples, Active_Power_index);
        this->Clear_Per_Second_Samples( Active_Power_samples, &Active_Power_index);
    }
    if( Reactive_Power_index > 0)
    {
        data_point->Reactive_Power = BK5335_metric_t::Average_Samples( Reactive_Power_samples, Reactive_Power_index);
        this->Clear_Per_Second_Samples( Reactive_Power_samples, &Reactive_Power_index);
    }
}

void BK5335_metric_t::Clear_All_Data()
{
    //printf("CLEAR ALL DATA\n");
    if( DC_Volt_index > 0)
        this->Clear_Per_Second_Samples( this->DC_Volt_samples,        &DC_Volt_index);

    if( AC_Volt_index > 0)
        this->Clear_Per_Second_Samples( this->AC_Volt_samples,        &AC_Volt_index);

    if( RMS_Volt_index > 0)
        this->Clear_Per_Second_Samples( this->RMS_Volt_samples,       &RMS_Volt_index);

    if( DC_Current_index > 0)
        this->Clear_Per_Second_Samples( this->DC_Current_samples,     &DC_Current_index);

    if( AC_Current_index > 0)
        this->Clear_Per_Second_Samples( this->AC_Current_samples,     &AC_Current_index);

    if( RMS_Current_index > 0)
        this->Clear_Per_Second_Samples( this->RMS_Current_samples,    &RMS_Current_index);

    if( Power_Factor_index > 0)
        this->Clear_Per_Second_Samples( this->Power_Factor_samples,   &Power_Factor_index);

    if( Apparent_Power_index > 0)
        this->Clear_Per_Second_Samples( this->Apparent_Power_samples, &Apparent_Power_index);

    if( Active_Power_index > 0)
        this->Clear_Per_Second_Samples( this->Active_Power_samples,   &Active_Power_index);

    if( Reactive_Power_index > 0)
    this->Clear_Per_Second_Samples( this->Reactive_Power_samples, &Reactive_Power_index);

    this->Clear_Data_Array();
}

// Averages values together contained in current "second" of sample_array
double BK5335_metric_t::Average_Samples(BK5335_sample_t** sample_array, int max_index)
{
    char label[50];
    if(sample_array == DC_Volt_samples)
    {    strcpy(label, "DC_Volt_samples"); }
    else if(sample_array == AC_Volt_samples)
    {    strcpy(label, "AC_Volt_samples"); }
    else if(sample_array == RMS_Volt_samples)
    {    strcpy(label, "RMS_Volt_samples"); }
    if(sample_array == AC_Current_samples)
    {    strcpy(label, "AC_Current_samples"); }
    else if(sample_array == DC_Current_samples)
    {    strcpy(label, "DC_Current_samples");  }
    else if(sample_array == RMS_Current_samples)
    {    strcpy(label, "RMS_Current_samples"); }

    if(DEBUG_PRINT) printf("Averaging %s \n", label);

      if( sample_array == NULL )
      {
          return -1;
      }

    if( max_index == 0)
        return -1;

    double value_sum = 0;
    int num_samples = 0;
    for(int i = 0; i < max_index; ++i)
    {
        value_sum += sample_array[i]->value;
        ++num_samples;
    }
    double average_value = value_sum / (double)num_samples;

    return average_value;
}

BK5335_Data_Point_t** BK5335_metric_t::Get_Data()
{
    return this->Data_Array;
}


 // frees all memory and clears out stored data
void BK5335_metric_t::Clear_Per_Second_Samples(BK5335_sample_t** sample_array, int* max_index)
{
    char label[50];
    if(sample_array == DC_Volt_samples)
        strcpy(label, "DC_Volt_samples");
    else if(sample_array == AC_Volt_samples)
        strcpy(label, "AC_Volt_samples");
    else if(sample_array == RMS_Volt_samples)
        strcpy(label, "RMS_Volt_samples");
    if(sample_array == AC_Current_samples)
        strcpy(label, "AC_Current_samples");
    else if(sample_array == DC_Current_samples)
        strcpy(label, "DC_Current_samples");
    else if(sample_array == RMS_Current_samples)
        strcpy(label, "RMS_Current_samples");

    if(DEBUG_PRINT) printf("Clearing %s \n", label);

    if( *max_index == 0 || sample_array == NULL || sample_array[0] == NULL)
    {
        return;
    }

    for(int i = 0; i < *max_index; ++i)
    {
        if( sample_array[i] != NULL )
        {
            delete sample_array[i];
            sample_array[i] = NULL;
        }
    }

    *max_index = 0;
    this->logging_timestamp = NULL;
    return;
}

void BK5335_metric_t::Clear_Data_Array()
{
    for(int i = 0; i < this->Data_Array_index; ++i)
    {
        if( this->Data_Array[i] != NULL )
        {
            delete this->Data_Array[i];
            this->Data_Array[i] = NULL;
        }
    }

    this->Data_Array_index = 0;
    return;
}
