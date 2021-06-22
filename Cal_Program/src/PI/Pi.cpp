


#include "Pi.h"


char receive_buffer[RECEIVE_BUFFER_SIZE];

void PI::Relay_ON( )
{
    if( !this->Is_Connected() )
            return;

    this->PI_Send("CAL_RELAY_ON");
    this->PI_Wait_Receive( );
    printf(COLOR_BROWN "%s\n" COLOR_RESET, receive_buffer);
}

void PI::Relay_OFF( )
{
    if( !this->Is_Connected() )
            return;

    this->PI_Send( "CAL_RELAY_OFF" );
    this->PI_Wait_Receive( );
    printf(COLOR_BROWN "%s\n" COLOR_RESET, receive_buffer);
}

void PI::Cal_Mode_ON( )
{
    if( !this->Is_Connected() )
            return;

     this->PI_Send("CAL ON");    // defined in client_socket.h
     this->PI_Wait_Receive( );
     //printf(COLOR_BROWN "\n%s\n" COLOR_RESET, receive_buffer);
}

void PI::Cal_Mode_OFF( )
{
    if( !this->Is_Connected() )
            return;

     this->PI_Send("CAL OFF");    // defined in client_socket.h
     this->PI_Wait_Receive( );
     //printf(COLOR_BROWN "\n%s\n" COLOR_RESET, receive_buffer);
}


void PI::Verbose_Mode_OFF( )
{
    if( !this->Is_Connected() )
            return;

     this->PI_Send("VERBOSE OFF");    // defined in client_socket.h
     this->PI_Wait_Receive( );
}


char* PI::Read_Metrics( )
{
    if( !this->Is_Connected() )
    {
        return NULL;
    }

     this->PI_Send("?metrics");    // defined in client_socket.h
     this->PI_Wait_Receive( );
     char* response =  strdup(receive_buffer);
     return response;
}


// read sequence optimized for taking as much data as possible for the 
// current and voltage to the CCC
void PI::Set_Read_Seq_POWER_CALIBRATION( )
{
    if( !this->Is_Connected() )
            return;

     this->PI_Send("POWER_CALIBRATION");
     this->PI_Wait_Receive( );
     printf(COLOR_BROWN "%s\n" COLOR_RESET, receive_buffer);
}

// Sets the ADC Read Sequence to Load OFF. This read sequence measures the normal parameters of CCC Volts and Current, 
// diode tempereature, thermocouples.... does not read load voltage or current
// because not reading load voltage or current, a few extra CCC current readings are taken
void PI::Set_Read_Seq_Load_OFF( )
{
    if( !this->Is_Connected() )
          return;

    this->PI_Send("LOOP2");		// Heat_Load_OFF Loop
    this->PI_Wait_Receive( );
    Test::Verify_Response_Contains( receive_buffer, "Read Metrics sequence has been set to HEAT_LOAD_OFF" );
    printf(COLOR_BROWN "%s\n" COLOR_RESET, receive_buffer);
    usleep(1000);
}

// Sets the ADC Read Sequence to Load ON. This read sequence measures the normal parameters of CCC Volts and Current, 
// diode tempereature, thermocouples.... and also measures the load current and voltage
void PI::Set_Read_Seq_Load_ON( )
{
    if( !this->Is_Connected() )
          return;

    this->PI_Send("LOOP1");		// Heat_Load_OFF Loop
    this->PI_Wait_Receive( );
    Test::Verify_Response_Contains( receive_buffer, "Read Metrics sequence has been set to HEAT_LOAD_ON" );
    StringH::Trim_WhiteSpace(receive_buffer);
    printf(COLOR_BROWN "%s\n" COLOR_RESET, receive_buffer);
    usleep(1000);
}


void PI::Set_DAC_Count(int DAC_count)
{
    if( !this->Is_Connected() )
          return;

    char pi_command[100];
    printf( "\n\tSetting DAC count to " COLOR_BRIGHT_MAGENTA "%d " COLOR_RESET "counts ...\n", DAC_count );
    StringH::Erase_Num_Chars(pi_command, 100);
	sprintf(pi_command, "SET_DAC_COUNT %d\n", DAC_count);
	this->PI_Send(pi_command);
    this->PI_Wait_Receive();
    //printf(COLOR_BROWN "%s\n" COLOR_RESET, receive_buffer);
}

int PI::Get_Socket_Address()
{
    return this->pi_sock_addr;
}


///////////------------------------------------------------------------------------------------------------

// This needs to be completed
double PI::Get_Coldfinger_Temp( )
{
    if( !this->Is_Connected() )
            return 1000;
    //      Check what mode Pi is in before reading coldfinger temp
    //      or perhaps coldfinger temp command in any command mode

    this->PI_Send("READ_ADC 1");   // read 1 value from PI to get current coldfinger temp
    this->PI_Wait_Receive( );
    this->pi_Data = PI::Parse_PI_Data( );
    double temp_diode_V = this->pi_Data[0]->ADC0_DiodeV;

    //double temp_Kelvins = Temp_Diode::get_Temp_K( temp_diode_V);
    //return temp_Kelvins;
    return temp_diode_V;
}

void PI::Erase_String(char* target_string)
{
   memset(target_string,0,sizeof(target_string));     // zero out string memory locations
   target_string[0] = '\0';                           // add null terminator
   for(int i =0; i< ARRAY_SIZE(target_string); ++i)
        target_string[i] = '\0';                       // put null terminator in every character location
}

// receive argument specified number of bytes, wait until the required number of bytes are read
char* PI::Receive_nBytes( int numBytesToRead )
{
	StringH::Erase_Num_Chars( (char*)this->complete_receive_string, 5000 );
    StringH::Erase_Num_Chars( receive_buffer, 5000 );

    if( !this->Is_Connected() )
        return NULL;

	while( numBytesToRead > 0 )
	{
		int num_chars_read = Pi_Socket::Receive_Data( this->pi_sock_addr, receive_buffer );
		numBytesToRead -= num_chars_read;
        receive_buffer[ num_chars_read ] = '\0';
		strncat( this->complete_receive_string, receive_buffer, num_chars_read );
		PI::Erase_String( receive_buffer );
	}
    this->complete_receive_string[ strlen(this->complete_receive_string) ] = '\0';
    char* received_string = strdup( this->complete_receive_string );

    return received_string;
}

// Receive whatever data is available now on the socket
// string returned from this function must be freed after use
char* PI::PI_Receive( )
{
    StringH::Erase_Num_Chars( receive_buffer, RECEIVE_BUFFER_SIZE);
    Pi_Socket::PI_Receive(this->pi_sock_addr, receive_buffer);      // store received data on temp string of "receive_buffer"
    strncpy(this->complete_receive_string, receive_buffer, RECEIVE_BUFFER_SIZE);    // copy "receive_buffer" to permanent "complete_receive_string"
    usleep(ONE_MILLISECOND*10);
    if( Pi_Socket::Get_Bytes_Available(this->pi_sock_addr) )    // if more data on the socket still
    {
        StringH::Erase_Num_Chars( receive_buffer, RECEIVE_BUFFER_SIZE); // erase the temporary buffer
        Pi_Socket::PI_Receive(this->pi_sock_addr, receive_buffer);      // receive more data to temporary buffer

        int bytes_available = RECEIVE_BUFFER_SIZE - strlen(this->complete_receive_string); // storage space available left on "complete_receive_string"
        strncat(this->complete_receive_string, receive_buffer, bytes_available);          // append additional data to "complete_receive_string"
        strcpy(receive_buffer, this->complete_receive_string);                             // copy the complete received string to receive buffer
    }
    return strdup(receive_buffer);      // return a copy of the string received on the socket. 
}

// wait until you receive data on the PI Socket
void PI::PI_Wait_Receive()
{
    if( !this->Is_Connected() )
          return;

    usleep(ONE_MILLISECOND*10);
    StringH::Erase_Num_Chars( receive_buffer, RECEIVE_BUFFER_SIZE);
    StringH::Erase_Num_Chars( complete_receive_string, RECEIVE_BUFFER_SIZE);

    bool pi_received = false;
    char* received_string = NULL;
    while( !pi_received )
    {
        usleep(ONE_MILLISECOND);
        //int bytes_avail = Get_Bytes_Available(this->pi_sock_addr);
        if( (received_string != NULL) && (strlen( received_string ) > 1) )
        {
            pi_received = true;
            free(received_string);  // no need to store this string, as its already stored internally on "this->complete_receive_string"

            return;
        }
        else
            received_string = this->PI_Receive( );
    }
}

void PI::PI_Send(const char* send_string)
{
    Pi_Socket::PI_Send( this->pi_sock_addr, (char*)send_string );
}

int PI::Connect_to_PI( const char* server_IP, const char* server_Port )
{
    this->pi_sock_addr =  Pi_Socket::Connect_to_PI( server_IP, server_Port );
    if( this->pi_sock_addr > 0)
    {
        this->connected = true;
        StringH::To_Upper(ETH_MAC);     // convert hexadecimal letters to uppercase
        printf("\n\tPI DUT ETH_ADDR: %s\n" COLOR_RESET, ETH_ADDR);
        printf("\t            MAC: %s\n" COLOR_RESET, ETH_MAC);
    }

    return this->pi_sock_addr;
}

bool PI::Is_Connected()
{
    if( !this->connected )
        return false;
    else if( this->pi_sock_addr < 0 )
        return false;
    else
        return true;
}

void PI::Free_PI_Data_Memory(PI_Data_Point** data)
{
    if( data == NULL)
        return;

    int i = 0;
    while( data[ i ] != NULL)
    {
        free( data[ i ] );
        data[i] = NULL;
        ++i;
    }
    free(data);
    data = NULL;
}

// free dynamic memory in allocated in pi_Data array list
void PI::Free_Mem()
{
    int cur_index = 0;

    while( pi_Data[ cur_index ] != NULL )   // free memory from each array location which contains a data point
    {
        delete( pi_Data[ cur_index ] );
        pi_Data[ cur_index ] = NULL;
        ++cur_index;
    }
    free( pi_Data );    // free memory allocated to the array
    pi_Data = NULL;
}


// parses data when Pi sends responses in CAL mode
// each grouping of data should be 10 samples. Each "Sample" Is a 1 second snapshot in time of all PI Metrics
PI_Data_Point** PI::Parse_PI_Data()
{
    if( this->pi_Data != NULL )
        this->Free_Mem();

    this->pi_Data = (PI_Data_Point**)malloc( sizeof( PI_Data_Point*) * PI_MAX_DATA_POINTS );
    pi_data_index = 0;

    for(int i =0; i< PI_MAX_DATA_POINTS; ++i)
        pi_Data[i] = NULL;    // initialize all memory locations to NULL

    int parse_len = strlen(receive_buffer) + 10;
    char parse_string[ parse_len ];
    StringH::Erase_Num_Chars( parse_string, parse_len );
    strcpy( parse_string, receive_buffer );    // make copy of incoming string so it isnt modified

    // Parse the incoming stream of data samples, one sample at a time. Below one sample is shown, but
    // the incoming string will contain many of these. Each one timestamped and having different sample#
    // A sample is of the form below, but may have different measurements depending on Cal mode
    //	         Sample#: 46
    //      ADC0_CoolV:	0.05370V
    //      ADC0_CoolV_Raw:	0.05292V
    //      ADC0_CoolA:	0.00217V
    //      ADC0_CoolA_Raw:	0.00229V
    //      ADC1_CoolA:	0.00216V
    //      ADC1_CoolA_Raw:	0.00228V
    //      ADC1_DiodeV:	0.00162V
    //      13:33:57  07/01/2020
    while( StringH::String_Contains( parse_string, "Sample#:" ) && pi_data_index < PI_MAX_DATA_POINTS)   // while there are still samples remaining to be parsed
    {
        int index_first_sample = StringH::Index_First_Sample( parse_string );
        int index_next_sample = StringH::Index_Second_Sample( parse_string );

        if( index_next_sample == -1 || index_next_sample > strlen(parse_string) )  // if there is no "next" sample, this is the last one
        {
            PI_Data_Point* new_data_point = PI::Parse_Sample( parse_string );
            this->Add_Sample_To_Data_Array( new_data_point );
            break;
        }
        else    // last sample in the list
        {
            char* single_sample = StringH::Get_Substring( parse_string, index_first_sample, index_next_sample );   // a substring containing only one "sample" in pi string array
            PI_Data_Point* new_data_point = PI::Parse_Sample( single_sample );                                   // parse the substring
            this->Add_Sample_To_Data_Array( new_data_point );           // Add parsed data to array
            free(single_sample);

            char* remaining_string = StringH::Get_Substring( parse_string, index_next_sample, strlen(parse_string)-1 );    // get string remaining left after the one we just parsed
            strcpy(parse_string, remaining_string);
            free(remaining_string);
        }
    }
    return this->pi_Data;
}


void PI::Add_Sample_To_Data_Array( PI_Data_Point* new_data_point )
{
      pi_Data[ pi_data_index ] = new_data_point;
      ++pi_data_index;
      pi_Data[pi_data_index] = NULL;        // null terminating the array to make it easy to detect when the end is reached
      pi_Data[pi_data_index+1] = NULL;
}

// parses 1 second worth of data from the PI. puts it into an object of type PI_Data_Point
PI_Data_Point* PI::Parse_Sample(char* pi_string)
{
    PI_Data_Point* new_data_point =  new PI_Data_Point();

    char* input_string = strdup( pi_string );       // duplicate input string, as it will be modified
    char* token = strtok(input_string, LINE_END);   // tokenize the string by lines, "\n". Every measurement is on a different line in the response

    while( token != NULL )
    {
        if( StringH::String_Contains(token, "Sample#") )
            new_data_point->sample_number = StringH::Parse_Int( StringH::Extract_Value_in_String(token) );
        if( StringH::String_Contains(token, "ADC0_CoolV:") )
            new_data_point->ADC0_CoolV = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC0_CoolA:") )
            new_data_point->ADC0_CoolA = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC1_CoolA:") )
            new_data_point->ADC1_CoolA = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "DiodeV:") )
            new_data_point->ADC0_DiodeV = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC1_LoadV:") )
            new_data_point->ADC1_LoadV = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC1_LoadmA:") )
            new_data_point->ADC1_LoadmA = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC0_CoolV_Raw:") )
            new_data_point->ADC0_CoolV = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC0_CoolA_Raw:") )
            new_data_point->ADC0_CoolA = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC1_CoolA_Raw:") )
            new_data_point->ADC1_CoolA = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC1_LoadV_Raw:") )
            new_data_point->ADC1_LoadV = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "ADC1_LoadmA_Raw:") )
            new_data_point->ADC1_LoadmA = StringH::Parse_Double( StringH::Extract_Value_in_String(token) );
        else if( StringH::String_Contains(token, "/202") ) // a timedate: XX:XX:XX XX/XX/2020
        {
            char* time_string = StringH::Get_Substring( token, 0, StringH::Index_First_Whitespace(token) );
            char* date_string = StringH::Get_Substring(token, StringH::Index_First_Whitespace(token), strlen(token) -1 );

            Time* data_time = Time::Parse_Time( time_string );
            free( time_string );

            Date* data_date = Date::Parse_Date( date_string );
            free( date_string );

            new_data_point->time.hour = data_time->hour;
            new_data_point->time.minute = data_time->minute;
            new_data_point->time.second = data_time->second;
            delete( data_time );

            new_data_point->date.month = data_date->month;
            new_data_point->date.day   = data_date->day;
            new_data_point->date.year  = data_date->year;
            delete data_date;
        }
        token = strtok(NULL, LINE_END);   // get next token
    }
    free(input_string);

    return new_data_point;
}


// copies the PI measured Load milliAmps values into a data stracture
// contains only 10 readings
Data_Group* PI::Extract_Load_mA_Data(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* LoadmA_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    LoadmA_data->data_points = (float*)malloc( sizeof(float)* 12);
    LoadmA_data->num_points  = 0;
    strcpy(LoadmA_data->units, "mA");
    strcpy(LoadmA_data->description, "PI Load");

    while( pi_data[pi_index] != NULL )
    {
        LoadmA_data->data_points[pi_index] = pi_data[pi_index]->ADC1_LoadmA;
        ++LoadmA_data->num_points;
        ++pi_index;
    }
    LoadmA_data->average = Data::Calc_Average( LoadmA_data );
    LoadmA_data->std_dev = Data::Calc_Std_Dev( LoadmA_data );
    return LoadmA_data;
}


// copies the PI measured Load milliAmps values into a data stracture
// contains only 10 readings
Data_Group* PI::Extract_Diode_V_Data(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* diode_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    diode_data->data_points = (float*)malloc( sizeof(float)* 12);
    diode_data->num_points  = 0;
    strcpy(diode_data->units, "V");
    strcpy(diode_data->description, "PI Diode");

    while( pi_data[pi_index] != NULL )
    {
        diode_data->data_points[pi_index] = pi_data[pi_index]->ADC0_DiodeV;
        ++diode_data->num_points;
        ++pi_index;
    }
    diode_data->average = Data::Calc_Average( diode_data );
    diode_data->std_dev = Data::Calc_Std_Dev( diode_data );
    return diode_data;
}

// copy the values of just the Load Volt measurements into an object and return it
Data_Group* PI::Extract_Load_V_Data(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* LoadV_data  = (Data_Group*)malloc( sizeof(Data_Group) );    // Allocate memory for data structure
    LoadV_data->data_points = (float*)malloc( sizeof(float)* 12);           // within data structure, allocate memory for each data point
    LoadV_data->num_points  = 0;

    strcpy(LoadV_data->units, "V");
    strcpy(LoadV_data->description, "PI Load");

    while( pi_data[pi_index] != NULL )
    {
        LoadV_data->data_points[pi_index] = pi_data[pi_index]->ADC1_LoadV;
        ++LoadV_data->num_points;
        ++pi_index;
    }
    LoadV_data->average = Data::Calc_Average(LoadV_data);
    LoadV_data->std_dev = Data::Calc_Std_Dev(LoadV_data);
    return LoadV_data;
}

// copy the values of just the PI measured CCC volts into an object and return it
Data_Group* PI::Extract_Cooler_V_Data(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* CoolV_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    CoolV_data->data_points = (float*)malloc( sizeof(float)* 12);
    CoolV_data->num_points  = 0;

    strcpy(CoolV_data->units, "V");
    strcpy(CoolV_data->description, "PI COOLER");

    while( pi_data[pi_index] != NULL )
    {
        CoolV_data->data_points[pi_index] = pi_data[pi_index]->ADC0_CoolV;
        ++CoolV_data->num_points;
        ++pi_index;
    }
    CoolV_data->average = Data::Calc_Average(CoolV_data);
    CoolV_data->std_dev = Data::Calc_Std_Dev(CoolV_data);
    return CoolV_data;
}

// copy the values of just the PI measured cooler milliAmps
Data_Group* PI::Extract_Cooler_mA_Data(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    Data_Group* CoolmA_data  = (Data_Group*)malloc( sizeof(Data_Group) );
    CoolmA_data->data_points = (float*)malloc( sizeof(float)* 12);
    CoolmA_data->num_points  = 0;

    strcpy(CoolmA_data->units, "mA");
    strcpy(CoolmA_data->description, "PI COOLER");
    while( pi_data[pi_index] != NULL )
    {
        CoolmA_data->data_points[pi_index] = pi_data[pi_index]->ADC1_CoolA;
        ++CoolmA_data->num_points;
        ++pi_index;
    }
    Data::Unit_Convert(CoolmA_data->data_points, 1000, CoolmA_data->num_points);
    CoolmA_data->average = Data::Calc_Average(CoolmA_data);
    CoolmA_data->std_dev = Data::Calc_Std_Dev(CoolmA_data);
    return CoolmA_data;
}

// Extracts PI measured cooler milliamps, into a data structure that prserves the timestamp information
AC_Data_Group* PI::Extract_AC_mA_Data(PI_Data_Point** pi_data)
{
    int pi_index = 0;

    AC_Data_Group* Cooler_mA_data  = new AC_Data_Group("PI Cooler [AC] Current", "mA");
    strcpy(Cooler_mA_data->units, "mA");

    while( pi_data[pi_index] != NULL )
    {
        float pi_Amps = pi_data[pi_index]->ADC1_CoolA;
        float pi_milliAmps = pi_Amps*1000;

        Time pi_time = pi_data[pi_index]->time;
        Date pi_date = pi_data[pi_index]->date;
        Timestamp* pi_timestamp = new Timestamp();
            pi_timestamp->date.month =pi_date.month;
            pi_timestamp->date.day =pi_date.day;
            pi_timestamp->date.year =pi_date.year;

            pi_timestamp->time.hour =pi_time.hour;
            pi_timestamp->time.minute =pi_time.minute;
            pi_timestamp->time.second =pi_time.second;

        AC_Data_Point* new_point = new AC_Data_Point(pi_milliAmps, pi_timestamp, AC_POWER_MILLIAMPS);
        Cooler_mA_data->data_points[ Cooler_mA_data->num_points ] = new_point;
        ++Cooler_mA_data->num_points;
        ++pi_index;
    }
    return Cooler_mA_data;
}

// Queries the local Wifi IP Address on the shell, then parse the response to an IP
// returns a string of just the wifi IP Address
char* PI::Get_Wifi_IP()
{
    char wifi_line[200];
    wifi_line[0] = '\0';

    while( strlen(wifi_line) == 0 )
    {
        PI::Query_Wifi(wifi_line);
    }

    char* wifi_address = StringH::Parse_IP(wifi_line);
    strcpy(WIFI_ADDR, wifi_address);
    free(wifi_address);

    return strdup(WIFI_ADDR);
}

char* PI::Get_ETH_IP()
{
    return strdup(ETH_ADDR);
}

// parses the local Ethernet IP and MAC address from the stores response to queries
void PI::Find_ETH_IP_and_MAC_Addr()
{
    char eth_line[200];
    eth_line[0] = '\0';

    printf("Searching for attached PI on local network.....\n");
    while( strlen(eth_line) == 0 )
    {
        PI::Query_ETH(eth_line);    // query the Linux shell for the IP
    }

    char* eth_address = StringH::Parse_IP(eth_line);
    strcpy(ETH_ADDR, eth_address);
    free(eth_address);

    char* eth_MAC = StringH::Parse_MAC(eth_line);
    strcpy(ETH_MAC, eth_MAC);
    StringH::To_Upper(ETH_MAC);
    free(eth_MAC);

    printf("Found PI @ IP: %s, MAC: " COLOR_BOLD_GREEN "%s\n" COLOR_RESET, ETH_ADDR, ETH_MAC);

    return;
}

char* PI::Get_ETH_MAC()
{
    if(strlen(ETH_MAC) > 10)
        return strdup(ETH_MAC);
    else
    {
        this->Find_ETH_IP_and_MAC_Addr();
        return strdup(ETH_MAC);
    }
}

// queries the local PI command line for what the wifi address is
// uses the following command:  sudo arp-scan --localnet -g -I wlan0 | grep -e 'Raspberry'\0
// stores the response string for later parsing
void PI::Query_Wifi(char* wifi_line)
{
    char** response = NULL;
    //    printf("wifi func start\n");
    //    printf("wifi cmd: \"%s\" \n", SEARCH_WIFI);

    usleep(ONE_MILLISECOND);
    errno = 0;
    response = PI::Exec_Shell_Command(SEARCH_WIFI);
    usleep(ONE_MILLISECOND*2000);
    if( response[0] == NULL )
    {
        printf("\tno response to shell command\n");
        while( response[0] == NULL)
        {
            printf("\texec search\n");
            usleep(ONE_MILLISECOND*2000);
            free(response);
            response = PI::Exec_Shell_Command(SEARCH_WIFI);
            usleep(ONE_MILLISECOND*100);
            if(response[0] != NULL)
                break;
        }
    }

    strcpy(wifi_line, response[0]);
    StringH::Free_String_Array_Memory(response);
    response = NULL;
}

// searches for the local 
// sudo arp-scan --localnet -g -I eth0 | grep "Raspberry"
void PI::Query_ETH(char* eth_line)
{
    char** response = NULL;

    usleep(ONE_MILLISECOND);
    response = PI::Exec_Shell_Command(SEARCH_ETHERNET);
    if(response[0] == NULL)
    {
        printf(COLOR_BOLD_RED "\tERROR: no PI_DAQ to calibrate was found on local Ethernet network.\n" COLOR_RESET);
        printf(COLOR_BOLD_RED "\tERROR: is PI_DAQ eth0 configured?.\n" COLOR_RESET);
        printf(COLOR_BOLD_RED "\t   **Configure PI DUT and verify you can ping it, then retry.\n" COLOR_RESET);
        exit(0);

    }
    strcpy(eth_line, response[0]);
    StringH::Free_String_Array_Memory(response);
    response = NULL;
}

/*
|--------------------------------------------------------------------------
| Exec_Shell_Command
|--------------------------------------------------------------------------
|
| Execute a command at the terminal prompt. The command is passed as argument.
| A string of the shell output response is returned. The string captures
| stdout and stderr.
|
| @param:  command    the shell command to execute
| @return:            a string of the shell response is returned
*/
char** PI::Exec_Shell_Command(const char* command)
{
    int MAX_LINES = 100;
    int MAX_LINE_LENGTH = 150;
    char read_line[250];



    FILE* command_shell = popen(command, POPEN_READ);                 //  The popen() function opens a process by creating a pipe, forking and invoking the shell.
    if (command_shell == NULL)
    {
        printf("Error opening pipe for shell command!!\n" );
        exit(1);
    }

    int line_count = 0;
    char**  string_array = (char**)malloc(sizeof(char*) * MAX_LINES);        // allocate memory for pointer to strings
    while (fgets(read_line, sizeof(read_line), command_shell) != NULL)     /* Read the output a line at a time - output it. */
    {
        StringH::Trim_Trailing_WhiteSpace(read_line);
        //printf("LINE:\t%s\n", read_line);
		string_array[line_count] = (char*)malloc(sizeof(char*) * ( strlen(read_line) +50 ) ); // allocate memory for the
        strcpy(string_array[line_count], read_line);
        ++line_count;
    }
    string_array[line_count] = NULL;   // Add a marker for the last string in the array
    int exit_code = pclose(command_shell);     // close file pointer to STDOUT
    return string_array;
}
