

#ifndef _TSTAMP_H_
#define _TSTAMP_H_

#include <ctype.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <ctime>

#include "TimeH.h"
#include "StringH.h"


class Timestamp
{
public:
    Timestamp();
    static char* Get_Timestamp();
    void Set_Now();
    bool Equals(Timestamp* other_timestamp);
    char* toString();

    Date date;
    Time time;
};




#endif
