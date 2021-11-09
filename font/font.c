// for PathCombine
#include <shlwapi.h>

#include "font.h"


static inline bool _isValid(font *f) {
    return f != NULL;
}

RC font_SetFont(HWND hwnd, font *f) {
    font_Free(f);

    char* buff;
    fail((buff = calloc(MAX_PATH, sizeof *buff)) == NULL || (f->fontPath = calloc(MAX_PATH, sizeof *f->fontPath)) == NULL,
         "cannot allocate memory for font path")

    GetModuleFileName(NULL, buff, MAX_PATH);
    PathRemoveFileSpec(buff);
    char* rPath = calloc(MAX_PATH, sizeof *rPath),
        * lookUpDir = calloc(MAX_PATH, sizeof *lookUpDir);
    for(int i = 0; i < MAX_DEPTH; i++) {
        for (int j = 0; j < i; j++)
            strcat(rPath, "..\\");
        strcat(rPath, FONT_LOCATION);
        PathCombine(lookUpDir, buff, rPath);
        if (AddFontResource(lookUpDir) != 0) {
            strcpy(f->fontPath, lookUpDir);
            break;
        }
        memset(rPath, 0, MAX_PATH);
    }
    free(buff);
    free(rPath);
    free(lookUpDir);

    failClean(*f->fontPath == '\0', font_Free(f), "font wasn't loaded")

    HDC hdc = GetDC(hwnd);
    // the default mapping mode is MM_TEXT, so the -MulDiv is not useless
    f->hFont = CreateFont(-MulDiv(FONT_HEIGHT, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, FF_MODERN,
                      FONT_NAME);
    fail(!f->hFont, "couldn't create font")
    // set the newly created font to the window
    SelectObject(hdc, f->hFont);
    SetBkMode(hdc, TRANSPARENT);

    OUTLINETEXTMETRIC *OutlineMetric;
    fail((OutlineMetric = malloc(sizeof *OutlineMetric)) == NULL, "OutlineMetric memalloc error")
    GetOutlineTextMetrics(hdc, sizeof *OutlineMetric, OutlineMetric);
    f->chHeight = (OutlineMetric->otmAscent + OutlineMetric->otmDescent + OutlineMetric->otmLineGap) * LINE_SPACING_COEFF;
    free(OutlineMetric);

    ABC abc;
    // FIXME for mono fonts only!
    GetCharABCWidths(hdc, 0, 0, &abc);
    f->chWidth = abc.abcC + abc.abcB + abc.abcA;

    debug("cH %ld cW %ld\n", f->chHeight, f->chWidth);
    ReleaseDC(hwnd, hdc);
    return SUCCESS;
}

void font_Free(font *f) {
    if (_isValid(f) && f->hFont != NULL) {
        DeleteObject(f->hFont);
        fail(RemoveFontResource(f->fontPath) == 0, "font wasn't unloaded");
        free(f->fontPath);
    }
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
