#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./E36468.h"
#include "../Util/Data.h"
#include "../HP34401/HP_34401.h"
#include "../Cal_Points.h"

extern double DCV_Set_Points[];

char* Get_Meter_ID(Meter* dmm)
{
    HP34401_Send_Command(*dmm, GET_ID);
    usleep(ONE_MILLISECOND*75); //Wait after sending

    char response[255];
    StringH::Erase_Num_Chars(response, 255);       // erase newly created buffer

    HP34401_Read_Response(dmm, response);
    // Example response:    HEWLETT-PACKARD,34401A,0,2-1-1
    printf("%s\n", response);

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

void Identify_Meter(Meter* dmm_B)
{
  char* ID2 = Get_Meter_ID(dmm_B);
  strcpy(dmm_B->meter_ID, ID2);
  free(ID2);
  HP34401_Display_Text(dmm_B, dmm_B->meter_ID);
}


void Initialize_Meter(Meter* dmm_B)
{
  strcpy(dmm_B->device_path, "/dev/ttyUSB1");

  printf("\n Opening connection to HP34401 meters ...\n");
  dmm_B->file_descriptor = HP34401_open_serial_port("/dev/ttyUSB1");
  printf("\n");

  printf(" setting HP meters to remote mode ...\n");
  HP34401_Set_Remote(dmm_B);

  printf(" resetting both HP34401 meters ...\n");
  HP34401_Reset(dmm_B);

  Identify_Meter(dmm_B);
  HP34401_Display_Text(dmm_B, "--HP_B--");
}

void Print_Data(Data_Group* data)
{



}

int main( int argc, char** argv )
{

/*
  char* resp = Get_Meter_ID(&E3648A);
  free(resp);
  int instr = E3648A_Query_Instrument(&E3648A);
  printf("instrument: %d\n", instr);
  double setV = Query_Set_Voltage(&E3648A);
  printf("SET VOLTAGE\t%f\n",setV);
*/

  PowerSupply E3648A;
  Initialize_PowerSupply(&E3648A);   
  Get_Power_Supply_ID(&E3648A);

  Meter HP_A;
  Initialize_Meter(&HP_A);            // defined in HP_Measurement_Functions.h
  HP34401_Display_Text(&HP_A, "TEST");
  Configure_Volts_DC(&HP_A);
  Setup_Trigger_10_samples(&HP_A);

  Set_Instrument1(&E3648A);     // sets "Instrument1" as the active instrument to receive commands
  Set_Volts(&E3648A, 0.00);     // start with output voltage of 0
  Set_Output_ON(&E3648A);
  Set_Current(&E3648A, 0.01);   // set current limit to only 10mA because we arent driving anything

  for(int i=0; i<ARRAY_SIZE(DCV_Set_Points); ++i)
  {
      double volt_setpoint = DCV_Set_Points[i];
      Set_Volts(&E3648A, volt_setpoint);

    usleep(ONE_MILLISECOND*500);
    //printf("\ttaking 10 samples with HP34401...\n");
    Data_Group*  V_samples;
    V_samples = HP34401_Average_10_DC_V_Samples(&HP_A);   // returns average of 10 samples

    double average_val = Data::Calc_Average(V_samples);
    char string_val[12];
    sprintf(string_val, "%.6f", average_val);
    HP34401_Display_Text(&HP_A, string_val);    // send the averaged value to the HP display
    printf("BK programmed value: %.3f\n", volt_setpoint);
    printf("HP avg value: " COLOR_LIGHT_MAGENTA "%.6f\n" COLOR_RESET, average_val);
    Print_Data(V_samples);

    Data::Free_Data_Group_Memory(V_samples);
    usleep(ONE_MILLISECOND*10);
  }


  //char* text = Get_Display_Text(&E3648A);
  //printf("Display text: %s\n", text);
  //free(text);
  


  return 0;
}
