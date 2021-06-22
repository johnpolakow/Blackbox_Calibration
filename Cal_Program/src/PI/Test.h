
#ifndef _TEST_
#define _TEST_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>

#include "colors.h"
#include "StringH.h"





class Test
{
public:
    Test();

    static bool Verify_Response_Contains(char* response, const char* match_string);

};

#endif
