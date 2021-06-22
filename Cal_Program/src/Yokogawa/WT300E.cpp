// BK_5535.h - Support for BK5535 power meter in Linux device tree
// ==========================================================================================
//
// uses the Linux /dev/usbtmc interface

#include "WT300E.h"

#define DEBUG_YOKOGAWA false

using namespace std;

#define ONE_MILLISECOND 1000
#define PRINT_MEASUREMENT       false
#define Yoko_SEARCH_COMMAND   "ls /dev | grep usbtmc"

// Open a power meter in /dev/usbtmc[dev_num]
// allocate memory for data storage
WT300E::WT300E( )
{
  lasterr = RDS_ERR_OK;
  connected = false;
  FD = -1;
  io_count = 0;
  memset(message, 0, 1024 );        // clear the buffers
  memset( write_buffer, 0, 2048 );

  char* usbtmc_available = WT300E::Search_USBTMC();
  if(usbtmc_available == NULL)
  {
      printf(COLOR_BOLD_RED "WT300E NOT DETECTED IN /dev. LOOKING FOR /dev/usbtmc[x]\n" COLOR_RESET);
      lasterr = RDS_ERR_CANTOPEN;
      connected = false;
      printf("Do not calibrate AC power ... no power meter connected\n");
      //exit(0);
      return;
  }
  int device_number = StringH::parse_USBTMC_Number(usbtmc_available);

  StringH::Erase_Num_Chars(this->device_path, 20);
  sprintf( this->device_path, "/dev/usbtmc%d", device_number );

  printf("\nopening YOKOGAWA meter at: " COLOR_MAGENTA "%s\n" COLOR_RESET, this->device_path);
  FD = open( this->device_path, O_RDWR); // make sure to open in non blocking mode
  usleep(10000);
  if( FD < 0 )
  {
    printf("  Cant't open the damn YOKO at:\t%s\n", write_buffer);
    printf(COLOR_BOLD_RED "\t ?? DID YOU REMEMBER TO USE SUDO ??\n" COLOR_RESET);
    printf("  Note: occasionally the YOKOGAWA gets stuck in an error state and needs to be power cycled\n");
    printf("  If sudo was used and it still won't connect, try power cycling the YOKOGAWA\n");
    lasterr = RDS_ERR_CANTOPEN;
    connected = false;
    printf("Calibration cannot continue. exiting...\n");
    exit(0);
    return;
  }
  else
    connected = true;

  this->Set_Remote();

  if( this->Get_Num_Chars_Waiting(FD) > 0)  // if there are characters on the waiting to be read, flush them out
  {
    this->Read();
  }
    
    // the following are objects that contain contain commands to query the the Yokogawa power meter
    this->volt_measurement = new WT300_Measurement(ITEM_VOLTAGE, WT300E_VOLTAGE);
    this->current_measurement = new WT300_Measurement(ITEM_CURRENT, WT300E_CURRENT);
    this->active_power_measurement = new WT300_Measurement(ITEM_ACTIVE_POWER, WT300E_ACTIVE_POWER);
    this->apparent_power_measurement = new WT300_Measurement(ITEM_APPARENT_POWER, WT300E_APPARENT_POWER);
    this->reactive_power_measurement = new WT300_Measurement(ITEM_REACTIVE_POWER, WT300E_REACTIVE_POWER);
    this->power_factor_measurement = new WT300_Measurement(ITEM_POWER_FACTOR, WT300E_POWER_FACTOR);
}

void WT300E::Initialize_Meter()
{
    char* id_string = this->Get_ID();
    this->Parse_ID_String( id_string );

    Clear();
    Set_ASCII_Output();
    Set_Verbose_OFF();
    Set_Overlap_Commands_OFF();
    Set_Headers_OFF();
    Set_Input_Mode_RMS();
    Set_Line_Filter_ON();
    Set_Volt_Range_Auto();
    //Set_Current_Range_2A();
    Set_Current_Range_1A();
    Set_Resolution_5_Digits();
    Set_Averaging_OFF();
    Set_Averaging_Linear();
    Set_Averaging_Count_8();
    Set_Sample_Rate_100_ms();
    Set_Commands_WT300E();
    this->Print_Error_Queue();
    usleep(ONE_MILLISECOND*500);
    printf("\n\n");
}

// Clean up.
WT300E::~WT300E()
{
  if( FD >= 0 )
    close( FD );

  FD = -1;
}

void WT300E::Parse_ID_String(char* id_string)
{
    char manufacturer[20];
    char model[20];
    char serial_number[20];
    char firmware_version[30];
    StringH::Erase_Num_Chars(manufacturer, 20);
    StringH::Erase_Num_Chars(model, 20);
    StringH::Erase_Num_Chars(serial_number, 20);
    StringH::Erase_Num_Chars(firmware_version, 30);

    char delims[] = ",";    // ID string is delimited by comma
    char* token = strtok(id_string, delims); // parse the response string, delimiter is comma

    if( token != NULL && strlen(token) > 0)
    {
        strcpy(manufacturer, token);    // first token is the manufacturer
        token = strtok(NULL, delims);       // get next token
        strcpy(model, token);       // second token is the model
        token = strtok(NULL, delims);       // get next token, should be a single digit and a comma
        strcpy(serial_number, token);   // add the read character to the ID string
        token = strtok(NULL, delims);   // get last token
    }
    else
        printf("Could not read response from Yokogawa.\n");
    free(id_string);

    printf(COLOR_BOLD_MAGENTA "%s\n" COLOR_RESET, this->device_path);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, manufacturer);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, model);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, serial_number);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, firmware_version);
}

bool WT300E::Is_Connected()
{
    if( this->FD >= 0)
        return true;
    else 
        return false;
}

// searches the /dev/ device tree location for devices with name USBTMC*
// returns the first response
char* WT300E::Search_USBTMC()
{
    char search_command[] = Yoko_SEARCH_COMMAND;
    char** response = Yoko_metric_t::Exec_Shell_Command(search_command);
    if( response[0] == NULL )
        return NULL;
    usleep(ONE_MILLISECOND*1000);

    char USBTMC_device[10];
    strcpy(USBTMC_device, response[0]);
    StringH::Free_String_Array_Memory(response);
    response = NULL;

    if(USBTMC_device != NULL && strlen(USBTMC_device)>1)
    {
        return strdup(USBTMC_device);
    }
    else return NULL;
}

// Send text string to query the meter and reads the response
// inserts time delay to prevent from reading bad values from the meter
// returns NULL on error
char* WT300E::Send( const char* text_query )
{
  StringH::Erase_String(this->write_buffer);
  strncpy( this->write_buffer, text_query, 2000 ); // copy command to write buffer, up to string length of command string
  if( connected )
  {
      usleep(1000);
      //printf("writing: %s\n", this->write_buffer);
      if( ( write( FD, this->write_buffer, strlen(this->write_buffer) ) )<0   )
      {
          printf("write error\n");
          lasterr = RDS_ERR_CANTWRITE;
          return NULL;
      }
      usleep( TIME_DELAY );
      usleep(18000);
      StringH::Erase_String(this->write_buffer);
      io_count = read( this->FD, this->write_buffer, 500 );
      // printf("read: %s\n", this->write_buffer);
      if( io_count < 0 )
      {
          printf("didnt read anything for FD: %d\n", this->FD);
	       lasterr = RDS_ERR_CANTREAD;
	        return NULL;
      }
      else
      {
	       this->write_buffer[io_count] = '\0';
	       return this->write_buffer;
      }
  }
  else
  {
    lasterr = RDS_ERR_NOTOPEN;
    return NULL;
  }
}

// Send text string to query the meter and reads the response
// inserts time delay to prevent from reading bad values from the meter
// returns NULL on error
bool WT300E::Write_No_Reply( const char* text_query )
{
  StringH::Erase_String(this->write_buffer);
  strncpy( write_buffer, text_query, 500 ); // copy command to write buffer, up to string length of command string
  if( connected )
  {
      usleep(1000);
      if( ( write( FD, write_buffer, strlen(write_buffer) ) ) < 0  )  // for the write always make sure to use strlen
      {
          lasterr = RDS_ERR_CANTWRITE;
          return false;
      }
      usleep( TIME_DELAY );
      return true;
  }
  else
  {
    lasterr = RDS_ERR_NOTOPEN;
    return false;
  }
}


int WT300E::Get_Num_Chars_Waiting(int port_FD)
{
    int bytes_ready = 0;

    ioctl(port_FD, FIONREAD, &bytes_ready);
    if( DEBUG_YOKOGAWA )
        printf("%d bytes in input buffer\n", bytes_ready);

    return bytes_ready;
}

char* WT300E::Read( )
{
    StringH::Erase_String(this->write_buffer);
    io_count = read( this->FD, this->write_buffer, 500 );
    //printf("read: %s\n", this->write_buffer);
    if( io_count < 0 )
    {
        printf("didnt read anything for FD: %d\n", this->FD);
        lasterr = RDS_ERR_CANTREAD;
        return NULL;
    }
    else
    {
        this->write_buffer[io_count] = '\0';
        return strdup(this->write_buffer);
    }
}


// must free memory after this command
char* WT300E::Send_Cmd(const char* cmd_string)
{
      //printf("SEND_CMD\n");
	    const char* reply = this->Send( cmd_string );
	    if( reply == NULL )
      {
	         return NULL;
	    }
	    else
      {
	         fflush( stdout );
           return strdup(reply);
	    }
}

void WT300E::Set_Remote()
{
    StringH::Erase_String(command);
    strcpy(command, ":COMMunicate:REMote 1");
    Write_No_Reply( command );
}

char* WT300E::Get_ID()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, "*IDN?");

    //printf("%s\n", command);
    Write_No_Reply( command );

    usleep(15000);  // wait 45 milliseconds
    char* result = WT300E::Read();

    return result;
}



// takes at least a minute to run this thing
void WT300E::Zero_Cal()
{
    StringH::Erase_String(command);
    strcpy(command, "*CAL?");
    WT300E::Write_No_Reply(command);
    sleep(30);
    printf("  ... cal done\n");

}

void WT300E::WaitForAcqComplete()
{
    string cmd1 = ":STATus:FILTer1 FALL;"; // sets filter so bit 0 in extended event register goes to 1 when updating of measured data completes
    string cmd2 = ":STATus:EESE 1;";       // only change the status byte based on bit 0 in the extended event register
    string cmd3 = ":STATus:EESR?;";        // clears extended event register
    string cmd4 = "*SRE 8";                // generates service requests based only on extended event register bits

    string command = cmd1 + cmd2 + cmd3 + cmd4 + "\n";
    char* status = WT300E::Send_Cmd( command.c_str() );
    free(status);
    usleep(100);
}


void WT300E::Set_ASCII_Output()
{
    string command = ":NUMeric:FORMat ASCII\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
}

void WT300E::Set_Verbose_OFF()
{
    string command = ":COMMunicate:VERBose 0\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    printf("\t--verbose mode OFF\n");
    usleep(100);
}

void WT300E::Set_Overlap_Commands_OFF()
{
    string command = ":COMMunicate:OVERlap 0\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
}

void WT300E::Set_Headers_OFF()
{
    string command = ":COMMunicate:HEADer 0\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    printf("\t--headers OFF\n");
    usleep(100);
}

void WT300E::Set_Input_Mode_RMS()
{
    string command = ":INP:MODE RMS\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    command = ":INP:MODE?";
    usleep(100);
    char* result = Send_Cmd( command.c_str() );
    printf("\t--input mode:  %s", result);
    free(result);
    usleep(100);
}

void WT300E::Set_Line_Filter_ON()
{
    string command = ":INP:FILT:LINE 1\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    printf("\t--line filter ON\n");
    usleep(100);
}

void WT300E::Set_Volt_Range_Auto()
{
    string command = ":INP:VOLT:AUTO 1\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
    command = ":INP:VOLT:RANGE?";
    char* result = Send_Cmd( command.c_str() );
    double range = StringH::Parse_Double(result);
    printf("\t--input volt range:  %.2f V\n", range);
    free(result);
    usleep(100);
}

void WT300E::Set_Current_Range_2A()
{
    string command = ":INP:CURR:RANGE 2A\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);

    command = ":INP:CURR:RANGE?";
    char* result = Send_Cmd( command.c_str() );
    double range = StringH::Parse_Double(result);
    printf("\t--input current range:  %.2f A\n", range);
    free(result);
    usleep(100);
}

void WT300E::Set_Current_Range_1A()
{
    string command = ":INP:CURR:RANGE 1A\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);

    command = ":INP:CURR:RANGE?";
    char* result = Send_Cmd( command.c_str() );
    double range = StringH::Parse_Double(result);
    printf("\t--input current range:  %.2f A\n", range);
    free(result);
    usleep(100);
}

void WT300E::Set_Resolution_5_Digits()
{
    string command = ":SYSTEM:RESOLUTION 5\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
}

void WT300E::Set_Averaging_ON()
{
    string command = ":MEASURE:AVERAGING:STATE 1\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
        
    command = ":MEASURE:AVERAGING:STATE?";
    char* result = Send_Cmd( command.c_str() );
    printf("\t--averaging:  %s", result);
    free(result);
    usleep(100);
}

void WT300E::Set_Averaging_OFF()
{
    string command = ":MEASURE:AVERAGING:STATE 0\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
        
    command = ":MEASURE:AVERAGING:STATE?";
    char* result = Send_Cmd( command.c_str() );
    printf("\t--averaging:  %s", result);
    free(result);
    usleep(100);
}

void WT300E::Set_Averaging_Linear()
{
    string command = ":MEASURE:AVERAGING:TYPE LINEAR\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
}

void WT300E::Set_Averaging_Count_8()
{
    string command = ":MEASURE:AVERAGING:COUNT 8\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
}

void WT300E::Set_Sample_Rate_100_ms()
{
    string command = ":RATE 100 MS\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);

    command = ":RATE?";
    char* result = Send_Cmd( command.c_str() );
    double rate = StringH::Parse_Double(result);
    printf("\t--sample rate:  %.3f sec\n", rate);
    free(result);
    usleep(100);
}

void WT300E::Set_Sample_Rate_250_ms()
{
    string command = ":RATE 250 MS\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);

    command = ":RATE?";
    char* result = Send_Cmd( command.c_str() );
    double rate = StringH::Parse_Double(result);
    printf("\t--sample rate:  %.3f sec\n", rate);
    free(result);
    usleep(100);
}

void WT300E::Set_Commands_WT300E()
{
    string command = ":SYSTEM:COMMUNICATE:COMMAND WT300E\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
}


void WT300E::Print_Error_Queue()
{
    string command = ":STAT:ERR?\n";   // output from the meter is in ASCII format
    char* result = WT300E::Send_Cmd(command.c_str() );
    printf("\t Yokogawa Errors: %s\n", result);
    free(result);
    usleep(100);
}

void WT300E::Clear()
{
    string command = "*CLS\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    printf("\t--clear regs\n");
    usleep(100);
}

void WT300E::Reset_Meter()
{
    string command = "*RST\n";   // output from the meter is in ASCII format
    WT300E::Write_No_Reply(command.c_str() );
    usleep(100);
}

double WT300E::Get_Voltage()
{
    char* result = Send_Cmd( this->volt_measurement->query_value.c_str() );
    double volts = StringH::Parse_Double(result);
    //printf("volts parsed:   %.3f V\n", volts);
    free(result);
}

double WT300E::Get_Current()
{
    char* result = Send_Cmd( this->current_measurement->query_value.c_str() );
    double amps = StringH::Parse_Double(result);
    double milliAmps = amps*1000;
    //printf("current parsed: %.1f mA\n", milliAmps);
    free(result);

    Yoko_sample_t* new_sample = new Yoko_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = milliAmps;
    new_sample->measurement_type = RMS_CURRENT;
}

Yoko_sample_t* WT300E::Measure_RMS_Current()
{
    char* result = Send_Cmd( this->current_measurement->query_value.c_str() );
    double amps = StringH::Parse_Double(result);
    double milliAmps = amps*1000;
    //printf("current parsed: %.1f mA\n", milliAmps);
    free(result);

    Yoko_sample_t* new_sample = new Yoko_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = milliAmps;
    new_sample->measurement_type = RMS_CURRENT;
}

double WT300E::Get_Active_Power()
{
    char* result = Send_Cmd( this->active_power_measurement->query_value.c_str() );
    double watts = StringH::Parse_Double(result);
    watts = abs(watts);
    printf("active watts:   %.3f W\n", watts);
    free(result);
}

double WT300E::Get_Total_Power()
{
    char* result = Send_Cmd( this->apparent_power_measurement->query_value.c_str() );
    double watts = StringH::Parse_Double(result);
    printf("total watts:   %.3f W\n", watts);
    free(result);
}

double WT300E::Get_Reactive_Power()
{
    char* result = Send_Cmd( this->reactive_power_measurement->query_value.c_str() );
    double reactive_watts = StringH::Parse_Double(result);
    printf("reactive watts: %.3f W\n", reactive_watts);
    free(result);
}

double WT300E::Get_Power_Factor()
{
    char* result = Send_Cmd( this->power_factor_measurement->query_value.c_str() );
    double power_factor = StringH::Parse_Double(result);
    printf("power_factor: %.3f \n", power_factor);
    free(result);
}

void WT300E::Set_Item_Metric_Assignments()
{

    WT300E::Write_No_Reply(volt_measurement->set_command.c_str() );
    usleep(10);
    WT300E::Write_No_Reply(current_measurement->set_command.c_str() );
    usleep(10);
    WT300E::Write_No_Reply(active_power_measurement->set_command.c_str() );
    usleep(10);
    WT300E::Write_No_Reply(apparent_power_measurement->set_command.c_str() );
    usleep(10);
    WT300E::Write_No_Reply(reactive_power_measurement->set_command.c_str() );
    usleep(10);
    WT300E::Write_No_Reply(power_factor_measurement->set_command.c_str() );
    usleep(10);

    usleep(100);
}