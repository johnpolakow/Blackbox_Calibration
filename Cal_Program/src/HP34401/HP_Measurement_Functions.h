

#ifndef _HP_HELPER_H
#define _HP_HELPER_H

#include "HP_34401.h"
#include "Data.h"
#include "relay.h"

#ifdef __cplusplus
extern "C" {
#endif


// In release mode the HP meter throws away the first round of readings, taking more time to run through the Cal
#define RELEASE_MODE false

char* Get_Meter_ID(Meter* dmm);
void Identify_Meters(Meter* dmm_A, Meter* dmm_B);
void Initialize_Meters(Meter* dmm_A, Meter* dmm_B);

void Cal_Diode_Voltage(Meter* HP_A, Meter* HP_B);
Data_Group* Measure_REF100(Meter* HP_A, Meter* HP_B);
Data_Group* Measure_Cooler_V(Meter* HP_A, Meter* HP_B);
Data_Group* Read_V_Load(Meter* HP_A);
Data_Group* Read_Load_milliAmps(Meter* HP_B);
void Initialize_Meter(Meter* dmm);
void Identify_Meter(Meter* dmm);

// returns HP34401A ID string. Memory for the string must be freed afterwards
char* Get_Meter_ID(Meter* dmm)
{
    HP34401_Send_Command(*dmm, GET_ID);
    usleep(ONE_MILLISECOND*75); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    HP34401_Read_Response(dmm, response);
    // Example response:    HEWLETT-PACKARD,34401A,0,2-1-1

    char delims[] = ",";    // delimiters to be used in parsing out tokens
    char* token = strtok(response, delims);

    if( token != NULL && strlen(token) > 0)
    {
        bool correct = StringH::String_Contains(token, "HEWLETT-PACKARD");
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "HEWLETT-PACKARD");

        token = strtok(NULL, delims);
        correct = StringH::String_Contains(token, "34401A");
        if(!correct)
            printf("NOT CORRECT! %s does not match %s\n", token, "34401A");

        char* ID = (char*)malloc( sizeof(char) * 15); // ID actually should only be 7 characters long
        StringH::Erase_Num_Chars(ID, 10);

        token = strtok(NULL, delims);
        strncpy(ID, token, 2);
        char comma[] = ",";
        strcat(ID, comma);

        token = strtok(NULL, delims);
        strcat(ID, token);

        int meter_number = parse_USB_Number(dmm->device_path);
        printf("\tHP34401 ID%d: " COLOR_MAGENTA "\t%s" COLOR_RESET, meter_number, ID);

        return ID;
    }
    else
        printf("Could not read response from HP meter.\n");
}

// Displays the ID string on the display of each meter
void Identify_Meters(Meter* dmm_A, Meter* dmm_B)
{
  char* ID1 = Get_Meter_ID(dmm_A);
  char* ID2 = Get_Meter_ID(dmm_B);
  strcpy(dmm_A->meter_ID, ID1);
  strcpy(dmm_B->meter_ID, ID2);
  free(ID1);
  free(ID2);

  HP34401_Display_Text(dmm_A, dmm_A->meter_ID);
  HP34401_Display_Text(dmm_B, dmm_B->meter_ID);
}

// Displays the ID string on the display of each meter
void Identify_Meter(Meter* dmm)
{
  char* ID1 = Get_Meter_ID(dmm);
  strcpy(dmm->meter_ID, ID1);
  free(ID1);

  HP34401_Display_Text(dmm, dmm->meter_ID);
}

void Initialize_Meter(Meter* dmm)
{
  printf("\n Opening connection to HP34401 meter ...\n");
  dmm->file_descriptor = HP34401_open_serial_port(dmm->device_path);
  printf("\tmfr: %s\n", dmm->manufacturer);
  printf("\t--setting HP meter to remote mode ...\n");
  HP34401_Set_Remote(dmm);
  printf("\t--resetting meter ...\n");
  HP34401_Reset(dmm);
  Identify_Meter(dmm);

  HP34401_Display_Text(dmm, dmm->text);
}



void Initialize_Meters(Meter* dmm_A, Meter* dmm_B)
{
  printf("\n Opening connection to HP34401 meters ...\n");
  dmm_A->file_descriptor = HP34401_open_serial_port(dmm_A->device_path);
  dmm_B->file_descriptor = HP34401_open_serial_port(dmm_B->device_path);
  printf("\n");

  printf(" setting HP meters to remote mode ...\n");
  HP34401_Set_Remote(dmm_A);
  HP34401_Set_Remote(dmm_B);

  printf(" resetting both HP34401 meters ...\n");
  HP34401_Reset(dmm_A);
  HP34401_Reset(dmm_B);

  Identify_Meters(dmm_A, dmm_B);

  HP34401_Display_Text(dmm_A, "--HP_A--");
  HP34401_Display_Text(dmm_B, "--HP_B--");
}

// configures meter HP34401_B to measure current
// sets up HP34401B to take 10 readings
// Sets the relay box to measure the REF100 current source through HP34401B
// takes the readings, returns the values in units of milliAmps
// the REF100 current source is 100uA, so converts the returned values to units of microAmps by multiplying by 1000
// resets all relays to OFF
// returns the measured microAmp readings
Data_Group* Measure_REF100(Meter* HP_A, Meter* HP_B)
{
  HP34401_Display_Text(HP_B, "REF100 TEST");
  HP34401_Display_Text(HP_A, "-- HP_A --");
  Configure_Current_DC(HP_B);
  Setup_Trigger_10_samples(HP_B);
  delayMicroseconds(ONE_MILLISECOND*10);
  printf("\tsetting relays...  ");
  Set_Relay_REF100();

  float average_val;
  Data_Group* uA_measurements;
  uA_measurements = HP34401_Average_10_DC_mA_Samples(HP_B);
  Data::Unit_Convert(uA_measurements->data_points, 1000, uA_measurements->num_points); // convert mA to uA by multiplying by 1000.
  uA_measurements->average = Data::Calc_Average(uA_measurements);
  uA_measurements->std_dev = Data::Calc_Std_Dev(uA_measurements);
  strcpy(uA_measurements->units, "uA");
  strcpy(uA_measurements->description, "REF100");

  delayMicroseconds(ONE_MILLISECOND);
  Reset_All_Relays(false);

  return uA_measurements;
}

// PRECONDITION: HP34401_B meter has already be setup to measure current with appropriate trigger and sample settings
//              -- Configure_Current_DC()       ... located in HP_34401.h
//              -- Setup_Trigger_10_samples()   ... located in HP_34401.h
// This function takes 10 current samples of meter HP34401B, and returns the values as object type Data_Group* in units of milliAmps
// must free the memory used to store the Data_Group object after done with return values
Data_Group* Read_Load_milliAmps(Meter* HP_B)
{
  Data_Group* Load_mA;
  delayMicroseconds(ONE_MILLISECOND);
  Load_mA = HP34401_Average_10_DC_mA_Samples(HP_B);
  Load_mA->average = Data::Calc_Average(Load_mA);
  Load_mA->std_dev = Data::Calc_Std_Dev(Load_mA);
  strcpy(Load_mA->units, "mA");
  strcpy(Load_mA->description, "HP Load");

  return Load_mA;
}

// PRECONDITION: HP34401_A meter has already be setup to measure voltage with appropriate trigger and sample settings
//              -- Configure_Voltage_DC()       ... located in HP_34401.h
//              -- Setup_Trigger_10_samples()   ... located in HP_34401.h
// This function takes 10 volt samples of meter HP34401B, and returns the values as object type Data_Group* in units of Volts
// must free the memory used to store the Data_Group object after done with return values
Data_Group* Read_V_Load(Meter* HP_A)
{
  Data_Group* Load_V;
  delayMicroseconds(ONE_MILLISECOND);
  Load_V = HP34401_Average_10_DC_V_Samples(HP_A);
  Load_V->average = Data::Calc_Average(Load_V);
  Load_V->std_dev = Data::Calc_Std_Dev(Load_V);
  strcpy(Load_V->units, "V");
  strcpy(Load_V->description, "HP Load");

  return Load_V;
}

#ifdef __cplusplus
}
#endif


#endif
