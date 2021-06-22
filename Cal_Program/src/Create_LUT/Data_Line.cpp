
#include "./Data_Line.h"

// order of fields in log file for parsing
const static int TIMESTAMP_TOKEN     = 0;
const static int PI_MA_TOKEN         = 1;
const static int METER_MA_TOKEN      = 2;
const static int METER_AVG_TOKEN     = 3;
const static int METER_STD_DEV_TOKEN = 4;
const static int METER_SPREAD_TOKEN  = 5;


// Example Line:
// timestamp, pi mA, meter mA, meter AVG, meter STD Dev, meter Spread
// 15:46:29    [101.41]    [724.28, 633.74, 554.53, 485.21, 424.56, 371.49]      AVG[532.30]   SD[120.641] SPR[352.79] 
void Data_Line::Parse_Line(char* readline)
{
    Tokenizer token(readline);
    char* current_token;
    int token_num = 0;
    bool meter_token_start = false;
    while( (current_token = token.Get_Next_Token() ) != NULL )
    {
        switch(token_num)
        {
            case TIMESTAMP_TOKEN:
                this->time = Time::Parse_Time(current_token);
                if(this->time != NULL)
                    ++token_num;
                break;

            case PI_MA_TOKEN:
                this->Pi_value = StringH::Parse_Double(current_token);
                ++token_num;
                break;

            case METER_MA_TOKEN:   // The meter "token", or dataset, may contain up to 5 individual tokens. Start and End indicated by [ & ]
                if( StringH::String_Contains(current_token, "[") )
                    meter_token_start = true;                          // flag to indicate we've received the first token
                
                this->Meter_Data[this->Meter_index] = StringH::Parse_Double(current_token);
                ++this->Meter_index;

                if( meter_token_start == true && StringH::String_Contains(current_token, "]") ) // "]" character marks the end of meter data tokens, so increment the token_number
                    ++token_num;
                break;
            
            case METER_AVG_TOKEN:
                this->Meter_Avg = StringH::Parse_Double(current_token);
                ++token_num;
                break;

            case METER_STD_DEV_TOKEN:
                this->Meter_Std_Dev_1sec = StringH::Parse_Double(current_token);
                ++token_num;
                break;
            
            case METER_SPREAD_TOKEN:
                this->Meter_spread_1sec = StringH::Parse_Double(current_token);
                ++token_num;
                break;
        }
        free(current_token);
    }
    this->Avg_Error = this->Pi_value - this->Meter_Avg;
}

void Data_Line::Print_Data()
{
    printf(COLOR_LIGHT_MAGENTA);
    char* time_str = this->time->toString();
    printf("%s   %.3f    ", time_str, this->Pi_value);
    free(time_str);
    for(int i=0; i < this->Meter_index; ++i)
    {
        printf("%.3f ", this->Meter_Data[i]);
    }
    printf("   ");
    printf("%.3f   %.3f   %.3f", this->Meter_Avg, this->Meter_Std_Dev_1sec, this->Meter_spread_1sec);

    printf(COLOR_RESET);

    if(this->Avg_Error >= 0)
        printf(COLOR_BRIGHT_GREEN "\t%.3f\n", this->Avg_Error);
    
    else
        printf(COLOR_BRIGHT_RED "\t%.3f\n", this->Avg_Error);

    printf(COLOR_RESET);
}


int Data_Line::Get_Num_Meter_Data_Points()
{
    return this->Meter_index;
}

bool Data_Line::Is_Data_Line(char* line)
{
    if( StringH::String_Contains(line, "-----------------")  || StringH::String_Contains(line, "─────────────────") )
    {
        return false;
    }

    StringH::Trim_WhiteSpace(line);
    Tokenizer tokenizer(line);

    char* token;
    int token_number = 0;
    int brackets_open = 0;
    bool seen_bracket = false;
    while( (token = tokenizer.Get_Next_Token() ) != NULL )
    {
        ++token_number;
        if(token_number == 1)
        {
            bool is_time = StringH::Line_Is_Time(token);
            free(token);
            token = NULL;
            if( !is_time )
            {   return false;   }

            continue;
        }
        else
        {
            int index = 0;
            while( token[index] != '\0')
            {
                char cur_char = token[index];
                if( cur_char == '[')
                {
                    ++brackets_open;
                    seen_bracket = true;
                }

                else if( cur_char == ']' )
                    --brackets_open;
                
                ++index;
            }
            free(token);
            token = NULL;
        }
    }

    if(seen_bracket == true && brackets_open == 0)
    {
        return true;
    }
    else
    {
        printf("line is NOT data point: \"%s\"\n", line);
        printf("seen bracket?: %d\n", seen_bracket);
        printf("brackets open: %d\n", brackets_open);
        return false;
    }
}


void Data_Line::Print_Data(Data_Line* printline)
{
    printf(COLOR_LIGHT_MAGENTA);
    char* time_str = printline->time->toString();
    printf("%s   %.3f    ", time_str, printline->Pi_value);
    free(time_str);
    for(int i=0; i < 10; ++i)
    {
        if(printline->Meter_Data[i] > 0)
            printf("%.3f ", printline->Meter_Data[i]);
    }
    printf("   ");
    printf("%.3f   %.3f   %.3f", printline->Meter_Avg, printline->Meter_Std_Dev_1sec, printline->Meter_spread_1sec);

    printf(COLOR_RESET);

    if(printline->Avg_Error >= 0)
        printf(COLOR_BRIGHT_GREEN "\t%.3f\n", printline->Avg_Error);
    
    else
        printf(COLOR_BRIGHT_RED "\t%.3f\n", printline->Avg_Error);

    printf(COLOR_RESET);
}

double Data_Line::Get_Meter_Average_1sec()
{
    double meter_sum = 0;
    for(int i=0; i < this->Meter_index; ++i)
    {
        meter_sum += this->Meter_Data[i];
    }
    this->Meter_Avg = meter_sum / (double)Meter_index;
    return this->Meter_Avg;
}

double Data_Line::Get_Meter_Std_Dev_1sec()
{
    double SD_sum = 0;
    double SD;

    double meter_avg = this->Get_Meter_Average_1sec();

    // calc standard deviation
    for(int i = 0; i < this->Meter_index; ++i)
    {
        double BK_val = this->Meter_Data[i];
        SD_sum += pow(BK_val - meter_avg, 2);
    }
    SD = sqrt(SD_sum/(double)this->Meter_index);

    return SD;
}

double Data_Line::Get_Meter_Spread_1sec()
{
    double BK_min = Meter_Data[0];
    double BK_max = Meter_Data[0];

    for(int i = 0; i < this->Meter_index; ++i)
    {
        double BK_val = this->Meter_Data[i];
        if( BK_val > BK_max)
            BK_max = BK_val;
        else if(BK_val < BK_min)
            BK_min = BK_val;
    }
    this->Meter_spread_1sec = BK_max - BK_min;
    return this->Meter_spread_1sec;
}