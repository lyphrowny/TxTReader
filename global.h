#pragma once

#include <stdbool.h>
#include "windows.h"

#define DEBUG

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef char buffer;

#define REALLOC_COEFF 1.45
#define MIN_ALLOC 8

#define MIN_LINES 24
#define MIN_CHARS 80

//#define max(a, b) ((a) > (b) ? (a) : (b))
//#define min(a, b) ((a) > (b) ? (b) : (a))

typedef enum tagReturnCodes {
    OUT_OF_BOUNDS = -3,
    BAD_REALLOC = -2,
    BAD_ALLOC = -1,
    FAILURE = 0,
    SUCCESS = 1
} RC;

void debug(const char* format, ...);
//char* errorMsg[];
void errorPrint(char* msg, const char* fileName, const char*funcName, const int lineNum);
RC _perror(RC rc, const char* fileName, const char*funcName, const int lineNum);

extern RC rc;

#define checkRC(expr)  if ((rc = _perror((expr), __FILE__, __func__, __LINE__)) != SUCCESS) \
 return rc;

#define fail(cond, message) if ((cond)) {                         \
    errorPrint((message), __FILE__, __func__, __LINE__);          \
    return FAILURE;                                               \
}


#define failClean(cond, cleanupCode, message) if ((cond)) {       \
    (cleanupCode);                                                \
    errorPrint((message), __FILE__, __func__, __LINE__);          \
    return FAILURE;                                               \
}

