
#include "Thermocouple.h"




// Destructor
Thermocouple_Data::~Thermocouple_Data()
{
    this->data.clear();
}


// parses 3 thermocouple readings from Pi response to "?metrics" query
// each grouping of data should be 10 samples. Each "Sample" Is a 1 second snapshot in time of all PI Metrics
// Example line:         0.052V  0.0023A  0.000W    171.5°K   0.8755V    0.00V    0.0㎃    0.0㎽    24.55℃   22.99℃   23.16℃   16:20:56  05/27/2021      not started      OFF
void Thermocouple::Parse_Thermocouple_Data(char* pi_string)
{
    int parse_len = strlen(pi_string) + 10;
    char parse_string[ parse_len ];
    StringH::Erase_Num_Chars( parse_string, parse_len );
    strcpy( parse_string, pi_string );    // make copy of input string

    char* token = strtok(parse_string, " \t");   // tokenize the string by space or tab
    int thrmcpl_number = 0;

    while( token != NULL && thrmcpl_number < 3 )
    {
        if( StringH::String_Contains(token, "℃") )
        {
            double temp_C = StringH::Parse_Double( token );
            switch(thrmcpl_number)
            {
                case 0:
                    this->thermocouple1_tempC = temp_C;
                    ++thrmcpl_number;
                    break;
                case 1:
                    this->thermocouple2_tempC = temp_C;
                    ++thrmcpl_number;
                    break;
                case 2:
                    this->thermocouple3_tempC = temp_C;
                    ++thrmcpl_number;
                    break;
                default:
                    break;
            }
        }
        token = strtok(NULL, " ");   // get next token
    }
    return;
}





void Thermocouple_Data::Add_Sample(char* pi_string)
{
    Thermocouple* sample = new Thermocouple(pi_string);

    this->data.push_back(sample);
}



Data_Group* Thermocouple_Data::Get_Thermocouple1_Data()
{
    int vector_size = this->data.size();

    Data_Group* datagroup = new Data_Group("Thermocouple#1");
    datagroup->data_points = (float*)malloc( sizeof(float) * vector_size);

    int num_points = 0;
    for(int i = 0; i< vector_size; ++i)
    {
        datagroup->data_points[i] = this->data[i]->thermocouple1_tempC;
        ++num_points;
    }
    datagroup->num_points = num_points;

    datagroup->average = Data::Calc_Average(datagroup);
    datagroup->std_dev = Data::Calc_Std_Dev(datagroup);
    strcpy(datagroup->units, "C");

    return datagroup;
}

Data_Group* Thermocouple_Data::Get_Thermocouple2_Data()
{
    int vector_size = this->data.size();

    Data_Group* datagroup = new Data_Group("Thermocouple#2");
    datagroup->data_points = (float*)malloc( sizeof(float) * vector_size);

    int num_points = 0;
    for(int i = 0; i< vector_size; ++i)
    {
        datagroup->data_points[i] = this->data[i]->thermocouple2_tempC;
        ++num_points;
    }
    datagroup->num_points = num_points;

    datagroup->average = Data::Calc_Average(datagroup);
    datagroup->std_dev = Data::Calc_Std_Dev(datagroup);
    strcpy(datagroup->units, "C");

    return datagroup;
}

Data_Group* Thermocouple_Data::Get_Thermocouple3_Data()
{
    int vector_size = this->data.size();

    Data_Group* datagroup = new Data_Group("Thermocouple#3");
    datagroup->data_points = (float*)malloc( sizeof(float) * vector_size);

    int num_points = 0;
    for(int i = 0; i< vector_size; ++i)
    {
        datagroup->data_points[i] = this->data[i]->thermocouple3_tempC;
        ++num_points;
    }
    datagroup->num_points = num_points;

    datagroup->average = Data::Calc_Average(datagroup);
    datagroup->std_dev = Data::Calc_Std_Dev(datagroup);
    strcpy(datagroup->units, "C");

    return datagroup;
}

//  \u3383\t   \u33BD         \u2103       \u2103       \u2103 
void Thermocouple_Data::Print_Thermcouple_Data()
{
    int vector_size = this->data.size();
    printf(COLOR_LIGHT_MAGENTA "\tTHRMCPL1 \tTHRMCPL2 \tTHRMCPL3\n" COLOR_RESET);

    this->Calcs();  // calculate average and std dev

    for(int i = 0; i< vector_size; ++i)
    {
        float therm1, therm2, therm3;
        therm1 = this->data[i]->thermocouple1_tempC;
        therm2 = this->data[i]->thermocouple2_tempC;
        therm3 = this->data[i]->thermocouple3_tempC;
        printf(COLOR_YELLOW "\t%.2f\t\t%.2f\t\t%.2f\n" COLOR_RESET, therm1, therm2, therm3);
    }

    printf(COLOR_BOLD_YELLOW "\n   AVG:\t%.2f\t\t%.2f\t\t%.2f  \u00B0C\n" COLOR_RESET, this->avg1, this->avg2, this->avg3);
    printf(COLOR_BOLD_YELLOW " STDEV:  %.2f\t\t %.2f\t\t %.2f  \u00B0C\n" COLOR_RESET, this->stdev1, this->stdev2, this->stdev3);
}

void Thermocouple_Data::Calcs()
{
    int vector_size = this->data.size();

    float sum1 = 0;
    float sum2 = 0;
    float sum3 = 0;

    double SD_sum1 = 0;
    double SD_sum2 = 0;
    double SD_sum3 = 0;

    float StDev1, StDev2, StDev3;

    for(int i = 0; i< vector_size; ++i)
    {
        float therm1, therm2, therm3;
        therm1 = this->data[i]->thermocouple1_tempC;
        therm2 = this->data[i]->thermocouple2_tempC;
        therm3 = this->data[i]->thermocouple3_tempC;
        sum1 += therm1;
        sum2 += therm2;
        sum3 += therm3;
    }
    this->avg1 = sum1/vector_size;
    this->avg2 = sum2/vector_size;
    this->avg3 = sum3/vector_size;

    for(int i = 0; i< vector_size; ++i)
    {
        float therm1, therm2, therm3;
        float variance1, variance2, variance3;
        therm1 = this->data[i]->thermocouple1_tempC;
        therm2 = this->data[i]->thermocouple2_tempC;
        therm3 = this->data[i]->thermocouple3_tempC;

        variance1 = therm1 - avg1;
        variance2 = therm2 - avg2;
        variance3 = therm3 - avg3;

        SD_sum1 += pow(variance1, 2);
        SD_sum2 += pow(variance2, 2);
        SD_sum3 += pow(variance3, 2);
    }


    this->stdev1 = sqrt(SD_sum1/(double)vector_size);
    this->stdev2 = sqrt(SD_sum2/(double)vector_size);
    this->stdev3 = sqrt(SD_sum3/(double)vector_size);
}