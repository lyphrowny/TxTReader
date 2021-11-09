#pragma once

#include "../global.h"

#define ARRAY_DATATYPE uint
#define ARRAY_SIZETYPE uint

typedef struct tagArray{
    ARRAY_DATATYPE* data;
    ARRAY_SIZETYPE size;
    ARRAY_SIZETYPE capacity;
} array;

RC array_init(array* arr);
RC array_prealloc(array* arr, size_t capacity);
RC array_append(array *arr, ARRAY_DATATYPE num);
RC array_takeAt(array *arr, size_t pos, ARRAY_DATATYPE *dest);
RC array_copy(array* dest, const array* src);
void array_free(array* arr);
