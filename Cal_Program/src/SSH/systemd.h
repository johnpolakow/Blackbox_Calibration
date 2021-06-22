
#ifndef _SYSTEMD_HELPER
#define _SYSTEMD_HELPER


#include "StringH.h"

class parse_systemd
{
public:
    parse_systemd();

    static bool Service_Is_Active(const char* msg_str);
    static bool Service_Is_Loaded(const char* msg_str);
    static bool Service_Is_Running(const char* msg_str);
    static bool Service_Is_Exited(const char* msg_str);
    static bool Service_Is_Inactive(const char* msg_str);
    static bool Service_Is_Failed(const char* msg_str);
    static char* Get_nth_Line(const char* buffer, int target_line_number);
    static int Get_Num_Lines(char* buffer);

    static const char* STATUS_ACTIVE;
    static const char* STATUS_ENABLED;
    static const char* STATUS_LOADED;
    static const char* STATUS_RUNNING;
    static const char* STATUS_EXITED;
    static const char* STATUS_WAITING;
    static const char* STATUS_INACTIVE;
    static const char* STATUS_FAILED;

private:
    static bool Is_Newline_Character(char character);

};
// Blackberry Service States





#endif