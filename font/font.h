#pragma once

#include "../global.h"

#define FONT_HEIGHT 16
#define FONT_NAME "Ubuntu Mono"
#define FONT_LOCATION "resources\\UbuntuMono-R.ttf"
// how many dirs to go up - 1 (for 4, max is ../../../)
#define MAX_DEPTH 4

#define LINE_SPACING_COEFF 1.75


// TODO
//  add function for scaling the font (on Ctrl + +, for instance)
//  add function to change the line spacing

// add `line_spacing` param?
typedef struct tagFont {
    HFONT hFont;
    ushort chHeight, chWidth;
    char* fontPath;
} font;

RC font_setFont(HWND hwnd, font *f);
void font_free(font* f);

ushort font_getHeight(font *f);
ushort font_getWidth(font *f);
