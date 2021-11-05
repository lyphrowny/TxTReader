#pragma once

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef char buffer;

#define REALLOC_COEFF 1.45
#define MIN_ALLOC 8

#define max(a, b) ((a) > (b) ? (a) : (b))

//void debug(const char *format, ...) {
//#ifdef DEBUG
//    va_list arglist;
//    va_start (arglist, format);
//    vprintf(format, arglist);
//    va_end(arglist);
//#endif // DEBUG
//}

typedef enum tagReturnCodes {
    OUT_OF_BOUNDS = -3,
    BAD_REALLOC = -2,
    BAD_ALLOC = -1,
    FAILURE = 0,
    SUCCESS = 1
} RC;

RC _perror(RC rc);

#define defRC RC rc;
#define checkRC(expr)  if ((rc = (expr)) != SUCCESS)              \
    return _perror(rc);           \


#define fail(cond, message) if ((cond)) { \
    perror((message));                     \
    return FAILURE;\
}

