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

#include "StringH.h"
#include "USB_Helper.h"



/*

/dev/ttyUSB0
	Agilent Technologies
	E3648A
	0,1.7-5.0-1.0

/dev/ttyUSB1
	HEWLETT-PACKARD
	34401A
	0,11-5-2

/dev/ttyUSB2
	HEWLETT-PACKARD
	34401A
	0,2-1-1


*/

/*
int main( int argc, char** argv )
{

    printf("---------------------------------\n");
    USB_Device** dev_list = USB::Search_ttyUSB_Devices();

    int index = 0;
    printf("made it back before seg fault\n");
    if( dev_list != NULL )
    {
        while( dev_list[index] != NULL)
        {
            USB_Device* dev = dev_list[index];
            //char* mfr = dev->manufacturer;
            //char* model = dev->model;
            //printf("FD: %d\n", dev->file_descriptor);
            //printf("MFR: %s\n", mfr);
            //printf("model: %s\n", model);
            ++index;
        }
    }

    USB::Free_Mem_Device_List(dev_list);


  return 0;
}

*/