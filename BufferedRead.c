#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "BufferedRead.h"

void debug(const char *format, ...) {
#ifdef DEBUG
    va_list arglist;
    va_start (arglist, format);
    vprintf(format, arglist);
    va_end(arglist);
#endif // DEBUG
}

void readFile(char *filename, unsigned short buffSize, DataModel *dm) {
    FILE *file;

    if ((file = fopen(filename, "r")) == NULL) {
        debug("file cannot be opened\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    uint fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize == 0) {
        debug("empty file");
        return;
    }

    char *buff;
    // no need to allocate +1, since the iteration is done through fileSize,
    // the \0 doesn't play any role here
    if ((buff = calloc(fileSize, sizeof *buff)) == NULL) {
        debug("The memory wasn't allocated");
        return;
    }
    fread(buff, sizeof *buff, fileSize, file); // some error here as well
    fclose(file);

    // the very last line is not terminated by '\n'
    dm->numLines = 1;
    for (uint i = 0; i < fileSize; ++i)
        dm->numLines += (buff[i] == '\n');

    dm->maxLen = 0;
    if ((dm->lines = calloc(dm->numLines, sizeof *dm->lines)) == NULL) {
        debug("Cannot allocate memory");
        return;
    } else {
        uint bPos = 0, j = 0, lineSize;

        for (uint i = 0; i <= fileSize; ++i) {
            // to cover the case of the last line
            // out-of-bounds read won't happen due to "||" laziness
            if (i == fileSize || buff[i] == '\n') {
                lineSize = i - bPos + 1;
                if (dm->maxLen > lineSize)
                    dm->maxLen = lineSize;
                if ((dm->lines[j] = calloc(lineSize, sizeof *dm->lines[j])) != NULL) {
                    // remove the ending '\n', the line is ended with '\0' due to calloc
                    memmove(dm->lines[j++], &(buff[bPos]), lineSize - 1);
                    bPos = i + 1;
                }
            }
        }
    }
    free(buff);
}

void freeDM(DataModel *dm) {
    // can it even be null?
    if (!dm)
        return;

    for (uint i = 0; i < dm->numLines; i++) {
        free(dm->lines[i]);
    }
    free(dm->lines);

    dm->lines = NULL;
    dm->numLines = 0;
    dm->maxLen = 0;
};