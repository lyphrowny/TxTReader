#include <stdio.h>

#include "bufferedRead.h"

RC readFile(char *filename, dataModel *dm) {
    FILE *file;

    fail((file = fopen(filename, "rt")) == NULL, "file cannot be opened")
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fail(fileSize == 0, "empty file")

    dm_free(dm);
    checkRC(dm_init(dm, fileSize + 1, NULL))
    debug("%d", fileSize);
    // since the file is opened in text mode,
    // fread drops \r characters, hence the fileSize obtained
    // in ftell becomes incorrect (the buffer doesn't contain \r, yet it has the size as if it does)
    // with this in mind, fileSize here gets the number of read elements, which is the actual size of
    // the buffer
    fileSize = fread(dm->buff, sizeof *dm->buff, fileSize, file);
    fail(ferror(file), "error while freading")
    debug(" %d\n", fileSize);
    fclose(file);

    // to get the last lineBreak set
    // if the last line doesn't end with artificial \n,
    // there would be no record of its end in lineBreaks
    dm->buff[fileSize] = '\n';
    size_t prev = 0;
    for (size_t i = 0; i < fileSize + 1; ++i) {
        if (dm->buff[i] == '\n') {
            checkRC(array_append(dm->lineBreaks, i))
            // TODO remove and fix viewModel
             dm->buff[i] = ' ';
            dm_setMaxLen(dm, i - prev);
            prev = i + 1;
        }
    }
    return SUCCESS;
}
