
#ifndef _NET_IFACE_H
#define _NET_IFACE_H

#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "../Util/StringH.h"
#include "../Util/colors.h"
#include "../../Cal_Parameters.h"

const char IFACE_SEARCH_ETHERNET[] =  "sudo arp-scan --localnet -g -I eth0 | grep \"Raspberry\"";
const char IFACE_SEARCH_WIFI[] = "sudo arp-scan --localnet -g -I wlan0 | grep -e 'Raspberry'\0";

#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )    // macro to get number of items in an object array
//#define POPEN_READ "re"  // "r" opens for reading, "e" sets close on exec flag ("FD_CLOEXEC")
#define ONE_MILLISECOND 1000
#define MAX_NET_INTERFACES 10
#define MAX_PIS_ON_NETWORK 100

typedef struct net_interface_t
{
    char local_ip_addr[20];
    char local_interface_name[20];
}network_interface;

typedef struct remote_pi_t
{
    char pi_ip_addr[20];
    char pi_mac_addr[20];
    char local_interface_name[20];
}remote_pi;



// constructor of PI automatically turns off the cooler relay and sets load current to 0
class Net_Interface
{
public:
    Net_Interface()
    {
        for(int i=0; i<MAX_NET_INTERFACES; ++i)
        {
            local_interfaces[i].local_ip_addr[0] = '\0';
            local_interfaces[i].local_interface_name[0] = '\0';
        }
        
        for(int i=0; i<MAX_PIS_ON_NETWORK; ++i)
        {
            remote_connected_pis[i].pi_ip_addr[0] = '\0';
            remote_connected_pis[i].pi_mac_addr[0] = '\0';
            remote_connected_pis[i].local_interface_name[0] = '\0';
        }


        this->local_iface_count = 0;
        this->remote_pi_count = 0;
        this->Get_Local_Network_Interfaces();
        //Find_Connected_PIs();
    }

    void Find_ETH_IP_and_MAC_Addr();
    char* Get_ETH_IP();
    char* Get_ETH_MAC();
    int Get_Socket_Address();

    void Get_Local_Network_Interfaces();
    void Print_Local_Interfaces();
    //static network_interface** Get_Local_Network_Interfaces();
    remote_pi** Find_Connected_PIs();                // first look on Ethernet, if no PIs are found, look on WLAN
    static void Print_Interface_List(network_interface**);
    static void Print_Connected_Pi_List(remote_pi**);
    static void Free_Mem_Pi_List(remote_pi** pi_list);
    static void Free_Mem_Interface_List(network_interface** iface_list);

private:
    int String_Array_Num_Strings(char** string_array);
    void Query_Wifi(char* wifi_line);
    void Query_ETH(char* eth_line);

    remote_pi** Find_Connected_PIs(char* local_interface_name);
    network_interface** Get_Local_Ethernet_Interfaces();
    static char** Exec_Shell_Command(const char* command);
    static char** Query_Shell(const char* command);

    network_interface local_interfaces[MAX_NET_INTERFACES];             // assuming target will have a max of 6 network interfaces
    remote_pi remote_connected_pis[MAX_PIS_ON_NETWORK];
    int local_iface_count;
    int remote_pi_count;
};

#endif
