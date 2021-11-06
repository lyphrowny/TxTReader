#pragma once

#include "../global.h"

#define FONT_HEIGHT 12
#define FONT_NAME "Courier New"

#define LINE_SPACING_COEFF 1.25


// TODO check the RC codes
//  add function for scaling the font (on Ctrl + +, for instance)
//  add function to change the line spacing

// add `line_spacing` param?
typedef struct tagFont {
    HFONT hFont;
    ushort chHeight, chWidth;
} font;

RC font_SetFont(HWND hwnd, font *f);
void font_Free(font* f);
RC font_GetHeight(font* f, ushort* height);
RC font_GetWidth(font* f, ushort* length);
