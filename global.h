#pragma once

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef char buffer;

#define REALLOC_COEFF 1.45
#define MIN_ALLOC 8


//void debug(const char *format, ...) {
//#ifdef DEBUG
//    va_list arglist;
//    va_start (arglist, format);
//    vprintf(format, arglist);
//    va_end(arglist);
//#endif // DEBUG
//}

#define fail(cond, message) if ((cond)) { \
    perror((message));                     \
    return FAILURE;\
}

typedef enum tagReturnCodes {
    BAD_REALLOC = -2,
    BAD_ALLOC = -1,
    FAILURE = 0,
    SUCCESS = 1
} RC;