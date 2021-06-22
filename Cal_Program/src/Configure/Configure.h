
#ifndef _CONFIGURE_H
#define _CONFIGURE_H

#include "SSH_Helper.h"

class Configure
{
public:
        Configure(SSH_Helper* helper)    // argument should be ssh instance
        {
            this->ssh = helper;
        }

        ~Configure();


        bool Check_bcm2835_Lib_Installed();
        void Install_Lib_bcm2835();
        void Install_Lib(char* libname);
        void Compile_PI_DAQ_Program();
        void Reload_and_Start_DAQ_Server();


private:

    SSH_Helper* ssh;

};

#endif