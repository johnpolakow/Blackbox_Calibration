

#ifndef TOKEN_H
#define TOKEN_H


#include <wchar.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string>
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
#include <fstream>
#include <iostream>

#include "StringH.h"


// Data Structure class to hold measurements read from Pi or meters
class Tokenizer
{
public:
        Tokenizer(char* string)
        {
            char* parse_string = strdup(string);
            StringH::Trim_WhiteSpace(parse_string);

            int parse_len = strlen(parse_string);
            this->line = (char*)malloc( (sizeof(char) * (parse_len + 1) ));
            StringH::Erase_Num_Chars(this->line, parse_len+1);
            strncpy(this->line, parse_string, parse_len);

            //printf("\t tokenize: \"%s\" \n", this->line);
            this->cur_index = 0;
            free(parse_string);
        }
        ~Tokenizer(){ free(line); }

        char* Get_Next_Token();

private:
        char* line;
        int cur_index;
};

#endif
