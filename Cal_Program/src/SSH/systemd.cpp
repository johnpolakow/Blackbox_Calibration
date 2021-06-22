#include "systemd.h"

const char* parse_systemd::STATUS_ACTIVE = "active";
const char* parse_systemd::STATUS_ENABLED = "enabled";
const char* parse_systemd::STATUS_LOADED = "loaded";
const char* parse_systemd::STATUS_RUNNING = "(running)";
const char* parse_systemd::STATUS_EXITED = "(exited)";
const char* parse_systemd::STATUS_WAITING = "(waiting)";
const char* parse_systemd::STATUS_INACTIVE = "inactive (dead)";
const char* parse_systemd::STATUS_FAILED = "failed";


// sudo systemctl list-units | grep Blackberry

// example output of 'sudo systemctl status Blackberry.service'
/*
pi@PI-BB38:~ $ sudo systemctl status Blackberry.service
● Blackberry.service - Pi Blackbox Control Software
   Loaded: loaded (/lib/systemd/system/Blackberry.service; enabled; vendor preset: enabled)
   Active: active (running) since Wed 2021-06-16 11:33:15 PDT; 12min ago
  Process: 15638 ExecStartPre=/bin/sleep 30 (code=exited, status=0/SUCCESS)
 Main PID: 15670 (exec)
    Tasks: 3 (limit: 4915)
   CGroup: /system.slice/Blackberry.service
           └─15670 /home/pi/firmware/exec

Jun 16 11:32:44 PI-BB38 systemd[1]: Starting Pi Blackbox Control Software...
Jun 16 11:33:15 PI-BB38 systemd[1]: Started Pi Blackbox Control Software.
*/



// retrieve a single line substring from a multiline strinig buffer
char* parse_systemd::Get_nth_Line(const char* buffer, int target_line_number)
{
    int index_line_start = -1;
    int index_line_end = -1;
    bool seen_line_start = false;
    int cur_line_number = 1;

    int len = strlen(buffer);
    for(int i=0; i< len; ++i)
    {
        int cur_ascii_val = (int)buffer[i];
        char cur_char = buffer[i];
        //printf("  %c[%d]%d  ", cur_char, cur_ascii_val, i);
        if(i == 0 )
        {
            if( parse_systemd::Is_Newline_Character(cur_char) )     // if first char is a newline, skip it
            {
                //buffer[i] = '\n';
                continue;
            }
        }

        if( cur_line_number == target_line_number && !seen_line_start)  // if we are at the desired line number in the buffer
        {
            index_line_start = i+1;
            seen_line_start = true;
        }

        if( parse_systemd::Is_Newline_Character(cur_char))
        {
            ++cur_line_number;    // increment the number of lines we've parsed through already
            
            if( cur_line_number == target_line_number && !seen_line_start)  // if we are at the desired line number in the buffer
            {
                index_line_start = i+1;
                seen_line_start = true;
            }

            if( cur_line_number == target_line_number+1)  // if we are at the desired line number in the buffer
            {
                    index_line_end = i-1;
                    break;
            }
        }
    }

    char* line = NULL;
    int substr_len = index_line_end - index_line_start + 1;
    if( substr_len >= 1)
    {
        line = StringH::Get_Substring(buffer, index_line_start, index_line_end);
    }
    return line;
}


bool parse_systemd::Is_Newline_Character(char character)
{
    bool is_newline = false;
    int ascii_value = (int)character;
    if(ascii_value == 10 || ascii_value == 11 || ascii_value == 12 || ascii_value == 13)
        is_newline = true;
    
    return is_newline;
}

int parse_systemd::Get_Num_Lines(char* buffer)
{
    int num_lines = 0;
    bool seen_normal_char = false;

    int len = strlen(buffer);
    for(int i=0; i< len; ++i)
    {
        char cur_char = buffer[i];

        if( parse_systemd::Is_Newline_Character(cur_char) )
        {
            ++num_lines;    // increment the number of lines we've parsed through already
        }
    }
    return num_lines;
}

bool parse_systemd::Service_Is_Active(const char* msg_str)
{
    // line #3 token #2 == active
    char* line3 = parse_systemd::Get_nth_Line(msg_str, 3);
    char* token2 = StringH::Parse_Second_Token(line3);
    if( StringH::Strings_Match(token2, parse_systemd::STATUS_ACTIVE) )
    {
        free(line3);
        free(token2);
        return true;
    }
    else
    {
        free(line3);
        free(token2);
        return false;
    }
}

bool parse_systemd::Service_Is_Loaded(const char* msg_str)
{
    // line #2 token #2 == loaded
    char* line2 = parse_systemd::Get_nth_Line(msg_str, 2);
    char* token2 = StringH::Parse_Second_Token(line2);
    if( StringH::Strings_Match(token2, parse_systemd::STATUS_LOADED) )
    {
        free(line2);
        free(token2);
        return true;
    }
    else
    {
        free(line2);
        free(token2);
        return false;
    }
}

bool parse_systemd::Service_Is_Running(const char* msg_str)
{

        // line #3 token #3 == running
    char* line3 = parse_systemd::Get_nth_Line(msg_str, 3);
    char* token3 = StringH::Parse_Third_Token(line3);
    bool is_running = StringH::Strings_Match(token3, parse_systemd::STATUS_RUNNING);
    free(line3);
    free(token3);
    return is_running;
}

bool parse_systemd::Service_Is_Exited(const char* msg_str)
{
    // line3, tokens 3: (exited)
    char* line3 = parse_systemd::Get_nth_Line(msg_str, 3);
    char* token3 = StringH::Parse_Third_Token(line3);
    bool is_exited = StringH::Strings_Match(token3, parse_systemd::STATUS_EXITED);
    free(line3);
    free(token3);
    return is_exited;
}

bool parse_systemd::Service_Is_Inactive(const char* msg_str)
{
    // line3, tokens 2: inactive
    char* line3 = parse_systemd::Get_nth_Line(msg_str, 3);
    char* token2 = StringH::Parse_Second_Token(line3);
    bool is_inactive = StringH::Strings_Match(token2, parse_systemd::STATUS_INACTIVE);
    free(line3);
    free(token2);
    return is_inactive;
}

bool parse_systemd::Service_Is_Failed(const char* msg_str)
{
    bool is_failed = false;
    // if lists failed anywhere on lines 3 or 4
    char* line2 = parse_systemd::Get_nth_Line(msg_str, 2);
    char* line3 = parse_systemd::Get_nth_Line(msg_str, 3);

    if( StringH::String_Contains(line2, STATUS_FAILED) || StringH::String_Contains(line3, STATUS_FAILED) )
        is_failed = true;
    
    free(line2);
    free(line3);
    return is_failed;
}