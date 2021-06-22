
#include "./DAC_Line.h"


const static int DAC_COUNT_TOKEN    = 1;
const static int PI_MA_TOKEN        = 2;
const static int METER_MA_TOKEN     = 3;



// Example Line:
//  [ DAC count, Pi mA, HP mA]
//  [ 20000, 25.0229, 24.8653 ]
void DAC_Line::Parse_Line(char* readline)
{
    Tokenizer token(readline);
    char* current_token;
    int token_num = 0;
    while( (current_token = token.Get_Next_Token() ) != NULL )
    {
        switch(token_num)
        {
            case DAC_COUNT_TOKEN:
                this->DAC_Count = StringH::Parse_Int(current_token);
                ++token_num;
                break;

            case PI_MA_TOKEN:
                this->Pi_value = StringH::Parse_Double(current_token);
                ++token_num;
                break;

            case METER_MA_TOKEN:   // The meter "token", or dataset, may contain up to 10 individual tokens. Start and End indicated by '[' & ']'
                this->Meter_value = StringH::Parse_Double(current_token);
                ++token_num;
                break;
        }
        free(current_token);
    }
    this->Error = this->Pi_value - this->Meter_value;
}


bool DAC_Line::Is_Data_Line(char* line)
{
 if( StringH::String_Contains(line, "-----------------")  || StringH::String_Contains(line, "─────────────────") )
    {
        return false;
    }

    Tokenizer tokenizer(line);

    char* token;
    int token_number = 0;
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

            else if( cur_char == ']' )
                --brackets_open;
            
            ++index;
        }      
    

    if(seen_bracket == true && brackets_open == 0)
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
