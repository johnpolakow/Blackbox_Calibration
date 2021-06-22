

#include "String_Tokenizer.h"




char* Tokenizer::Get_Next_Token()
{
    char* token;

    if(this->line == NULL)
        return NULL;

    int index_whitespace = StringH::Index_First_Whitespace(this->line);

    if( index_whitespace == strlen(this->line) )
    {
        token = StringH::Get_Substring(this->line, 0, index_whitespace);
        free(this->line);
        this->line = NULL;
        return token;
    }
    else
    {
        token = StringH::Get_Substring(this->line, 0, index_whitespace);
        char* temp = strdup(this->line);    // copy current line string
        free(this->line);                   // free memory for original line
        this->line = StringH::Get_Substring(temp, index_whitespace, strlen(temp));  // update line to remove the token that is returned from this function
        free(temp);
    }
    return token;
}
