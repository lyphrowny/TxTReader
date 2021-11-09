#pragma once

#include "../global.h"
#include "../array/array.h"
#include "../dataModel/dataModel.h"
#include "../font/font.h"

typedef enum tagWordWrap {
    NO_WRAP = 0,
    WRAP
} WORD_WRAP;

typedef struct tagViewModel {
    array *lineBreaks;
    size_t hPos, vPos,
            maxLines, maxChars;
    WORD_WRAP mode;
} viewModel;

RC vm_init(viewModel *vm);
void vm_changeViewMode(viewModel* vm);
RC vm_resizeViewModel(HWND hwnd, viewModel* vm, dataModel* dm, ushort width, ushort height);
RC vm_drawViewModel(HDC hdc, viewModel *vm, dataModel *dm, font* f);
void vm_free(viewModel *vm);
