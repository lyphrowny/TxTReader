#pragma once

#include "../global.h"

#define ARRAY_DATATYPE uint

typedef struct tagArray{
    ARRAY_DATATYPE* data;
    size_t size;
    size_t capacity;
} array;

RC array_init(array* arr);
RC array_append(array *arr, T num);
void array_free(array* arr);