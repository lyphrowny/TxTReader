#pragma once

#include "../global.h"

#include "../dataModel/dataModel.h"
#include "../viewModel/viewModel.h"
#include "../font/font.h"

typedef struct tagDocker {
    dataModel* dm;
    viewModel* vm;
    font* f;
    SCROLLINFO si;
    HINSTANCE hInstance;
    bool isSailed;
    ulong scrollLines;
    int deltaPerLine, accumDelta;
} docker;

RC docker_init(HWND hwnd, docker** doc);

ushort docker_getMinWidth(docker* doc);
ushort docker_getMinHeight(docker* doc);

bool docker_isSailed(docker* doc);
RC docker_setSail(docker* doc, char* fileName);

void docker_free(docker** doc);
