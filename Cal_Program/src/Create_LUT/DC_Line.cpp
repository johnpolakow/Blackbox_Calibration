
#include "./DC_Line.h"


const static int PI_TOKEN         = 1;
const static int METER_TOKEN      = 2;

const static int DAC_COUNT_TOKEN      = 1;
const static int DAC_PI_TOKEN         = 2;
const static int DAC_METER_TOKEN      = 3;

// Example Line:
// [ Pi meas mA, BK5335 meas mA]
// 	[ 494.42999, 484.11981 ]
void DC_Line::Parse_Line(char* readline)
{
    Tokenizer token(readline);
    char* current_token;
    int token_num = 0;
    bool BK_token_start = false;
    while( (current_token = token.Get_Next_Token() ) != NULL )
    {

        switch(token_num)
        {
                    //printf("token num : %d\t%s", token_num, current_token);
            case PI_TOKEN:
                this->Pi_value = StringH::Parse_Double(current_token);
                break;

            case METER_TOKEN:   // The BK "token", or dataset, may contain up to 5 individual tokens. Start and End indicated by [ & ]
                this->Meter_value = StringH::Parse_Double(current_token);
                break;
        }
        ++token_num;
        free(current_token);
    }
    this->Error = this->Pi_value - this->Meter_value;
}

void DC_Line::Parse_DAC_Line(char* readline)
{
    Tokenizer token(readline);
    char* current_token;
    int token_num = 0;
    bool BK_token_start = false;
    while( (current_token = token.Get_Next_Token() ) != NULL )
    {

        switch(token_num)
        {
            case DAC_COUNT_TOKEN:
                this->DAC_count = StringH::Parse_Int(current_token);
                break;
            case DAC_PI_TOKEN:
                this->Pi_value = StringH::Parse_Double(current_token);
                break;

            case DAC_METER_TOKEN:   // The BK "token", or dataset, may contain up to 5 individual tokens. Start and End indicated by [ & ]
                this->Meter_value = StringH::Parse_Double(current_token);
                break;
        }
        ++token_num;
        free(current_token);
    }
    this->Error = this->Pi_value - this->Meter_value;
}

bool DC_Line::Is_Data_Line(char* line)
{
    if( StringH::String_Contains(line, "-----------------")  || StringH::String_Contains(line, "─────────────────") )
    {
        return false;
    }

    bool seen_digits = false;
    int brackets_open = 0;
    bool seen_bracket = false;
    int index = 0;
    while( line[index] != '\0')
    {
        char cur_char = line[index];
        if( cur_char == '[')
        {
            ++brackets_open;
            seen_bracket = true;
        }

        if( isdigit(cur_char))
        {
            seen_digits = true;
        }

        if( isalpha(cur_char) )
            return false;

        else if( cur_char == ']' )
            --brackets_open;
        
        ++index;
    }      
    

    if(seen_bracket == true && brackets_open == 0 && seen_digits == true)
    {
        return true;
    }
    else
    {
        printf("line is NOT DC line: \"%s\"\n", line);
        printf("seen bracket?: %d\n", seen_bracket);
        printf("brackets open: %d\n", brackets_open);
        return false;
    }
}


bool DC_Line::Is_Data_Average(char* line)
{
    // if line begins with 'AVG['
    StringH::Trim_WhiteSpace(line);
    int index_whitespace = StringH::Index_First_Whitespace(line);
    char* first_token = StringH::Get_Substring(line, 0, index_whitespace);
    if(StringH::String_Contains_Ignore_Case(first_token, "AVG["))
    {
        free(first_token);
        return true;
    }
    free(first_token);
    return false;

}


bool DC_Line::Is_Std_Dev(char* line)
{
    // if line begins with 'STD DEV['
    StringH::Trim_WhiteSpace(line);
    int index_whitespace = StringH::Index_Second_Whitespace(line);
    char* first_token = StringH::Get_Substring(line, 0, index_whitespace);
    if(StringH::String_Contains_Ignore_Case(first_token, "STD DEV["))
    {
        free(first_token);
        return true;
    }
    free(first_token);
    return false;
}