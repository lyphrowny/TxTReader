#pragma once

#include "../global.h"
#include "../array/array.h"

typedef struct tagDataModel {
    buffer* buff;
    array* lineBreaks;
    ARRAY_DATATYPE maxLen;
} dataModel;

RC dm_init(dataModel *dm, size_t buffSize, array *arr);

void dm_free(dataModel *dm);

void dm_setMaxLen(dataModel *dm, size_t len);