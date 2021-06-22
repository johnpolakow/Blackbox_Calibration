#ifndef _STRUCT_DEFS_H
#define _STRUCT_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

static const int MAX_COMMANDS = 100;
#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )

#define ONE_MILLISECOND 1000
#define POPEN_READ "re"  // "r" opens for reading, "e" sets close on exec flag ("FD_CLOEXEC")

typedef struct pi_load_data_t
{
    double Load_V;
    double Load_mA;
}PI_Load_Point;



#ifdef __cplusplus
}
#endif


#endif
