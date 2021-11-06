#include "font.h"


static inline bool _isValid(font *f) {
    return f != NULL;
}

RC font_SetFont(HWND hwnd, font *f) {
    font_Free(f);

    HDC hdc = GetDC(hwnd);
    // the default mapping mode is MM_TEXT, so the -MulDiv is not useless
    f->hFont = CreateFont(-MulDiv(FONT_HEIGHT, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, FF_MODERN,
                      FONT_NAME);
    fail(!f->hFont, "couldn't create font")
    // set the newly created font to the window
    SelectObject(hdc, f->hFont);
    SetBkMode(hdc, TRANSPARENT);

    TEXTMETRIC tm;
    if (!GetTextMetrics(hdc, &tm)) {
        font_Free(f);
        return FAILURE;
    }
    f->chHeight = tm.tmHeight + tm.tmExternalLeading;
    f->chWidth = tm.tmMaxCharWidth;

    debug("%ld %ld %ld _%c_\n", tm.tmHeight, f->chHeight, f->chWidth, tm.tmBreakChar);

    ReleaseDC(hwnd, hdc);
    return SUCCESS;
}

void font_Free(font *f) {
    if (_isValid(f) && f->hFont != NULL)
        DeleteObject(f->hFont);
}

RC font_GetHeight(font *f, ushort *height) {
    if (!_isValid(f))
        return FAILURE;
    *height = f->chHeight;
    return SUCCESS;
}

RC font_GetWidth(font *f, ushort *width) {
    if (!_isValid(f))
        return FAILURE;
    *width = f->chWidth;
    return SUCCESS;
}