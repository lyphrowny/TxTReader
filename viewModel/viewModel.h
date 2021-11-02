#pragma once

#include "../global.h"
#include "../array/array.h"
#include "../dataModel/dataModel.h"

typedef enum tagWordWrap {
    NO_WRAP = 0,
    WRAP
} WORD_WRAP;

typedef struct tagViewModel {
    array *lineBreaks;
    size_t hPos, vPos,
            maxLines, maxLen;
    WORD_WRAP mode;
} viewModel;

RC vm_init(viewModel *vm, array *arr, dataModel *dm);
size_t __upperBinarySearch(array *arr, size_t l, size_t r, const ARRAY_DATATYPE *elem);
RC drawViewModel(viewModel *vm, dataModel *dm, ushort width);
void vm_free(viewModel *vm);