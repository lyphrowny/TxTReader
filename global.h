#pragma once

#include "array/array.h"
#include "buffer/buffer.h"

typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#define REALLOC_COEFF 1.45
#define MIN_ALLOC 8

typedef enum tagReturnCodes {
    BAD_REALLOC = -2,
    BAD_ALLOC = -1,
    FAILURE = 0,
    SUCCESS = 1
} RC;