
#ifndef _AUTO_CALIBRATE_H
#define _AUTO_CALIBRATE_H

#include "./PI/Pi.h"
#include "Data.h"
#include "./HP34401/HP_34401.h"
#include "./E3648A/E36468.h"
#include "./HP34401/HP_Measurement_Functions.h"
#include "AC_Data.h"
#include "LUT.h"
#include "./Util/Write_Log.h"
#include "Menu.h"
#include "Cal_Parameters.h"
#include "./Yokogawa/WT300E.h"
#include "DC_Data.h"

extern double DCV_Set_Points[];
extern double Diode_Cal_Points[];

#ifdef __cplusplus
extern "C" {
#endif


void Auto_Calibrate_Load_Increments(PI* pi, Meter* dmm_A, Meter* dmm_B, int DAC_INCREMENT, Write_Log* FileWrite);
void Auto_Calibrate_Cooler_V_DC(PI* pi, Meter* HP34401A, Meter* HP34401B, PowerSupply* E3648A, Write_Log* FileWrite);
void Auto_AC_Power_Calibration(PI* pi, WT300E* yokogawa, Write_Log* FileWrite);
void Auto_Calibrate_Diode_Volts(PI* pi, Meter* HP34401A, PowerSupply* E3648A, Write_Log* FileWrite);
void Change_Cables_Diode_Volts();
void Change_Cables_Cooler_V_DC();
void Change_Cables_Cooler_mA();


void Change_Cables_Diode_Volts()
{
    printf(COLOR_LIGHT_MAGENTA "\n\n\t## Diode Voltage CAL:  read diode V w/ HP_A and PI_DAQ  ##\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_BOLD_RED "\tE3648A Output 1" COLOR_RESET " --> " COLOR_BOLD_RED " Cal Box J17/J20\n" COLOR_RESET);
    printf("\t\tConnect Cal Box" COLOR_RED "\tDB9 cable" COLOR_RESET " --> " COLOR_RED " PI DB9 Header\n" COLOR_RESET);
    Menu::Wait_Enter();
}

void Change_Cables_Cooler_V_DC()
{
    printf(COLOR_LIGHT_MAGENTA "\n\n\t## Cooler Voltage CAL:  read Cool_V ##\n\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_BOLD_RED "\tE3648A Output 1" COLOR_RESET " --> " COLOR_BOLD_RED " Cal Box J24/J25\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_BOLD_RED "\tPI_DAQ" COLOR_RESET " --> " COLOR_BOLD_RED " Cal Box J24/J25\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_BOLD_RED "\tHP_B" COLOR_RESET " --> " COLOR_BOLD_RED " Cal Box J24/J25\n" COLOR_RESET);
    Menu::Wait_Enter();
}

void Change_Cables_Cooler_mA()
{
    printf(COLOR_LIGHT_MAGENTA "\n\n\t## Cooler AC Current CAL:  read Cool_mA ##\n\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_RED "\tYokogawa" COLOR_RESET " --> " COLOR_RED " Cal Box J3/J6\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_RED "\tFL100" COLOR_RESET " --> " COLOR_RED " Cal Box J5/J7\n" COLOR_RESET);
    Menu::Wait_Enter();
}


// This function calibrates these mappings:
//      PI_DAQ count -> actual Load mA out  (as measured by HP34401B)
//      PI measured load mA out -> actual Load mA out  (as measured by HP34401B)
//      PI measured load V out -> actual Load V out  (as measured by HP34401A)
//
// First the PI device under calibration is put into Calibration mode with the "CAL ON" command
// PI ADC loop is set to measure the load current and load voltage with the "LOOP1" command
// Relay to power the cooler is turned OFF (since we are only calibrating the load resistor current and voltage)
// HP34401B meter is configured to measure current
// HP34401A is configured to measure voltage
// in a loop, DAC is incremented, PI measures voltage and current, HP meters measure voltage and current
// the measured values are aggregated, written to file, and then continue to next DAC increment value
// When complete DAC is set to zero (load current source turned off)
void Auto_Calibrate_Load_Increments(PI* pi, Meter* HP34401A, Meter* HP34401B, int DAC_INCREMENT, Write_Log* FileWrite)
{
    char pi_string[5000];

    printf("\n\n");
        // ## DC Calibrate Load Test ##
    printf(COLOR_LIGHT_MAGENTA "\n\n\t##  LOAD CAL ...set DAC and calibrate current source ##\n" COLOR_RESET);
    printf("\t\tConnect DB9 header " COLOR_RED "\tcal box" COLOR_RESET " --> " COLOR_RED " PI DAQ\n\n" COLOR_RESET);

    int sock_addr = pi->Get_Socket_Address();
    pi->Cal_Mode_ON();
    pi->Set_Read_Seq_Load_ON( );
    pi->Relay_OFF();

    Set_Relays_Cal_Load();
    HP34401_Display_Text(HP34401B, "LOAD I");
    Configure_Current_DC(HP34401B);
    Setup_Trigger_10_samples(HP34401B);

    HP34401_Display_Text(HP34401A, "LOAD V");
    Configure_Volts_DC(HP34401A);
    Setup_Trigger_10_samples(HP34401A);



    PI_Data_Point** Pi_Data;
    int DAC_count = 0;
    while( DAC_count < 64881 )
    {
        pi->Set_DAC_Count(DAC_count);
        usleep(ONE_MILLISECOND*1000);
        StringH::Erase_Num_Chars(pi_string, 3000);
        pi->PI_Send("READ_ADC 10");
        usleep(ONE_MILLISECOND*5000);
        Data_Group* pi_load_mA;
        Data_Group* pi_load_V;

        pi->PI_Wait_Receive();

        Pi_Data    = pi->Parse_PI_Data( );
        pi_load_mA = pi->Extract_Load_mA_Data( Pi_Data );
        pi_load_V  = pi->Extract_Load_V_Data( Pi_Data );

        pi_load_mA = Data::Discard_Outlier_Points( pi_load_mA );
        pi_load_V  = Data::Discard_Outlier_Points( pi_load_V );

        Data_Group* HP_load_mA = Read_Load_milliAmps(HP34401B);     // in HP_Measurement_Functions.h
        Data_Group* HP_load_V = Read_V_Load(HP34401A);              // in HP_Measurement_Functions.h

        Data::Print_DataGroup_Compare(pi_load_mA, HP_load_mA);
        Data::Print_DataGroup_Compare(pi_load_V, HP_load_V);

        FileWrite->Write_Load_mA_Data(DAC_count, pi_load_mA, HP_load_mA);
        FileWrite->Write_Volt_Data(pi_load_V, HP_load_V, logfile_t::Load_V);

        // TODO: write current ambient temperature to file as CAL data

        Data::Free_Data_Group_Memory(pi_load_mA);
        Data::Free_Data_Group_Memory(pi_load_V);
        Data::Free_Data_Group_Memory(HP_load_mA);
        Data::Free_Data_Group_Memory(HP_load_V);

        printf("------------------------------------------------------------------\n");

        DAC_count += DAC_INCREMENT;
    }
    pi->Set_DAC_Count(0);
    printf("------------------------------------------------------------------\n");

    pi->Relay_OFF( );
    pi->Set_Read_Seq_Load_OFF( );
    pi->Cal_Mode_OFF();

    usleep(ONE_MILLISECOND);
    Reset_All_Relays(false);
    Menu::Clear_Console_Screen();
}



void Auto_Calibrate_Load_Increments_Reverse(PI* pi, Meter* HP34401A, Meter* HP34401B, int DAC_INCREMENT, Write_Log* FileWrite)
{
    char pi_string[5000];

    printf("\n\n");
        // ## DC Calibrate Load Test ##
    printf(COLOR_LIGHT_MAGENTA "\n\n\t##  LOAD CAL ...set DAC and calibrate current source ##\n" COLOR_RESET);
    printf("\t\tConnect DB9 header " COLOR_RED "\tcal box" COLOR_RESET " --> " COLOR_RED " PI DAQ\n\n" COLOR_RESET);

    int sock_addr = pi->Get_Socket_Address();
    pi->Cal_Mode_ON();
    pi->Set_Read_Seq_Load_ON( );
    pi->Relay_OFF();

    Set_Relays_Cal_Load();
    HP34401_Display_Text(HP34401B, "LOAD I");
    Configure_Current_DC(HP34401B);
    Setup_Trigger_10_samples(HP34401B);

    HP34401_Display_Text(HP34401A, "LOAD V");
    Configure_Volts_DC(HP34401A);
    Setup_Trigger_10_samples(HP34401A);

    PI_Data_Point** Pi_Data;
    int DAC_count = 65536;
    while( DAC_count > 500 )
    {
        pi->Set_DAC_Count(DAC_count);
        usleep(ONE_MILLISECOND*1000);
        StringH::Erase_Num_Chars(pi_string, 3000);
        pi->PI_Send("READ_ADC 10");
        usleep(ONE_MILLISECOND*5000);
        Data_Group* pi_load_mA;
        Data_Group* pi_load_V;

        pi->PI_Wait_Receive();

        Pi_Data    = pi->Parse_PI_Data( );
        pi_load_mA = pi->Extract_Load_mA_Data( Pi_Data );
        pi_load_V  = pi->Extract_Load_V_Data( Pi_Data );

        pi_load_mA = Data::Discard_Outlier_Points( pi_load_mA );
        pi_load_V  = Data::Discard_Outlier_Points( pi_load_V );

        Data_Group* HP_load_mA = Read_Load_milliAmps(HP34401B);     // in HP_Measurement_Functions.h
        Data_Group* HP_load_V = Read_V_Load(HP34401A);              // in HP_Measurement_Functions.h

        Data::Print_DataGroup_Compare(pi_load_mA, HP_load_mA);
        Data::Print_DataGroup_Compare(pi_load_V, HP_load_V);

        FileWrite->Write_Load_mA_Data(DAC_count, pi_load_mA, HP_load_mA);
        FileWrite->Write_Volt_Data(pi_load_V, HP_load_V, logfile_t::Load_V);

        // TODO: write current ambient temperature to file as CAL data

        Data::Free_Data_Group_Memory(pi_load_mA);
        Data::Free_Data_Group_Memory(pi_load_V);
        Data::Free_Data_Group_Memory(HP_load_mA);
        Data::Free_Data_Group_Memory(HP_load_V);

        printf("------------------------------------------------------------------\n");

        DAC_count -= DAC_INCREMENT;
    }
    pi->Set_DAC_Count(0);
    printf("------------------------------------------------------------------\n");

    pi->Relay_OFF( );
    pi->Set_Read_Seq_Load_OFF( );
    pi->Cal_Mode_OFF();

    usleep(ONE_MILLISECOND);
    Reset_All_Relays(false);
    Menu::Clear_Console_Screen();
}





// This function calibrates the PI measured cooler DC volts, to actual cooler volts (as measured by HP34401_B)
// The voltage is calibrated at many specific DC volt values, specified by array DCV_Set_Points[], which is in "Cal_Points.h"
//
// First the PI device is put into Calibration mode with the "CAL ON" command
// PI cooler relay is turned OFF (so an external voltage source can be applied to the circuit)
// relay box is set to pass the cooler voltage to meter HP34401_B
// HP34401B is configured to measure voltage
// in a loop,E3648 programmable power supply is set to a calibration point voltage,
// PI measures the voltage, HP34401_B meter measures the voltage as well
// the measured values are written to file
// continue to next volt calibration point
void Auto_Calibrate_Cooler_V_DC(PI* pi, Meter* HP34401A, Meter* HP34401B, PowerSupply* E3648A, Write_Log* FileWrite)
{

    printf(COLOR_LIGHT_MAGENTA "\n\n\t## Cooler Voltage CAL:  read Cool_V ##\n\n" COLOR_RESET);
    printf(COLOR_BOLD_RED "\tIMPORTANT:\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_RED "\tE3648A Output 1" COLOR_RESET " --> " COLOR_RED " Cal Box J24/J25\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_RED "\tPI_DAQ" COLOR_RESET " --> " COLOR_RED " Cal Box J24/J25\n" COLOR_RESET);
    printf("\t\tConnect banana jumpers " COLOR_RED "\tHP_B" COLOR_RESET " --> " COLOR_RED " Cal Box J24/J25\n" COLOR_RESET);

    pi->Cal_Mode_ON();           // "CAL ON"
    pi->Relay_OFF();            // "CAL_RELAY_OFF"

    Set_Relays_Cal_Cooler_VDC();

    HP34401_Display_Text(HP34401B, "COOL V");
    HP34401_Display_Text(HP34401A, "-------");
    Configure_Volts_DC(HP34401B);                       // defined in "HP_34401.h"
    Setup_Trigger_10_samples(HP34401B);                 // defined in "HP_34401.h"
    usleep(ONE_MILLISECOND);

    Set_Instrument1(E3648A);     // sets "Instrument1" as the active instrument to receive commands
    Set_Volts(E3648A, 0.00);     // start with output voltage of 0
    Set_Output_ON(E3648A);
    Set_Current(E3648A, 0.025);   // set current limit to only 25mA because we arent driving anything

    pi->PI_Receive();   // grab any text sitting on the socket to prevent hangs

    PI_Data_Point** Pi_Data;
    Data_Group* pi_cooler_V;
    Data_Group* HP_cool_V;
    printf("\nSTARTING DC VOLT CAL LOOP....\n");
    for( int i=0; i< V_POINTS_NUM_ELEMENTS; ++i)
    {
        double DCV_set_point = DCV_Set_Points[i];   // get current loop setpoint. DCV_Set_Points[] is defined in Cal_Points.h

        // prompt user to set power supply to desired value
        //sprintf( user_message, "\n\tE3648A power supply set to " COLOR_BRIGHT_MAGENTA "%.3f " COLOR_RESET "Volts ...", DCV_set_point );
        Set_Volts(E3648A, DCV_set_point);
        printf("\n\tE3648A power supply set to " COLOR_BRIGHT_MAGENTA "%.3f " COLOR_RESET "Volts ...\n", DCV_set_point );
        //Menu::Prompt_User_Message(user_message);

        pi->PI_Send("READ_ADC 10");  // command for pi to take 10 ADC readings, and return all values as one string
        usleep(ONE_MILLISECOND*2000);
        pi->PI_Wait_Receive( );

        Pi_Data     = pi->Parse_PI_Data();                    // parse the pi_string of 10 data points into discrete metrics (Cooler Volts, Cooler mA, etc.)
        pi_cooler_V = PI::Extract_Cooler_V_Data( Pi_Data );   // extract only the Cooler voltage from the data set
        pi_cooler_V = Data::Discard_Outlier_Points( pi_cooler_V );  // throwout data points more than 1 standard deviation away from the average
        //PI::Free_PI_Data_Memory(Pi_Data);

        HP_cool_V   = HP34401_Average_10_DC_V_Samples(HP34401B); // measure actual cooler voltage with HP34401_B
        strcpy(HP_cool_V ->description, "HP Cool");

        Data::Print_DataGroup_Compare(pi_cooler_V, HP_cool_V);

        // Record Data
        FileWrite->Write_Volt_Data(pi_cooler_V, HP_cool_V, logfile_t::Cooler_V);

        Data::Free_Data_Group_Memory(HP_cool_V);
        Data::Free_Data_Group_Memory(pi_cooler_V);
        printf("------------------------------------------------------------------\n");
    }
    /*
        INSTRUCT USER TO TURN PS OFF
    */
    printf(COLOR_BRIGHT_MAGENTA "DCV CAL LOOP COMPLETE.\nTURNING AUXILIARY PS OFF\n" COLOR_RESET);
    Set_Volts(E3648A, 0.00);     // set output voltage of power supply to 0 
    Set_Output_OFF(E3648A);     // turn output of power supply off

    pi->Relay_OFF();
    pi->Set_Read_Seq_Load_OFF();
    pi->Cal_Mode_OFF();
    Reset_All_Relays(false);
}


void Auto_AC_Power_Calibration(PI* pi, WT300E* yokogawa, Write_Log* FileWrite)
{
    bool cooled_down = false;
    bool cooler_power_on = false;
    time_t start, loop_start, end, shutdown_timer;
    int seconds_since_logging = 0;
    int secs_since_reached_setpoint = 0;

    //Menu::Wait_Enter();

    if( !yokogawa->Is_Connected() )
    {
        printf("\n\nYokogawa meter is not detected, cannot continue with AC Calibration\n\n");
        Menu::Wait_Enter();
        Menu::Clear_Console_Screen();
        return;
    }

    pi->Cal_Mode_ON();
    pi->Relay_OFF();
    pi->Set_Read_Seq_POWER_CALIBRATION();
    printf("\n\n");

    pi->Relay_ON();
    time(&start);
    PI_Data_Point** Pi_Cool_Pwr_Data;

    // char* filename = LUT_Metric::Initialize_Volt_Write_File();
    //LUT* COOL_DC_A_LUT = new LUT( COOLER_AC_CURRENT );
    //BK5335_metric_t* BK5335_Data = new BK5335_metric_t();

    AC_Data_Group* Yokogawa_AC_Data = new AC_Data_Group("Yokogawa AC Current", "mA");
    printf(COLOR_LIGHT_MAGENTA "\t##  COOLER AC CURRENT CAL  ##\n\n" COLOR_RESET);
    printf("\n\twaiting to turn on cooler power\n\t");

    bool seen_mA_above_450 = false;
    bool seen_mA_below_100 = false;

    while( !cooled_down )
    {
      pi->PI_Send("READ_ADC 10"); // instruct pi to capture 10 consecutive samples
      double elapsed_secs = 0;
      time(&loop_start);
      while( elapsed_secs < 10 )    // wait seconds for pi to finish taking data
      {
          Yoko_sample_t* new_sample = yokogawa->Measure_RMS_Current();
          Yokogawa_AC_Data->Add_Sample( new_sample );

          time(&end);
          elapsed_secs = (int)difftime(end, loop_start);

          if(!cooler_power_on)
          {
              printf("... ");
              usleep(ONE_MILLISECOND);
              seconds_since_logging = (int)difftime(end, start);
              if(seconds_since_logging > 1)
              {
                  Set_Relays_Cal_Cooler_AC_Current();       // turns on cooler
                  cooler_power_on = true;
              }
          }
      }

      // Receive the Data Samples from the PI
      pi->PI_Wait_Receive( );
      PI_Data_Point** Pi_Cooler_RMS_Data = pi->Parse_PI_Data();         // contains sequential samples as timestamped data
      AC_Data_Group* pi_AC_mA = PI::Extract_AC_mA_Data( Pi_Cooler_RMS_Data );
      float avg_pi_mA = AC_Data_Group::Get_AC_Data_Average(pi_AC_mA);
      int samples_below_100mA = AC_Data_Group::Get_Num_Samples_Below_100(pi_AC_mA);
      int samples_above_450mA = AC_Data_Group::Get_Num_Samples_Above_450(pi_AC_mA);

      //AC_Data_Group::Print_AC_Data( pi_AC_mA );
      //AC_Data_Group::Print_AC_Data( Yokogawa_AC_Data );
      LUT_Data*  matched_samples = new LUT_Data("PI vs Yokogawa- Cooler mA");
      matched_samples->Match_Time_Samples(pi_AC_mA , Yokogawa_AC_Data);
      LUT_Data::Print_Matches(matched_samples);
      FileWrite->Write_Power_Meter_mA_Data(matched_samples->data_matches, matched_samples->num_points);

      if(seen_mA_below_100)
      {
        pi->Relay_OFF();
        pi->Set_Read_Seq_Load_OFF();
        Reset_All_Relays(false);
        return;
      }

      if( seen_mA_above_450 && (avg_pi_mA < 100 || samples_below_100mA > 2))
      {
          seen_mA_below_100 = true;
          printf("pi avg mA: %f\n", avg_pi_mA);
          printf("samples below 100mA: %d\n", samples_below_100mA);
      }
      if( !seen_mA_above_450 && (avg_pi_mA > 450 || samples_above_450mA > 3))
      {
          seen_mA_above_450 = true;
          printf("pi avg mA: %f\n", avg_pi_mA);
          printf("samples above 450mA: %d\n", samples_above_450mA);
      }

      Yokogawa_AC_Data->Clear_All_Data();
    }
    // take out of cal mode
    // free any memory
}




// This function calibrates the PI measured diode volts, to actual cooler volts (as measured by HP34401_B)
// The voltage is calibrated at many specific DC volt values, specified by array DCV_Set_Points[], which is in "Cal_Points.h"
//
// First the PI device is put into Calibration mode with the "CAL ON" command
// PI cooler relay is turned OFF (so an external voltage source can be applied to the circuit)
// relay box is set to pass the cooler voltage to meter HP34401_B
// HP34401B is configured to measure voltage
// in a loop,E3648 programmable power supply is set to a calibration point voltage,
// PI measures the voltage, HP34401_B meter measures the voltage as well
// the measured values are written to file
// continue to next volt calibration point
void Auto_Calibrate_Diode_Volts(PI* pi, Meter* HP34401A, PowerSupply* E3648A, Write_Log* FileWrite)
{
    printf(COLOR_LIGHT_MAGENTA "\n\n\t## Diode Voltage CAL:  read diode V w/ HP_A and PI_DAQ  ##\n" COLOR_RESET);

    // Menu::Wait_Enter();
    pi->Cal_Mode_ON();              // "CAL ON"
    pi->Relay_OFF();                // "CAL_RELAY_OFF"
    pi->Set_Read_Seq_Load_OFF();   //   "LOOP2" 

    Set_Relays_Cal_Diode_Volts();
    HP34401_Display_Text(HP34401A, "DIODE CAL");
    Configure_Volts_DC(HP34401A);                       // defined in "HP_34401.h"
    Setup_Trigger_10_samples(HP34401A);                 // defined in "HP_34401.h"
    usleep(ONE_MILLISECOND);

    Set_Instrument1(E3648A);     // sets "Instrument1" as the active instrument to receive commands
    Set_Volts(E3648A, 0.00);     // start with output voltage of 0
    Set_Output_ON(E3648A);
    Set_Current(E3648A, 0.025);   // set current limit to only 25mA because we arent driving anything

    pi->PI_Receive();   // grab any text sitting on the socket to prevent hangs

    PI_Data_Point** Pi_Data;
    Data_Group* pi_diode_V;
    Data_Group* HP_diode_V;
    printf("\nSTARTING DIODE VOLT CAL LOOP....\n");
    for( int i=0; i< DIODE_POINTS_NUM_ELEMENTS; ++i)
    {
        double diode_V_set_point = Diode_Cal_Points[i];   // get current loop setpoint. DCV_Set_Points[] is defined in Cal_Points.h

        Set_Volts(E3648A, diode_V_set_point);
        printf("\n\tE3648A power supply set to " COLOR_BRIGHT_MAGENTA "%.3f " COLOR_RESET "Volts ...\n", diode_V_set_point );

        pi->PI_Send("READ_ADC 10");  // command for pi to take 10 ADC readings, and return all values as one string
        usleep(ONE_MILLISECOND*2000);
        pi->PI_Wait_Receive();

        Pi_Data    = pi->Parse_PI_Data();                    // parse the pi_string of 10 data points into discrete metrics (Cooler Volts, Cooler mA, etc.)
        pi_diode_V = PI::Extract_Diode_V_Data( Pi_Data );   // extract only the Cooler voltage from the data set
        pi_diode_V = Data::Discard_Outlier_Points( pi_diode_V );  // throwout data points more than 1 standard deviation away from the average
        //PI::Free_PI_Data_Memory(Pi_Data);

        HP_diode_V   = HP34401_Average_10_DC_V_Samples(HP34401A); // measure actual cooler voltage with HP34401_B
        strcpy(HP_diode_V ->description, "HP Diode");

        Data::Print_DataGroup_Compare(pi_diode_V, HP_diode_V);

        // Record Data
        FileWrite->Write_Volt_Data(pi_diode_V, HP_diode_V, logfile_t::Diode_V);

        Data::Free_Data_Group_Memory(HP_diode_V);
        Data::Free_Data_Group_Memory(pi_diode_V);
        printf("------------------------------------------------------------------\n");
    }
    /*
        INSTRUCT USER TO TURN PS OFF
    */
    printf(COLOR_BRIGHT_MAGENTA "DIODE V CAL LOOP COMPLETE.\nTURNING AUXILIARY PS OFF\n" COLOR_RESET);
    Set_Volts(E3648A, 0.00);     // set output voltage of power supply to 0 
    Set_Output_OFF(E3648A);     // turn output of power supply off

    pi->Cal_Mode_OFF();
    Reset_All_Relays(false);
}


#ifdef __cplusplus
}
#endif

#endif
