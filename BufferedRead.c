#include <stdio.h>

#include "BufferedRead.h"

RC readFile(char *filename, dataModel *dm) {
    FILE *file;

    fail((file = fopen(filename, "r")) == NULL, "file cannot be opened")
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fail(fileSize == 0, "empty file")

    // FIXME what if we're reading another file?
    fail(dm_init(dm, fileSize, NULL) != SUCCESS, "dataModel couldn't be initialized")
    fread(dm->buff, sizeof *dm->buff, fileSize, file);
    fclose(file);

    size_t prev = 0;
    for (size_t i = 0; i < fileSize; ++i) {
        if (dm->buff[i] == '\n') {
            fail(array_append(dm->lineBreaks, i) != SUCCESS, "value not appended")
            dm->buff[i] = '\0';
            dm_setMaxLen(dm, i - prev);
            prev = i + 1;
        }
    }
    fail(array_append(dm->lineBreaks, fileSize) != SUCCESS, "value not appended")
    dm_setMaxLen(dm, fileSize - prev);
    return SUCCESS;
}
