// BK_5535.h - Support for BK5535 power meter in Linux device tree
// ==========================================================================================
//
// uses the Linux /dev/usbtmc interface

#include "BK5335.h"

#define PRINT_MEASUREMENT       false
#define BK5335_SEARCH_COMMAND   "ls /dev | grep usbtmc"

// Open a power meter in /dev/usbtmc[dev_num]
// allocate memory for data storage
BK5335::BK5335( )
{
  lasterr = RDS_ERR_OK;
  connected = false;
  FD = -1;
  io_count = 0;
  memset(message, 0, 1024 );        // clear the buffers
  memset( write_buffer, 0, 2048 );

  char* usbtmc_available = BK5335::Search_USBTMC();
  if(usbtmc_available == NULL)
  {
      printf(COLOR_BOLD_RED "BK5335 NOT DETECTED IN /dev. LOOKING FOR /dev/usbtmc[x]\n" COLOR_RESET);
      lasterr = RDS_ERR_CANTOPEN;
      connected = false;
      printf("Do not calibrate AC power ... no power meter connected\n");
      //exit(0);
      return;
  }
  int device_number = StringH::parse_USBTMC_Number(usbtmc_available);

  StringH::Erase_Num_Chars(this->device_path, 20);
  sprintf( this->device_path, "/dev/usbtmc%d", device_number );

  printf("\nopening BK5335 meter at: " COLOR_MAGENTA "%s\n" COLOR_RESET, this->device_path);
  FD = open( this->device_path, O_RDWR); // make sure to open in non blocking mode
  usleep(10000);
  if( FD < 0 )
  {
    printf("  Cant't open the damn BK5335 at:\t%s\n", write_buffer);
    printf(COLOR_BOLD_RED "\t ?? DID YOU REMEMBER TO USE SUDO ??\n" COLOR_RESET);
    printf("  Note: occasionally the BK5335 gets stuck in an error state and needs to be power cycled\n");
    printf("  If sudo was used and it still won't connect, try power cycling the BK5335\n");
    lasterr = RDS_ERR_CANTOPEN;
    connected = false;
    printf("Calibration cannot continue. exiting...\n");
    exit(0);
    return;
  }
  else
    connected = true;

}

void BK5335::Initialize_Meter()
{
    char manufacturer[20];
    char model[20];
    char serial_number[20];
    char firmware_version[30];
    StringH::Erase_Num_Chars(manufacturer, 20);
    StringH::Erase_Num_Chars(model, 20);
    StringH::Erase_Num_Chars(serial_number, 20);
    StringH::Erase_Num_Chars(firmware_version, 30);

    char* id_string = this->Get_ID();
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
        strcpy(firmware_version, token);   // add comma to ID string
        token = strtok(NULL, delims);   // get last token

    }
    else
        printf("Could not read response from HP meter.\n");
    free(id_string);

    printf(COLOR_BOLD_MAGENTA "%s\n" COLOR_RESET, this->device_path);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, manufacturer);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, model);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, serial_number);
    printf(COLOR_LIGHT_BLUE "\t%s\n" COLOR_RESET, firmware_version);

    this->Set_Continuous_Trigger();
    this->Turn_Beep_Off();
}

// Clean up.
BK5335::~BK5335()
{
  if( FD >= 0 )
    close( FD );

  FD = -1;
}

bool BK5335::Is_Connected()
{
    if( this->FD >= 0)
        return true;
    else 
        return false;
}

// searches the /dev/ device tree location for devices with name USBTMC*
// returns the first response
char* BK5335::Search_USBTMC()
{
    char search_command[] = BK5335_SEARCH_COMMAND;
    char** response = PI::Exec_Shell_Command(search_command);
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

//-----------------------------------------------
// Output a message via notify_out().
void BK5335::notify( const char* fmt, ... )
{
  va_list args;

  va_start( args, fmt );
  vsprintf( message, fmt, args );
  va_end( args );
  notify_out();
}

// Output a message string.
void BK5335::notify_out( void )
{
  fprintf( stderr, "%s", message );
}

// Return a description of the last error and reset last error.
// Return NULL if no error.
const char* BK5335::error_string( void )
{
  static const char* errstrings[] = {
    NULL,                                          // RDS_ERR_OK          0
    "Cannot open device /dev/usbtmc...",           // RDS_ERR_CANTOPEN    1
    "Cannot allocate memory.",                     // RDS_ERR_CANTMALLOC  2
    "Invalid command syntax.",                     // RDS_ERR_CMDSYNTAX   3
    "Cannot write to /dev/usbtmc...",              // RDS_ERR_CANTWRITE   4
    "/dev/usbtmc... was not opened successfully.", // RDS_ERR_NOTOPEN     5
    "Cannot read from /dev/usbtmc...",             // RDS_ERR_CANTREAD    6
    "Invalid query syntax."                        // RDS_ERR_QUERYSYNTAX 7
  };
  if( lasterr >=0 && lasterr < RDS_NUM_ERR ){
    int slasterr = lasterr;
    lasterr = RDS_ERR_OK;
    return( errstrings[slasterr] );
  }
  else
    return( "Unknown error." );
}

// Send text string to query the meter and reads the response
// inserts time delay to prevent from reading bad values from the meter
// returns NULL on error
char* BK5335::Send( const char* text_query )
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
bool BK5335::Write_No_Reply( const char* text_query )
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

char* BK5335::Read( )
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
char* BK5335::Send_Cmd(const char* cmd_string)
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

//    MEASUREMENT COMMANDS

//  AC VOLT MEASUREMENT
BK5335_sample_t* BK5335::Measure_AC_Volts()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:VOLT:AC?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = AC_VOLTS;

    return new_sample;
}

// measure RMS volts
BK5335_sample_t* BK5335::Measure_RMS_Volts()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:VOLT:RMS?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = RMS_VOLTS;

    return new_sample;
}

BK5335_sample_t* BK5335::Measure_DC_Volts()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:VOLT:DC?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = DC_VOLTS;

    return new_sample;
}

//  AC CURRENT MEASUREMENT FUNCTION
BK5335_sample_t* BK5335::Measure_AC_Current()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:CURR:AC?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = AC_CURRENT;

    return new_sample;
}

//  DC CURRENT MEASUREMENT FUNCTION
BK5335_sample_t* BK5335::Measure_DC_Current()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:CURR:DC?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = DC_CURRENT;

    return new_sample;
}

//  RMS CURRENT MEASUREMENT FUNCTION
BK5335_sample_t* BK5335::Measure_RMS_Current()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:CURR:RMS?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = RMS_CURRENT;


    return new_sample;
}

//  ACTIVE POWER MEASUREMENT
BK5335_sample_t* BK5335::Measure_Active_Power()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:POW:ACT?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp =new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = ACTIVE_POWER;

    return new_sample;
}


//  APPARENT POWER MEASUREMENT
BK5335_sample_t* BK5335::Measure_Apparent_Power()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:POW:APP?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = APPARENT_POWER;

    return new_sample;
}

BK5335_sample_t* BK5335::Measure_Reactive_Power()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:POW:REAC?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = REACTIVE_POWER;

    return new_sample;
}

BK5335_sample_t* BK5335::Measure_Power_Factor()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:POW:PFAC?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = POWER_FACTOR;

    return new_sample;
}

BK5335_sample_t* BK5335::Measure_Phase_Angle()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, ":MEAS:POW:PHAS?");

    char* result = Send_Cmd( command );
    double measured_val = StringH::Parse_Double(result);
    free(result);

    BK5335_sample_t* new_sample = new BK5335_sample_t();    // Assemble data value, measurement type, and timestamp into one object
    new_sample->timestamp = new Timestamp();
    new_sample->value = measured_val;
    new_sample->measurement_type = PHASE_ANGLE;;

    return new_sample;
}

char* BK5335::Get_ID()
{
    usleep(16500);
    StringH::Erase_String(command);
    strcpy(command, "*IDN?");

    //printf("%s\n", command);
    Write_No_Reply( command );

    usleep(45000);  // wait 45 milliseconds
    char* result = BK5335::Read();
    //if( result != NULL )
        //printf( "\t%s\n", result);

    return result;
}

// turn off that annoying ass beep when you push a button
void BK5335::Turn_Beep_Off()
{
      StringH::Erase_String(command);
      //printf("TURN BEEP OFF\n");
      strcpy(command, ":SYST:BEEP: OFF");
      printf("\t--setting BK5335 beep OFF ...\n");
      BK5335::Write_No_Reply( command );
}

// takes at least a minute to run this thing
void BK5335::Zero_Cal()
{
    StringH::Erase_String(command);
    strcpy(command, ":CAL:ZERO");
    BK5335::Write_No_Reply(command);
    sleep(30);
    printf("  ... cal done\n");

}

void BK5335::Set_Continuous_Trigger()
{
    StringH::Erase_String(command);
    strcpy(command, ":INIT:CONT: ON");    // set continuous trigger
    printf("\t--setting BK5335 continuous trigger ...\n");
    BK5335::Write_No_Reply(command);
    usleep(1000);

    StringH::Erase_String(command);
    strcpy(command, ":INIT:IMM");       // set immediate trigger
    printf("\t--setting BK5335 immediate trigger ...\n");
    BK5335::Write_No_Reply(command);
    return;
}
