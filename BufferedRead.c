#include <stdio.h>

#include "BufferedRead.h"

RC readFile(char *filename, dataModel *dm) {
    defRC
    FILE *file;

    fail((file = fopen(filename, "r")) == NULL, "file cannot be opened")
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fail(fileSize == 0, "empty file")

    // FIXME what if we're reading another file?
    checkRC(dm_init(dm, fileSize + 1, NULL))
    fread(dm->buff, sizeof *dm->buff, fileSize, file);
    fclose(file);

    dm->buff[fileSize] = '\n';
    size_t prev = 0;
    for (size_t i = 0; i < fileSize + 1; ++i) {
        if (dm->buff[i] == '\n') {
            checkRC(array_append(dm->lineBreaks, i))
            dm->buff[i] = ' ';
            dm_setMaxLen(dm, i - prev);
            prev = i + 1;
        }
    }
//    checkRC(array_append(dm->lineBreaks, fileSize))
//    dm_setMaxLen(dm, fileSize - prev);
    return SUCCESS;
}
