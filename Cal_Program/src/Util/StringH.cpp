

#include "StringH.h"
#include "TimeH.h"

#define BUF_LEN 200

// Must free memory after this function
char* StringH::Get_Substring(const char* full_string, int start_index, int end_index)
{
   int substring_length = end_index - start_index + 1;
   char* substring = (char*)malloc(sizeof(char*) * substring_length + 1);        // allocate memory for the substring

   //Erase_String_Contents(substring);
   // Copy Substring from start_index to end index

   for( int i = 0; i< (end_index - start_index + 1); ++i)
   {
       substring[i] = full_string[i + start_index];
   }

   substring[ substring_length ] = '\0';                                // add null terminating character
   StringH::Trim_Leading_WhiteSpace( substring );
   StringH::Trim_Trailing_WhiteSpace( substring );
   return substring;
}

int StringH::parse_USBTMC_Number(char* path)
{
    char devpath[strlen(path)];
    strcpy(devpath, path);

    char delimiter[] = "/";    // delimiters to be used in parsing out tokens
    char* token = strtok(devpath, delimiter);
    int digit;

    while( token != NULL)
    {
        if( StringH::String_Contains_Ignore_Case(token, "USBTMC") )
        {
            for( int i = 0; i<strlen(token); ++i)
            {
                char single_char = token[i];
                if( isdigit( single_char ))
                {
                    digit = single_char - '0';
                    return digit;
                }
            }
            return digit;
        }
        token = strtok(NULL, delimiter);

    }
    return 0;
}

// converts all letters to uppercase
void StringH::To_Upper(char* lowercase_string)
{
    for(int i = 0; i<strlen(lowercase_string); ++i)
    {
        char capital_letter = toupper( lowercase_string[i] ); // if character is a lowercase letter, convert to uppercase
        lowercase_string[i] = capital_letter;
    }
}

int StringH::Parse_Int(char* integer_string)
{
   int first_dig_index = 0;
   for(int i = 0; i< strlen(integer_string); ++i)
   {
       if( isdigit(integer_string[i]) || integer_string[i] == '+' || integer_string[i] == '-' )
       {
           first_dig_index = i;
           break;
       }
   }
   char* int_substr = StringH::Get_Substring(integer_string, first_dig_index, strlen(integer_string));
   char* end_ptr;
   int parsed_int = strtol(int_substr, &end_ptr, 10);   // parse the substring to integer using stringtolong function
   free(int_substr);
   return parsed_int;
 }

double StringH::Parse_Double(char* double_string)
{
    bool is_negative = false;
    StringH::Trim_Trailing_WhiteSpace(double_string);

    // Find index of start of where to parse. Looking for first digit or + or -
    int start_index = 0;
    for(int i = 0; i<strlen(double_string); ++i )
    {
        if(double_string[i] == '+' || double_string[i] == '-')
        {
            if( isdigit(double_string[i+1]) && double_string[i] == '-' ) // if first char is a sign, and next character is a digit, found starting point
            {
                start_index = i+1;
                is_negative = true;
                break;
            }
        }
        if( isdigit(double_string[i]) )
        {
            start_index = i;
            break;
        }
    }

    // find index of last valid digit
    int end_index = start_index;                              // start search of last "digit" character at the beginning of the integer portion
    for(int i=start_index; i<strlen(double_string); ++i )
    {
        if( isdigit( double_string[i] ) )
        {
            end_index = i;
            continue;
        }
        else
        {
            if( double_string[i] == '.' && isdigit( double_string[ i +1 ] ) )
            {
                end_index = i+1;
                continue;
            }
            else
                break;
        }
     }
    
     bool scientific_notation = false;
     double multiplier = 0;
    if(double_string[end_index+1] == 'e' || double_string[end_index+1] == 'E')
    {
        scientific_notation = true;
        char* exp_string = StringH::Get_Substring( double_string, end_index+2, strlen(double_string) );
        int exponent = StringH::Parse_Int(exp_string);
        multiplier = pow(10, exponent);
        free(exp_string);
    }

     char* integer_substring = StringH::Get_Substring(double_string, start_index, end_index);
     integer_substring[ strlen(integer_substring) ] = '\0';    // add null terminating character

     char* end_ptr;
     double parsed_double = strtod(integer_substring, &end_ptr);   // parse the substring to integer using stringtolong function
     if(scientific_notation)
     {
        parsed_double *= multiplier;
     }
     if( is_negative )
        parsed_double *= -1;
    free(integer_substring);

    return parsed_double;
 }

bool StringH::Line_Is_Whitespace(char* line)
{
    for(int i =0; i<strlen(line); ++i)
    {
        if(!isspace(line[i]) && line[i] !='\0')
            return false;
    }
    //printf("line is whitespace\n");
    return true;
}


// look for date of the form 11-02-2020 or 11/02/2020 (or only 2 digit year)
bool StringH::Line_Is_Date(char* line)
{
    //printf("function line is date\n");
    bool is_date;
    Date* date = Date::Parse_Date(line);
    if( date == NULL )
        is_date = false;
    else
        is_date = true;

    delete(date);

    return is_date;
}

bool StringH::Line_Is_Time(char* line)
{
    bool is_time;
    //printf("line is time: %s\n", line);
    Time* time = Time::Parse_Time(line);
    if( time == NULL )
        is_time = false;
    else
        is_time = true;

    delete(time);

    return is_time;
}

bool StringH::Line_Is_TimeStamp(char* line)
{
    bool is_time;
    Time* line_time = Time::Parse_Timestamp(line);
    if( line_time == NULL )
        is_time = false;
    else
        is_time = true;

    delete(line_time);

    return is_time;
}


char* StringH::Parse_IP(char* line)
{
    int index_first_space = Index_nTH_Delimiter( line, 1, ' ' );
    char* ip_addr = Get_Substring(line, 0, index_first_space);
    return ip_addr;
}


// determines if string is of the form "XX." where X is hex digit, and . is a delimiter from the set [ whitespace, ., :]
bool StringH::Is_Hex_Group(char* cstring, int index)
{
    if(cstring == NULL)
        return false;
    
    if( strlen(cstring) < 3)
        return false;

    char char1 = cstring[index];
    char char2 = cstring[index+1];
    char char3 = cstring[index+2];

    if(isxdigit(char1) && isxdigit(char2) && Is_MAC_Delim(char3))
        return true;
    else
    {
        /*
        if(!isxdigit(char1))
            printf("\t%c is not a hex digit\n", char1);

        if(!isxdigit(char2))
            printf("\t%c is not a hex digit\n", char2);

        if(!Is_MAC_Delim(char3))
            printf("\t%c is not a MAC delim\n", char3);
*/

        return false;
    }

}

bool StringH::Is_MAC_Delim(char character)
{
    if(character == ' ' || character == ':' || character == '.' || character == '\0' || character == 0)
        return true;
    else
        return false;
}


char* StringH::Parse_MAC(char* line)
{
    int index_first_space = Index_nTH_Delimiter( line, 1, ' ' );
    int index_second_space = Index_nTH_Delimiter( line, 2, ' ' );
    char* mac_addr = Get_Substring(line, index_first_space, index_second_space);
    return mac_addr;
}


// Parses MAC from a string line
// Example line: "Cal log for MAC address:  B8.27.EB.87.3D.76"
// delimiter can be whitespace, period, or colon
char* StringH::Parse_MAC_Addr(char* line)
{
    StringH::Trim_WhiteSpace(line);
    bool End_of_Line = false;
    int cur_index = 0;
    char cur_char;
    int line_length = strlen(line);
    char* mac_string = NULL;

    if(line == NULL)
        return NULL;

    // looking for start of MAC addr in this loop
    while(cur_index < line_length)
    {
        cur_char = line[cur_index];
        int length_remaining = strlen(line) -cur_index;
        char* temp = StringH::Get_Substring(line, cur_index, strlen(line));
        //printf("length remaining: %d\t%s\n", length_remaining, temp);
        free(temp);
        if( length_remaining < 17)  // need at least 17 characters for the Hex digits and delimiters between digit groups
            return NULL;

        if( Is_Hex_Group(line, cur_index) && Is_Hex_Group(line, cur_index+3) && Is_Hex_Group(line, cur_index+6) && Is_Hex_Group(line, cur_index+9) \
            && Is_Hex_Group(line, cur_index+12) && Is_Hex_Group(line, cur_index+15) )      // is Hex digit [1-9,a-f,A-F]
        {
            char* mac_string = StringH::Get_Substring(line, cur_index, cur_index+17);
            //printf("parsed MAC: %s\n", mac_string);
            return mac_string;
        }
        
        else
        {
            if(!Is_Hex_Group(line, cur_index))
            {
                char* temp = StringH::Get_Substring(line, cur_index, cur_index+3);
                //printf(" Is Hex fails for group: %s\n", temp);
                free(temp);
            }
            else if(!Is_Hex_Group(line, cur_index+3))
            {
                char* temp = StringH::Get_Substring(line, cur_index+3, cur_index+6);
                //printf(" Is Hex fails for group: %s\n", temp);
                free(temp);
            }
            else if(!Is_Hex_Group(line, cur_index))
            {
                char* temp = StringH::Get_Substring(line, cur_index+6, cur_index+9);
                //printf(" Is Hex fails for group: %s\n", temp);
                free(temp);
            }
            else if(!Is_Hex_Group(line, cur_index))
            {
                char* temp = StringH::Get_Substring(line, cur_index+9, cur_index+12);
                //printf(" Is Hex fails for group: %s\n", temp);
                free(temp);
            }
            else if(!Is_Hex_Group(line, cur_index))
            {
                char* temp = StringH::Get_Substring(line, cur_index+12, cur_index+15);
                //printf(" Is Hex fails for group: %s\n", temp);
                free(temp);
            }
        } 
        

        ++cur_index;
    }
    return mac_string;
}



char* StringH::Parse_First_Token(char* readline)
{
    // skip to first non whitespace character
    // start tracking chars. Mark next whitespace as end of current token

    int start_index = 0;
    int end_index = 0;

    // start tracking the token at first non whitespace character
    for(start_index; start_index < strlen(readline); ++start_index )
    {
        if(isspace(readline[start_index]))
            continue;
        else
            break;
    }

    // end of first token is marked by the next whitespace encountered
    for(end_index = start_index; end_index < strlen(readline); ++end_index )
    {
        if( isspace(readline[end_index]) )
            break;
        else
            continue;
    }

    char* token = StringH::Get_Substring(readline, start_index, end_index);

    return token;
}




char* StringH::Parse_Third_Token(char* readline)
{
    // skip to first non whitespace character
    // start tracking chars. Mark next whitespace as end of current token
    int first_whitespace_index = 0;
    int first_token_end_index = 0;
    int second_token_start_index = 0;
    int second_token_end_index = 0;
    int third_token_start_index = 0;
    int third_token_end_index = 0;

    // process begins by finding first non whitespace character, so consume all leading whitespace
    for(first_whitespace_index; first_whitespace_index < strlen(readline); ++first_whitespace_index )
    {
        if(isspace(readline[first_whitespace_index]))
            continue;
        else
            break;
    }

    // end of first token is marked by the next whitespace encountered
    for(first_token_end_index = first_whitespace_index; first_token_end_index < strlen(readline); ++first_token_end_index )
    {
        if( isspace(readline[first_token_end_index]) )
            break;
        else
            continue;
    }

    // consume whitespace between token#1 & token#2, loop breaks when a non whitespace character is encountered
    while( isspace(readline[first_token_end_index]) && first_token_end_index < strlen(readline) )
    {
        ++first_token_end_index;
    }

    second_token_start_index = first_token_end_index;
    second_token_end_index = second_token_start_index;

    // now consume all non whitespace chars, which marks the content of the second token. loop breaks when a whitespace character is encoutnered
    while( !isspace(readline[second_token_end_index]) && second_token_end_index < strlen(readline) )
    {
        ++second_token_end_index;
    }

    // consume whitespace between token#2 & token#3, loop breaks when a non whitespace character is encountered
    while( isspace(readline[second_token_end_index]) && second_token_end_index < strlen(readline) )
    {
        ++second_token_end_index;
    }

    third_token_start_index = second_token_end_index;
    third_token_end_index = third_token_start_index;

    // now consume all non whitespace chars, which marks the content of the second token. loop breaks when a whitespace character is encoutnered
    while( !isspace(readline[third_token_end_index]) && third_token_end_index < strlen(readline) )
    {
        ++third_token_end_index;
    }


    char* token = StringH::Get_Substring(readline, third_token_start_index, third_token_end_index);
    //printf("third token: %s\n", token);

    return token;
}


char* StringH::Get_First_Token(char* line)
{
    if(line == NULL)
        return NULL;
    
    int start_index = 0;
    int end_index = 0;

    for(int i=0; i<strlen(line); ++i)
    {
        if( isspace( line[i]) )
            continue;
        else
        {
            start_index = i;
            break;
        }
    }
    for(int i=start_index+1; i<strlen(line); ++i)
    {
        if( isspace(line[i]) )
            end_index = i;      // end of first token when we hit whitespace
        else
            continue;
    }

    char* first_token = StringH::Get_Substring(line, start_index, end_index);

    return first_token;
}


char* StringH::Parse_Second_Token(char* readline)
{
    // skip to first non whitespace character
    // start tracking chars. Mark next whitespace as end of current token

    int whitespace_index = 0;
    int first_token_index = 0;
    int second_token_start = 0;
    int second_token_end = 0;

    // process begins by finding first non whitespace character, so consume all leading whitespace
    for(whitespace_index; whitespace_index < strlen(readline); ++whitespace_index )
    {
        if(isspace(readline[whitespace_index]))
            continue;
        else
            break;
    }

    // end of first token is marked by the next whitespace encountered
    for(first_token_index = whitespace_index; first_token_index < strlen(readline); ++first_token_index )
    {
        if( isspace(readline[first_token_index]) )
            break;
        else
            continue;
    }

    // now consume whitespace between token#1 & token#2, loop breaks when a non whitespace character is encountered
    while( isspace(readline[first_token_index]) && first_token_index < strlen(readline) )
    {
        ++first_token_index;
    }
    second_token_start = first_token_index;
    second_token_end = second_token_start;

    // now consume all non whitespace chars, which marks the content of the second token. loop breaks when a whitespace character is encoutnered
    while( !isspace(readline[second_token_end]) && second_token_end < strlen(readline) )
    {
        ++second_token_end;
    }

    char* token = StringH::Get_Substring(readline, second_token_start, second_token_end);

    return token;
}



bool StringH::Strings_Match(const char* string1, const char* string2)
{
    int index = 0;
    bool strings_match = true;

    char* match1 = strdup(string1);
    char* match2 = strdup(string2);
    StringH::Trim_WhiteSpace(match1);
    StringH::Trim_WhiteSpace(match2);

    int len1 = strlen(match1);
    int len2 = strlen(match2);

    //printf("str1: '%s'\nstr2: '%s'\n", match1, match2);


    while( index<strlen(match1) && index<strlen(match2) && (index < len1) && (index < len2) )
    {
        //printf("index: %d \t %c   %c\n", index, match1[index], match2[index] );
        if( !(match1[index] == match2[index]) )
        {
            strings_match = false;
            break;
        }
        ++index;

    }
    free(match1);
    free(match2);
    return strings_match;
}

int StringH::Index_nTH_Delimiter( char* string, int n, char delimiter )
{
    int index = 0;
    int count = 0;
    for( index = 0; string[ index ] != '\0'; ++index)
    {
        char cur_char = string[index];
        if( delimiter == '\t' || delimiter == ' ' )
        {
            if( isspace(cur_char) )
            {
                ++count;
            }
            if(count == n)
                break;
        }
        else
        {
            if( cur_char == delimiter )
                ++count;
            if(count == n)
                break;
        }
    }
    return index;
}

void StringH::Convert_Character(char* line, char orig_char, char change_char)
{
    if(line == NULL)
        return;
    
    char cur_char;
    for(int i =0; i<strlen(line); ++i)
    {
        cur_char = line[i];
        if( cur_char == orig_char)
            line[i] = change_char;
    }

    return;
}


int StringH::Index_First_Colon( char* string )
{
    int index = 0;
    for( index = 0; string[ index ] != '\0'; ++index)
    {
        char cur_char = string[index];
        if( cur_char == ':' )
            break;
    }
    return index;
}

int StringH::Get_Last_Index_Of(char* str, char find_char)
{
    int last_index = -1;
    for(int i=0; i<strlen(str); ++i)
    {
        if( str[i] == find_char)
            last_index = i;
    }

    return last_index;
}

void StringH::Erase_String(char* target_string)
{
   memset(target_string,0,sizeof(target_string));                                     // zero out string memery locations
   target_string[0] = '\0';         // add null terminator
   for(int i =0; i< ARRAY_SIZE(target_string); ++i)
        target_string[i] = '\0';
}

void StringH::Erase_Num_Chars(char* string, int length)
{
   memset(string,0,sizeof(string));    // zero out string memory locations
   for(int i = 0; i< length; ++i)
        string[i] = '\0';
}

void StringH::Trim_Leading_WhiteSpace(char* trim_string)
{
    int start_index=0, new_index=0, old_index=0;

    // find index of first non-whitespace character
    while(trim_string[start_index] == ' ' || trim_string[start_index] == '\t' || trim_string[start_index] == '\n')
    { ++start_index;  }

    if(start_index != 0)
    {
        // copy all characters over to the left
        old_index = start_index;
        while( trim_string[ old_index ] != '\0' )
        {
            trim_string[ new_index ] = trim_string[ old_index ];
            ++new_index;
            ++old_index;
        }
        trim_string[new_index] = '\0'; // Make sure that string is NULL terminated
    }
}

void StringH::Trim_Trailing_WhiteSpace(char * trim_string)                              // removes trailing whitespace (spaces and newlines)
{

   int stringLen = strlen(trim_string);
   while(stringLen > 0 && isspace(trim_string[stringLen -1]) )
   {
      stringLen--;
   }
   trim_string[stringLen] = '\0';
}

void StringH::Trim_WhiteSpace(char * trim_string)      // removes all leading andtrailing whitespace (spaces and newlines)
{
    StringH::Trim_Trailing_WhiteSpace( trim_string );
    StringH::Trim_Leading_WhiteSpace( trim_string );
}


char* StringH::Extract_Value_in_String(char* line)
{
   	char read_line[512];
    bool found = false;

   	if( line == NULL )
    {
        printf("KeyValueError: line argument is NULL\n");
   		  return NULL;
   	}

    StringH::Trim_Leading_WhiteSpace(line);
    strcpy(read_line, line);
    if(read_line[0] == '#')   // if '#' begins a line in a file, the line is commented out and thus not active in the file
          return NULL;
    else if( read_line[0] == '/' && read_line[1] == '/') // if line begins with '//' it is considered commented out
          return NULL;

    int start_index = 0;
    int end_index = strlen(read_line) - 1;

    for(int i=0; i< strlen(read_line); ++i)
    {
        if( read_line[i]==':' || isspace(read_line[i]) )  // look for delimiter between key and value, which is either ':' or ' '
        {
            found = true;
            start_index = i+1;
            break;
        }
    }
    if( !found )
        return NULL;

    char* value = StringH::Get_Substring(read_line, start_index, end_index);
        return value;
 }

int StringH::Index_First_Whitespace( char* socket_string )
{
    int index = 0;
    for( index = 0; socket_string[ index ] != '\0'; ++index)
    {
        char cur_char = socket_string[index];
        if( isspace(cur_char) )              // isspace() returns true for: space, tab, newline, form feed, carriage return, vertical tab
            break;
    }
    return index;
}

int StringH::Index_Second_Whitespace( char* socket_string )
{
    int index = 0;
    bool passed_first_whitespace = false;
    for( index = 0; socket_string[ index ] != '\0'; ++index)
    {
        char cur_char = socket_string[ index ];
        char next_char = socket_string[ index+1 ];
        if( isspace(cur_char) && !passed_first_whitespace && !isspace(next_char) )      // mark flag for first whitespace
            passed_first_whitespace = true;
        else if( isspace(cur_char) && passed_first_whitespace )                         // exit on second whitespace
        {
            break;
        }
        else
            continue;
    }
    return index;
}

int StringH::Index_First_Sample(char* parse_string )
{
    char* first_match_ptr = strstr( parse_string, "Sample#:");

    if( first_match_ptr == NULL )
        return -1;
    else
        return first_match_ptr - parse_string;
}


int StringH::Index_Second_Sample(char* parse_string )
{
    int index_first_match = StringH::Index_First_Sample( parse_string );

    if( index_first_match == -1 )
        return -1;

    char* remaining_string = StringH::Get_Substring(parse_string, index_first_match + strlen("Sample#:"), strlen(parse_string)-1 );
    char* second_match_ptr = strstr( remaining_string, "Sample#:");
    if( second_match_ptr == NULL)
        return -1;
    free(remaining_string);

    int second_match_index = second_match_ptr - remaining_string + index_first_match + strlen("Sample#:");

    return second_match_index;
}

 bool StringH::String_Contains(char* haystack, const char* needle)
 {
    bool contains_string;
    if( haystack == NULL || needle == NULL)
        return false;
    else if( strlen(needle) == 1)
    {
        char needle_char = needle[0];
        if( strchr(haystack, needle_char) == NULL)
        {
            return false;
        }
    }
    else if( strstr(haystack, needle) == NULL ) // strstr returns a substring if needle is in haystack, otherwise it returns null
    {
        return false;
    }
    return true;
 }

// returns the 0 based index of the first comma in the string
int StringH::Index_First_Comma( char* string )
{
    int index = 0;
    for( index = 0; string[ index ] != '\0'; ++index)
    {
        char cur_char = string[index];
        if( cur_char == ',' )
            break;
    }
    return index;
}

// returns the 0 based index of the first comma in the string
int StringH::Index_First_Period( char* string )
{
    int index = 0;
    for( index = 0; string[ index ] != '\0'; ++index)
    {
        char cur_char = string[index];
        if( cur_char == '.' )
            break;
    }
    return index;
}

 // used to parse values returned from SDM3065
 double StringH::Parse_Scientific_Notation( char* notation_string)
 {
     // if contains 'E'
         // get index of e
         // get substring after e
         // parse second substring to integer exponent
         // parse first substring before e to double
         // answer = leading double * 10^exponent

     int E_index = StringH::Index_of_E( notation_string );
     char* double_string = StringH::Get_Substring(notation_string, 0, E_index);
     double base_val = StringH::Parse_Double( double_string );

     char* exponent_string = StringH::Get_Substring(notation_string, E_index+1, strlen(notation_string)-1);
     int exponent = (int)strtol(exponent_string, NULL, 10);

     double tens_multiplier = pow(10, exponent);
     double float_value = base_val * tens_multiplier;

     return float_value;
 }

int StringH::Index_of_E( char* string )
{
    int E_index;
    for( E_index = 0; string[ E_index ] != '\0'; ++E_index )
    {
        char cur_char = string[E_index];
        if( cur_char == 'E' )
            break;
    }
    return E_index;
}

bool StringH::String_Contains_Ignore_Case(char* haystack, const char* needle)
{
    if( haystack == NULL || needle == NULL)
        return false;
    else if( strlen(needle) == 1)
    {
        if( strchr(haystack, toupper( needle[0]) ) == NULL)  // strchr returns whether a character is contained within a substring
        {
            return false;
        }
    }

    char haystack_cpy[strlen(haystack) + 2];
    memset(haystack_cpy,0,sizeof(haystack_cpy));
    strcpy(haystack_cpy, haystack);
    haystack_cpy[ strlen(haystack) ] = '\0';

    char needle_cpy[strlen(needle) + 2];
    memset(needle_cpy,0,sizeof(needle_cpy));
    strcpy(needle_cpy, needle);
    needle_cpy[ strlen(needle) ] = '\0';

    // convert both strings to uppercase
    int i=0;
    while( (haystack_cpy[i] != '\0') )
    {
        haystack_cpy[i] = toupper(haystack_cpy[i]);
        ++i;
    }

    i=0;
    while( (needle_cpy[i]) != '\0' )
    {
        needle_cpy[i] = toupper(needle_cpy[i]);
        ++i;
    }

    char* substring = strstr(haystack_cpy, needle_cpy); // strstr returns ppointer to first char of ocurrence if needle is in haystack, otherwise it returns null
    if( substring == NULL )
        return false;

    return true;
 }


 char* StringH::Get_File_Extension(char* filepath)
 {
     int index_first_period;

     for(int i=0; i < strlen(filepath); ++i)
     {
         char current_char = filepath[i];
         if( isalpha(current_char) || isdigit(current_char) || current_char == '/')
             continue;
         if( filepath[i] == '.' && filepath[i+1] == '/')
             continue;
         if( current_char == '.' )
         {
             index_first_period = i;
             break;
         }
     }
     return StringH::Get_Substring(filepath, index_first_period, strlen(filepath)-1);
 }


 // Extracts the filename from a fully specified filepath, includes the file extension
 char* StringH::Get_File_Name(char* filepath)
 {
     int index_last_slash;

     for(int i=0; i < strlen(filepath); ++i)
     {
         char current_char = filepath[i];
         if( current_char == '/')
         {
             index_last_slash = i;
             continue;
         }
     }

     return StringH::Get_Substring(filepath, index_last_slash+1, strlen(filepath)-1);
 }

 void StringH::Remove_File_Extension(char* filepath)
 {
      int index_first_period;

      for(int i=0; i < strlen(filepath); ++i)
      {
          char current_char = filepath[i];
          if( isalpha(current_char) || isdigit(current_char) || current_char == '/')
              continue;
          if( filepath[i] == '.' && filepath[i+1] == '/')
              continue;
          if( current_char == '.' )
          {
              index_first_period = i;
              break;
          }
      }
      for(int i = index_first_period+1; i < strlen(filepath); ++i)
          filepath[i] = '\0';
 }

 char* StringH::Get_File_Directory(char* filepath)
 {
     int index_last_slash;

     for(int i=0; i < strlen(filepath); ++i)
     {
         char current_char = filepath[i];
         if( current_char == '/')
         {
             index_last_slash = i;
             continue;
         }
     }

     return StringH::Get_Substring(filepath, 0, index_last_slash);
 }


void StringH::Free_String_Array_Memory(char** string_array)
{
    int i = 0;
    if(string_array == NULL)
        return;
    while(string_array[i] != NULL)
    {
        free(string_array[i]);
        ++i;
    }
    free(string_array);

    return;
}

  int StringH::Get_String_Array_Length(char** string_array)
  {
    int i = 0;
    if(string_array == NULL)
        return 0;
    while(string_array[i] != NULL)
    {
        ++i;
    }

    return i;
  }

void StringH::Advance_to_Index(char* write_buffer, int* index_ptr, int target_index)
{
    if( write_buffer == NULL)
        return;

    while( (*index_ptr < target_index) && (*index_ptr < BUF_LEN ) )
    {
        write_buffer[ (*index_ptr) ] = ' ';
        ++(*index_ptr);
    }
}

// column index is the column# to currently print a character at
void StringH::Write_Chars(char* write_buffer, int* column_index, const char* print_buffer, int MAX_CHARS)
{
    int num_chars_written = 0;
    int print_index = 0;

    while( (*column_index < BUF_LEN) && (num_chars_written < MAX_CHARS) )
    {

        if( print_buffer[ (print_index) ] == '\0')      // don't want to print the null character in the middle of concatenating strings
        {
            while( (*column_index < BUF_LEN) && (num_chars_written < MAX_CHARS) )
            {
                write_buffer[ (*column_index) ] = ' ';
                ++num_chars_written;
                ++(*column_index);
            }
            write_buffer[ (*column_index) ] = '\0';   // will get overwritten if more strings are added to write_buffer
            return;
        }
        else
          write_buffer[ (*column_index) ] = print_buffer[ print_index ];

        ++print_index;
        ++num_chars_written;
        ++(*column_index);
    }
    write_buffer[ (*column_index) ] = '\0';   // will get overwritten if more strings are added to write_buffer
    return;
}


bool StringH::Contains_Only_Digits(char* digit_string)
{
    int index = 0;
    while( digit_string[index] != '\0')
    {
        if( !isdigit( digit_string[index] ) )
        {
            printf("Error: digit string \"%s\" contains non digit at character \'%c\'\n", digit_string, digit_string[index]);
            return false;
        }
        ++index;
    }
    return true;
}


// Format of timestamp:       15:46:30   11-25-20
bool StringH::String_Is_TimeStamp(char* time_string)
{
    // Get First Token:
        // verify hour can be parsed to integer 00-23, max 2 chars
        // verify min can be parsed to integer 00-59, max 2 chars
        // verify second can be parsed to integer 00-59, max 2 chars

    // Get Second Token:
        // verify first token before '-' can be parse to integer between 1-12 max 2 chars
        // verify second token (day) can be parsed to integer between 1-31 max 2 chars
        // verify 2 digit year

    char *str_month, *str_day, *str_year, *str_hour, *str_minute, *str_second;
    int month, day, year, hour, minute, second;
    int look_ahead_index = 0;
    int substr_start_index=0, substr_end_index=0, substring_length = 0;

// PARSE "TIME" PART OF TIMEDATE STRING
//
    // PARSE THE HOUR
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit hour
    while( time_string[look_ahead_index] != ':' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first '.', should be max 2 chars before '.'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit hour
    substring_length = substr_end_index - substr_start_index + 1;
    if( substring_length > 2 || substring_length < 1)             // max 2 characters for "hour", but at least one character
    {
        printf("Error: hour in time string \"%s\" has less than one or more than 2 digits\n", time_string);
        return false;
    }
    else
    {
        str_hour = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
        if( !StringH::Contains_Only_Digits(str_hour) )
        {
            free(str_hour);
            return false;
        }
        hour = StringH::Parse_Int( str_hour );     // parse month string to an integer
        if( hour < 0 || hour > 23)
        {
            printf("Error: hour \"%d\" is not in valid range (0-23)\n", hour);
            free(str_hour);
            return false;
        }
        else
        free(str_hour);
    }
    // PARSE THE MINUTE
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "MINUTE" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit minute
    while( time_string[look_ahead_index] != ':' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first '.', should be max 2 chars before '.'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit minute
    substring_length = substr_end_index - substr_start_index + 1;
    if( substring_length > 2 || substring_length < 1)             // max 2 characters for "minute", but at least one character
    {
        printf("Error: minute in time string \"%s\" has less than one or more than 2 digits\n", time_string);
        return false;
    }
    else
    {
        str_minute = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
        if( !StringH::Contains_Only_Digits(str_minute) )
        {
            free(str_minute);
            return false;
        }
        minute = Parse_Int( str_minute );     // parse month string to an integer
        if( minute < 0 || minute > 59)
        {
            printf("Error: minute \"%d\" is not in valid range (0-59)\n", minute);
            free(str_minute);
            return false;
        }
        else
        free(str_minute);
    }
    // PARSE THE SECOND
    //++look_ahead_index;                           // increment, so now look_ahead_index should be in "SECOND" part of the timedate string


    if(time_string[look_ahead_index] != ':')
        while( time_string[look_ahead_index] != ':' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of first '.', should be max 2 chars before '.'
    ++look_ahead_index;
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit SECOND

    substr_end_index = StringH::Index_First_Whitespace(time_string);
    look_ahead_index = substr_end_index;

    substring_length = substr_end_index - substr_start_index;
    if( substring_length > 2 || substring_length < 1)             // max 2 characters for "SECOND", but at least one character
    {
        printf("Error: second \"%s\" has less than one or more than 2 digits)\n", str_second);
        return false;
    }
    else
    {
        str_second = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
        if( !StringH::Contains_Only_Digits(str_second) )
        {
            printf("\"%s\" contains non digit characters\n", str_second);
            free(str_second);
            return false;
        }
        second = Parse_Int( str_second );     // parse month string to an integer
        if( second < 0 || second > 59)
        {
            printf("Error: second \"%d\" is not in valid range (0-59)\n", second);
            free(str_second);
            return false;
        }
        free(str_second);
    }

// PARSE "DATE" PART OF TIMEDATE STRING
//
    // PARSE MONTH PART OF THE STRING
    if( isspace(time_string[look_ahead_index]) )
    {
        while( isspace(time_string[look_ahead_index]) )
            ++look_ahead_index;
    }
    substr_start_index = look_ahead_index;
    while( time_string[look_ahead_index] != '-' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index first '-', should be max 2 chars before '-'
    substr_end_index = look_ahead_index -1;
    substring_length = substr_end_index - substr_start_index;
    if( substring_length > 2 || substring_length < 1)             // max 2 characters for "str_month", but at least one character
        return false;
    else
    {

        str_month = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
        if( !StringH::Contains_Only_Digits(str_month) )
            return false;
        month = StringH::Parse_Int( str_month );     // parse month string to an integer
        if( month < 1 || month > 12)
        {
            printf("Error: month \"%d\" is not in valid month range (1-12)\n", month);
            return false;
        }
        free(str_month);
    }
    // PARSE "DAY OF MONTH" PART OF STRING
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "DAY" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing one or two digit month day
    while( time_string[look_ahead_index] != '-' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index of second '-', should be max 2 chars before '-'

    substr_end_index = look_ahead_index-1;          // end of substring containing one or two digit day of month
    substring_length = substr_end_index - substr_start_index + 1;
    if( substring_length > 2 || substring_length < 1)             // max 2 characters for "day", but at least one character
        return false;
    else
    {
        str_day = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
        if( !StringH::Contains_Only_Digits(str_day) )
            return false;
        day = StringH::Parse_Int( str_day );     // parse month string to an integer
        if( day < 1 || day > 31)
        {
            printf("Error: day \"%d\" is not in valid day range (1-12)\n", day);
            return false;
        }
        free(str_day);
    }

    // PARSE YEAR PART OF THE STRING
    ++look_ahead_index;                           // increment, so now look_ahead_index should be in "YEAR" part of the timedate string
    substr_start_index = look_ahead_index;        // start of substring containing four or two digit YEAR
    while( time_string[look_ahead_index] != '_' && time_string[look_ahead_index] != '\0'){ ++look_ahead_index; }    // find index '_' marking end of year

    substr_end_index = look_ahead_index-1;          // end of substring containing four or two digit year
    substring_length = substr_end_index - substr_start_index + 1;
    if( substring_length != 4 && substring_length != 2)             // max 4 characters for "year", but at least two characters
    {
        printf("Error: year in time string \"%s\" is not either 4 or 2 digits\n", time_string);
        printf("calculated length and indices- len: %d digits  start_ind: %d [%c]   end_ind:   %d [%c]\n", substring_length, substr_start_index, time_string[substr_start_index], substr_end_index, time_string[substr_end_index]);
        str_year = StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
        printf("calculated substr:   \"%s\"\n", str_year);
        free(str_year);
        return false;
    }
    else
    {
        str_year =StringH::Get_Substring(time_string, substr_start_index, substr_end_index);
        year = StringH::Parse_Int( str_year );     // parse month string to an integer
        if( !StringH::Contains_Only_Digits(str_year) )
        {
            free(str_year);
            return false;
        }
        free(str_year);
    }
    return true;
}