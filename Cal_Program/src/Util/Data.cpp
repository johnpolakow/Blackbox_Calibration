

#include "Data.h"

#define PRINT_OUTLIERS false


Data_Group* Data::Discard_Outlier_Points(Data_Group* data)
{
    // throwout points more than 1 std deviation away from the average

    Data_Group* new_array;  // object to copy good data points to new array if throwing out data points

    int data_index = 0;
    int num_samples = data->num_points;
    double AVERAGE = Calc_Average(data);
    double STD_DEV = Calc_Std_Dev(data);

    double max_variance = 0;
    double variance, var_dev_ratio;
    double cur_value;

    bool print_data = Data::Detect_Outliers(data, TWO_STD_DEV);


    // throwout outliers (if variance greater than 1 standard deviation) by setting value to -1 (a flag)
    bool outliers_exist = false;
    for( int i = 0; i<data->num_points; ++i)
    {
        cur_value = data->data_points[ i ];
        variance =abs(cur_value - AVERAGE);    // absolute value of difference
        if( variance > max_variance)
            max_variance = variance;
        if(variance > (2*STD_DEV)  && ((variance/data->average) > .001) )
        {
            if(PRINT_OUTLIERS)
            {
                printf(COLOR_BOLD_RED "\t\t\t%.6f %s\t THROWN OUT..." COLOR_RESET, data->data_points[i], data->units);
                printf(COLOR_BOLD_RED "VARIANCE: %.6f > 2*STD DEV: %.6f\n" COLOR_RESET, abs(data->data_points[i]-AVERAGE), 2*STD_DEV);
            }

            data->data_points[i] = -1;
            outliers_exist = true;
        }
        else
        {
            if(print_data && PRINT_OUTLIERS)
            {
                printf(COLOR_YELLOW "\t\t\t%.6f %s\n" COLOR_RESET, data->data_points[i], data->units);
            }
        }
    }

    if(outliers_exist)
    {
        // copy input array, leaving out outlier points
        new_array = (Data_Group*)malloc( sizeof(Data_Group) ); // only taking 10 samples at a time
        new_array->data_points = (float*)malloc( sizeof(float) * 12);
        new_array->num_points = 0;
        strcpy(new_array->units, data->units);
        strcpy(new_array->description, data->description);
        int new_array_index = 0;

        // deep copy of valid data_points
        for(int i = 0; i<data->num_points; ++i)
        {
            if(data->data_points[i] != -1.0 && data->data_points[i] != -1)
            {
                new_array->data_points[new_array_index] = data->data_points[i];
                ++new_array_index;
                ++new_array->num_points;
            }
        }
        Free_Data_Group_Memory(data);
        new_array->average = Calc_Average(new_array);
        new_array->std_dev = Calc_Std_Dev(new_array);

        var_dev_ratio = max_variance / STD_DEV;
        if(var_dev_ratio > 3 )   // recursively repeating the process if the standard deviation was artificially high from an extreme outlier
        {
            if(PRINT_OUTLIERS)
            {
                printf(COLOR_BOLD_RED "\t\t \u03C3 too big, repeating filter process.\n\n" COLOR_RESET);
                printf("\t\t............\n");
            }
            new_array = Data::Discard_Outlier_Points(new_array);
        }
        return new_array;
    }
    else
    {
        return data;
    }
}

Data_Group* Data::Discard_Outlier_Points(Data_Group* data, int std_dev_threshold)
{
    // throwout points more than X std deviation away from the average

    Data_Group* new_array;  // object to copy good data points to new array if throwing out data points

    int data_index = 0;
    int num_samples = data->num_points;
    double AVERAGE = Calc_Average(data);
    double STD_DEV = Calc_Std_Dev(data);

    double max_variance = 0;
    double variance, var_dev_ratio;
    double cur_value;

    bool print_data = Data::Detect_Outliers(data, std_dev_threshold);

    // throwout outliers (if variance greater than 1 standard deviation) by setting value to -1 (a flag)
    bool outliers_exist = false;
    for( int i = 0; i<data->num_points; ++i)
    {
        cur_value = data->data_points[ i ];
        variance =abs(cur_value - AVERAGE);    // absolute value of difference
        if( variance > max_variance)
            max_variance = variance;
        if(variance > (std_dev_threshold*STD_DEV)  && ((variance/data->average) > .001) )
        {
            if(PRINT_OUTLIERS)
            {
                printf(COLOR_BOLD_RED "\t\t\t%.6f %s\t THROWN OUT..." COLOR_RESET, data->data_points[i], data->units);
                printf(COLOR_BOLD_RED "VARIANCE: %.6f > %d*STD DEV: %.6f\n" COLOR_RESET, abs(data->data_points[i]-AVERAGE), std_dev_threshold, std_dev_threshold*STD_DEV);
            }
            data->data_points[i] = -1;
            outliers_exist = true;
        }
        else
        {
            if(print_data && PRINT_OUTLIERS)
            {
                printf(COLOR_YELLOW "\t\t\t%.6f %s\n" COLOR_RESET, data->data_points[i], data->units);
            }
        }
    }

    if(outliers_exist) // rerun filter process if values have variance than 1% of average value
    {
        // copy input array, leaving out outlier points
        new_array = (Data_Group*)malloc( sizeof(Data_Group) ); // only taking 10 samples at a time
        new_array->data_points = (float*)malloc( sizeof(float) * 12);
        new_array->num_points = 0;
        strcpy(new_array->units, data->units);
        strcpy(new_array->description, data->description);
        int new_array_index = 0;

        // deep copy of valid data_points
        for(int i = 0; i<data->num_points; ++i)
        {
            if(data->data_points[i] != -1.0 && data->data_points[i] != -1)
            {
                new_array->data_points[new_array_index] = data->data_points[i];
                ++new_array_index;
                ++new_array->num_points;
            }
        }
        Free_Data_Group_Memory(data);
        new_array->average = Calc_Average(new_array);
        new_array->std_dev = Calc_Std_Dev(new_array);

        if(((max_variance/new_array->average) > .01))   // recursively repeating the process if the standard deviation was artificially high from an extreme outlier
        {
            if(PRINT_OUTLIERS)
            {
                printf(COLOR_BOLD_RED "\t\t \u03C3 too big,  repeating filter process.\n\n" COLOR_RESET);
                printf("\t\t............\n");
            }
            new_array = Data::Discard_Outlier_Points(new_array, std_dev_threshold);
        }
        return new_array;
    }
    else
    {
        return data;
    }
}

bool Data::Detect_Outliers(Data_Group* data, int std_dev_threshold)
{
    int data_index = 0;
    int num_samples = data->num_points;
    double AVERAGE = Calc_Average(data);
    double STD_DEV = Calc_Std_Dev(data);

    double variance, var_dev_ratio;
    double cur_value;

    bool outliers_exist = false;
    for( int i = 0; i<data->num_points; ++i)
    {
        cur_value = data->data_points[ i ];
        variance =abs(cur_value - AVERAGE);    // absolute value of difference
        if(variance > (std_dev_threshold*STD_DEV)  && ((variance/data->average) > .001) )
        {
            outliers_exist = true;
            return true;
        }
    }
    return outliers_exist;
}


void Data::Print_DataGroup_Compare(Data_Group* data1, Data_Group* data2)
{
    printf(COLOR_BOLD_BLUE "\n\t%s %s:\r\t" COLOR_RESET, data1->description, data1->units);
    printf(MOVE_CURSOR_FORWARD_40);
    printf(COLOR_BOLD_BLUE "%s %s:\n" COLOR_RESET, data2->description, data2->units);
    int i = 0;
    while( (i< data1->num_points) || (i< data2->num_points) )
    {
        if(i< data1->num_points)
        {
            printf(COLOR_YELLOW "\t\t%.6f %s\r" COLOR_RESET, data1->data_points[i], data1->units);
            printf(MOVE_CURSOR_FORWARD_40);
        }
        else
        {
            printf(MOVE_CURSOR_FORWARD_40);
        }
        if(i< data2->num_points)
        {
            printf(COLOR_YELLOW "\t\t%.6f %s" COLOR_RESET, data2->data_points[i], data2->units);
            printf("\n");
        }
        else
        {
            printf("\n");
        }
        ++i;
    }
    printf("    %s AVG:  " COLOR_BOLD_YELLOW "%.6f %s\r" COLOR_RESET, data1->description, data1->average, data1->units );
    printf(MOVE_CURSOR_FORWARD_40);
    printf("    %s AVG:  " COLOR_BOLD_YELLOW "%.6f %s\n" COLOR_RESET, data2->description, data2->average, data2->units );

    printf("%s STD DEV:  " COLOR_BOLD_YELLOW "%.6f\r" COLOR_RESET, data1->description, data1->std_dev);
    printf(MOVE_CURSOR_FORWARD_40);
    printf("%s STD DEV:  " COLOR_BOLD_YELLOW "%.6f\n" COLOR_RESET, data2->description, data2->std_dev);

    printf(COLOR_BOLD_RED "\t\tPI/METER DIFF in AVG Value: %.6f %s\n\n" COLOR_RESET, data2->average - data1->average, data2->units);
}

void Data::Free_Data_Group_Memory(Data_Group* datagroup)
{
    free(datagroup->data_points);
    datagroup->data_points = NULL;
    free(datagroup);
}

void Data::Print_DataGroup(Data_Group* data)
{
    printf(COLOR_BOLD_BLUE "\t%s %s:\n" COLOR_RESET, data->description, data->units);
    int i = 0;
    while( i< data->num_points)
    {
        printf(COLOR_YELLOW "\t\t%.6f %s\n" COLOR_RESET, data->data_points[i], data->units);
        ++i;
    }
    printf("    %s AVG:  " COLOR_BOLD_YELLOW "%.6f %s\n" COLOR_RESET, data->description, data->average, data->units );
    printf("%s STD DEV:  " COLOR_BOLD_YELLOW "%.6f\n\n" COLOR_RESET, data->description, data->std_dev);
}

void Data::Unit_Convert(float* array, float multiply_factor, int num_elements)
{
    int array_index = 0;

    // calculate average
    while( array_index < num_elements )
    {
        array[array_index]*=multiply_factor;
        ++array_index;
    }
}

void Data::Print_Float_Array(float* data, char* description, int num_elements)
{
    int data_index = 0;
    printf(COLOR_BOLD_BLUE "\t%s\n" COLOR_RESET, description);
    while( data_index < num_elements )
    {
        printf(COLOR_YELLOW "\t\t%.6f\n" COLOR_RESET, data[data_index]);
        ++data_index;
    }
}

float Data::Calc_Std_Dev(Data_Group* datagroup)
{
    int data_index = 0;
    float* data = datagroup->data_points;
    int num_elements = datagroup->num_points;
    double data_sum = 0;
    double data_average = 0;
    float SD_sum = 0;
    float SD;

    // calculate average first
    while( data_index < num_elements )
    {
        data_sum += data[data_index];
        ++data_index;
    }
    const int num_samples = data_index;
    data_average = data_sum/(float)num_samples;

    // calc standard deviation
    for(int i = 0; i<num_samples; ++i)
    {
        SD_sum += pow(data[i]-data_average, 2);
    }
    SD = sqrt(SD_sum/(float)num_samples);

    return SD;
}

float Data::Calc_Average(Data_Group* datagroup)
{
    int data_index = 0;
    float* data = datagroup->data_points;
    int num_elements = datagroup->num_points;
    double data_sum = 0;
    double data_average = 0;

    // calculate average
    while( data_index < num_elements )
    {
        data_sum += data[data_index];
        ++data_index;
    }
    const int num_samples = data_index;
    data_average = data_sum/(float)num_samples;

    return data_average;
}
