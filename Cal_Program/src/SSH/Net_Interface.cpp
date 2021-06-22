


#include "Net_Interface.h"
// START OF CODE WHICH MUST BE COMPILED AS C, HENCE COMPILER DECLARATION

#ifdef __cplusplus
extern "C" {
#endif

void Remove_WhiteSpace(char * trim_string)  // removes trailing whitespace (spaces and newlines)
{

    // first trim trailing whitespace
    int stringLen = strlen(trim_string);
    while(stringLen > 0 && isspace(trim_string[stringLen -1]) )
    {
        printf("iteration\n");
        --stringLen;
    }
    trim_string[stringLen] = '\0';

    // Next trim leading whitespace
    int start_index=0, new_index=0, old_index=0;

    // find index of first non-whitespace character
    while( (trim_string[start_index] == ' ' || trim_string[start_index] == '\t' || trim_string[start_index] == '\n') && start_index < strlen(trim_string))
    { ++start_index;  }

    printf("start now\n");
        printf(" time string: %s, %d\n", trim_string, start_index);
    
    if(start_index != 0)
    {
        // copy all characters over to the left
        old_index = start_index;

        while( trim_string[ old_index ] != '\0'  && old_index < strlen(trim_string) )
        {   printf("old index: %d\n", old_index);
            trim_string[ new_index ] = trim_string[ old_index ];
            ++new_index;
            ++old_index;
        }
        trim_string[new_index] = '\0'; // Make sure that string is NULL terminated
    }
    printf("returning\n");
}

bool Strings_Match(char* string1, char* string2)
{
    int index = 0;

    Remove_WhiteSpace(string1);
    Remove_WhiteSpace(string2);

    printf("made it passed remove whitespace\n");
    printf("\t%s matches %s: %d\n", string1, string2, strcmp(string1, string2) );

    if( strcmp(string1, string2) == 0)
        return true;
    else 
        return false;
}

// consumer must free the memory from the return object
network_interface** Get_Network_Interfaces()
{
    char          buf[1024];
    struct ifconf ifc;
    struct ifreq *ifr;
    int           sock;
    int           nInterfaces;
    int           i;

/* Get a socket handle. */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("Get_Local_Network_Interfaces:: socket creation failed");
        return NULL;
    }

/* Query available interfaces. */
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if(ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
        perror("Get_Local_Network_Interfaces:: ioctl(SIOCGIFCONF) error querying interfaces");
        return NULL;
    }

/* Iterate through the list of interfaces. */
    ifr         = ifc.ifc_req;
    nInterfaces = ifc.ifc_len / sizeof(struct ifreq);
    network_interface**  ifaces = (network_interface**)malloc(sizeof(network_interface*) * nInterfaces + 5);  // allocate memory for return array
    
    for(int i=0; i<nInterfaces; ++i)
        ifaces[i] = NULL;    // initialize each string pointer to NULL to keep track of termination

    const char* lo_iface = "lo";
    char iface_name[20];
    int count = 0;
    for(i = 0; i < nInterfaces; i++)
    {
        struct ifreq *item = &ifr[i];
        strncpy(iface_name, item->ifr_name, 20);

        /* Show the device name and IP address */
        if( (strlen(iface_name) > 0) /*&& !Strings_Match(iface_name, lo_iface) */ )
        {
            if(StringH::String_Contains(iface_name, lo_iface))  // avoid returning the "lo" or loopback interface
                continue;

            ifaces[count] = (network_interface*)malloc( sizeof(network_interface) ); // allocate memory for the network interface
            network_interface* new_interface = ifaces[count];   // for convenience declare a pointer to the mem location
            strncpy(new_interface->local_interface_name, item->ifr_name, 20);   // store the interface name string
            strncpy(new_interface->local_ip_addr, inet_ntoa(((struct sockaddr_in *)&item->ifr_addr)->sin_addr), 20);    // store the ip addr string
            ++count;
        }
    }
    return ifaces;
}

#ifdef __cplusplus
}
#endif

// END OF PORTION OF FILE WITH C MACRO

// class helper function to call C declared function (non class member) of similar name
void Net_Interface::Get_Local_Network_Interfaces()
{
    network_interface** interface_list =  Get_Network_Interfaces();
    int i = 0;
    while( interface_list[i])   // copy entries in return list to local list before freeing memory
    {
        strcpy(this->local_interfaces[this->local_iface_count].local_interface_name, interface_list[i]->local_interface_name);
        strcpy(this->local_interfaces[this->local_iface_count].local_ip_addr, interface_list[i]->local_ip_addr);
        ++i;
        ++(this->local_iface_count);
    }
    printf("\n");

    Net_Interface::Free_Mem_Interface_List(interface_list);
    this->Print_Local_Interfaces();
}

void Net_Interface::Print_Local_Interfaces()
{
    printf("  Local Interface List:\n");
    for(int i= 0; i<this->local_iface_count; ++i)
        printf(COLOR_MAGENTA "\t%s" COLOR_RESET  "\t(%s)\n", this->local_interfaces[i].local_interface_name, this->local_interfaces[i].local_ip_addr);
}

void Net_Interface::Print_Connected_Pi_List(remote_pi** pi_list)
{
    if(pi_list == NULL)
        return;

    int pi_index = 0;
    while(pi_list[pi_index] != NULL)
    {
        printf("\tPI on %s:\n", pi_list[pi_index]->local_interface_name);
        printf("\t\tIP:  %s\n", pi_list[pi_index]->pi_ip_addr);
        printf("\t\tMAC: %s\n\n", pi_list[pi_index]->pi_mac_addr);
        ++pi_index;
    }
}

void Net_Interface::Print_Interface_List(network_interface** iface_list)
{
    if(iface_list == NULL)
        return;

    int index = 0;
    while(iface_list[index] != NULL)
    {
        printf("\tLocal Network Connection %s:\t%s\n", iface_list[index]->local_interface_name, iface_list[index]->local_ip_addr);
        ++index;
    }
}

void Net_Interface::Free_Mem_Pi_List(remote_pi** pi_list)
{
    if( pi_list == NULL)
        return;

    int pi_index = 0;
    while(pi_list[pi_index]!= NULL)
    {
        free(pi_list[pi_index]);    // free the memory for each list location
        ++pi_index;
    }
    free(pi_list);
    pi_list = NULL;
}

void Net_Interface::Free_Mem_Interface_List(network_interface** iface_list)
{
    if( iface_list == NULL)
        return;

    int iface_index = 0;
    while(iface_list[iface_index]!= NULL)
    {
        free(iface_list[iface_index]);    // free the memory for each list location
        ++iface_index;
    }
    free(iface_list);
    iface_list = NULL;
}

// pre condition for this command is that we have already discovered our local interfaces
// ignores MAC ADDRESS specified in variable CAL_STATION_MAC_ADDR
// Order of operations:
//  looks through all local network interfaces for interface name with "eth" in it (looking for ethernet connection)
// if finds one with eth, grabs it & searches for Pis. If not, goes through each interface looking for attached PIs
remote_pi** Net_Interface::Find_Connected_PIs()
{
    remote_pi** pi_iface_list;
    remote_pi** attached_pi_list = (remote_pi**)malloc(sizeof(remote_pi*) * MAX_PIS_ON_NETWORK);

    for(int i=0; i< MAX_PIS_ON_NETWORK; ++i)
        attached_pi_list[i] = NULL;     // initialize all pointers to NULL

    int add_index = 0;  // index of connected pi to add to "attached_pi_list"

    if(!(this->local_iface_count > 0) ) // if we dont have any network interfaces, there are no network connected pis
        return NULL;

    // first get any local interfaces which are Ethernet
    network_interface** local_eth_interfaces = this->Get_Local_Ethernet_Interfaces();

    if(local_eth_interfaces != NULL)    // search found local ethernet interfaces for connected pis
    {
        int iface_index = 0;
        while(local_eth_interfaces[iface_index] != NULL)   // iterate through returned list of local ethernet interfaces
        {
            pi_iface_list = this->Find_Connected_PIs(local_eth_interfaces[iface_index]->local_interface_name);
            Net_Interface::Print_Connected_Pi_List(pi_iface_list);

            if(pi_iface_list != NULL)
            {
                int pi_index = 0;
                // add each pi found on this ethernet interface to the complete return list
                while( pi_iface_list[pi_index] != NULL)
                {
                    remote_pi* add_pi = (remote_pi*)malloc(sizeof(remote_pi));
                    strcpy(add_pi->local_interface_name, pi_iface_list[pi_index]->local_interface_name);
                    strcpy(add_pi->pi_ip_addr, pi_iface_list[pi_index]->pi_ip_addr);
                    strcpy(add_pi->pi_mac_addr, pi_iface_list[pi_index]->pi_mac_addr);
                    attached_pi_list[add_index] = add_pi;
                    ++pi_index;
                    ++add_index;
                }
                Net_Interface::Free_Mem_Pi_List(pi_iface_list);
            }
            ++iface_index;
        }
    }
    Net_Interface::Free_Mem_Interface_List(local_eth_interfaces);


    //printf("Attached PIs:\n");
    //Net_Interface::Print_Connected_Pi_List(attached_pi_list);

    return attached_pi_list;

}

// returns a list if there are any local Ethernet interfaces, if not returns NULL
network_interface** Net_Interface::Get_Local_Ethernet_Interfaces()
{
    //printf("\n  Local ETH Interfaces\n");
    char iface_name[20];
    network_interface** ethernet_interfaces = (network_interface**)malloc(sizeof(network_interface*)*MAX_NET_INTERFACES);
    for(int i =0; i< MAX_NET_INTERFACES; ++i)
        ethernet_interfaces[i] = NULL;

    int eth_index = 0;
    network_interface cur_iface;    // temp variable

    for(int i=0; i<this->local_iface_count; ++i)
    {
        cur_iface = this->local_interfaces[i];
        strncpy(iface_name, cur_iface.local_interface_name, 20);

        if( StringH::String_Contains_Ignore_Case(iface_name, "eth") )   // examine whether the local interface name contains the word "eth"
        {
            // add it to the ethernet interfaces list
            network_interface* eth_iface = (network_interface*)malloc(sizeof(network_interface));
            //printf("\t " COLOR_MAGENTA "%s" COLOR_RESET " (%s)\n", iface_name, cur_iface.local_ip_addr);
            strncpy(eth_iface->local_interface_name, cur_iface.local_interface_name, 20);
            strncpy(eth_iface->local_ip_addr, cur_iface.local_ip_addr, 20);
            ethernet_interfaces[eth_index] = eth_iface;
            ++eth_index;
        }
    }
    if(eth_index > 0)
        return ethernet_interfaces;
    else
    {
        free(ethernet_interfaces);
        return NULL;
    }
}

// returns a list of remote Pis that are connected via the local network interface name specified in the argument
// for instance "wlan0" or "eth0"
remote_pi** Net_Interface::Find_Connected_PIs(char* local_interface_name)
{
    //printf("\n-----FIND CONNECTED PIS ON %s------------\n\n", local_interface_name);
    char search_command[100];
    StringH::Erase_Num_Chars(search_command, 100);
    sprintf(search_command, "sudo arp-scan --localnet -g -I %s | grep \"Raspberry\"", local_interface_name );
    //printf("search command: %s\n", search_command);

    printf("\n  Searching " COLOR_MAGENTA "%s" COLOR_RESET " for connected Raspberry PIs\t" COLOR_CYAN "%s\n" COLOR_RESET, local_interface_name, search_command);

    remote_pi** pi_list = (remote_pi**)malloc(sizeof(remote_pi*) * MAX_PIS_ON_NETWORK);
    for(int i=0; i<MAX_PIS_ON_NETWORK; ++i)
        pi_list[i] = NULL;
    int add_index = 0;

    errno = 0;
    char** response = NULL;
    response = Net_Interface::Query_Shell(search_command);
    usleep(ONE_MILLISECOND*1000);   // give some time for the command to execute
    if( response == NULL || response[0] == NULL )
    {
        //printf("\treceived NULL from Query_Shell(). What the heck.\n\n");
       free(pi_list); 
        return NULL;
    }
    else
    {
        int i = 0;
        while( response[i]!= NULL)  // example response of one line: 192.168.123.4	b8:27:eb:87:3d:76	Raspberry Pi Foundation 
        {
            //print each line
            printf( COLOR_BOLD_GREEN "\t[%d]" COLOR_RESET COLOR_YELLOW "\t %s \t" COLOR_RESET, i, response[i] );
            char* line = response[i];
            ++i;
            StringH::Trim_WhiteSpace(line);


            // Parse out the ip address and the mac_address
            char* ip_addr = StringH::Parse_First_Token(line);
            char* mac_addr = StringH::Parse_Second_Token(line);
            StringH::To_Upper(mac_addr);

            // now check the parsed MAC Address against our known Cal Box, excluding CAL BOX from the list
            if(StringH::String_Contains_Ignore_Case(mac_addr, CAL_STATION_MAC_ADDR))
            {
                printf(COLOR_BOLD_RED "<-- not adding, this is the Calibration Pi\n" COLOR_RESET);
                free(ip_addr);
                free(mac_addr);
                continue;
            }
            else
            {
                printf("\n");
                //printf("adding PI: %s  on %s to list\n", mac_addr, local_interface_name);
                remote_pi* add_pi = (remote_pi*)malloc(sizeof(remote_pi));
                strncpy(add_pi->local_interface_name, local_interface_name, 20);
                strncpy(add_pi->pi_ip_addr, ip_addr, 20);
                strncpy(add_pi->pi_mac_addr, mac_addr, 20);

                pi_list[add_index] = add_pi;
                ++add_index;    // increment the index of STORED pi connections (not necessarily just ones we found)
                free(ip_addr);
                free(mac_addr);
            }
        }   // done adding all responses to the list
        printf("\n");
        //this->Print_Connected_Pi_List(pi_list);
    }

    StringH::Free_String_Array_Memory(response);
    response = NULL;
    return pi_list;
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
char** Net_Interface::Exec_Shell_Command(const char* command)
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
    for(int i=0; i<MAX_LINES; ++i)
        string_array[i] = NULL;

    while (fgets(read_line, sizeof(read_line), command_shell) != NULL)     /* Read the output a line at a time - output it. */
    {
        StringH::Trim_Trailing_WhiteSpace(read_line);
        //printf("LINE:\t%s\n", read_line);
        if(strlen(read_line) > 0)
        {
            string_array[line_count] = (char*)malloc(sizeof(char*) * ( strlen(read_line) +50 ) ); // allocate memory for the
            strcpy(string_array[line_count], read_line);
            ++line_count;
        }
    }
    string_array[line_count] = NULL;   // Add a marker for the last string in the array
    int exit_code = pclose(command_shell);     // close file pointer to STDOUT

    if(line_count == 0)
    {
        free(string_array);
        string_array = NULL;
    }

    return string_array;
}


char** Net_Interface::Query_Shell(const char* command)
{

    char** response = NULL;
    response = Net_Interface::Exec_Shell_Command(command);
    usleep(ONE_MILLISECOND*1500);   // give some time for the command to execute
    if( response == NULL || response[0] == NULL )
    {
        int attempts = 1;
        printf("\tno response from shell yet, trying again\n");
        while( (response == NULL || response[0] == NULL) && attempts<2)             // need to add a timeout here
        {
            //printf("num attempts: %d\n", attempts);
            ++attempts;
            usleep(ONE_MILLISECOND*2000);
            response = Net_Interface::Exec_Shell_Command(command);
            usleep(ONE_MILLISECOND*100);
            if(response == NULL || response[0] == NULL)
            {
                continue;
            }
            if(response[0] != NULL)
            {
                return response;
            }
        }
        printf("\tsorry, no shell response\n\n");
        printf(COLOR_RESET);
        free(response);
        response = NULL;
    }
    
    return response;
}