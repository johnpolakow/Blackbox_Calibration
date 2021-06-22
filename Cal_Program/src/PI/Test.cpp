





#include "Test.h"


bool Test::Verify_Response_Contains(char* response, const char* match_string)
{
    // verify "response" contains "match string" (not case sensitive)
    // if not contained in the response, print error
    bool verify_response = StringH::String_Contains_Ignore_Case(response, match_string);

    if( !verify_response)
    {
        printf( COLOR_BRIGHT_RED "\nError: failed verify.\n" COLOR_RESET);
        printf( COLOR_LIGHT_MAGENTA "\tresponse: \"%s\" does not contain expected \"%s\" \n", response, match_string);
        return false;
    }
    else
        return true;
}
