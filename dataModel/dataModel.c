#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>
#include "dataModel.h"

static bool _isValid(dataModel *dm) {
    return dm != NULL;
}

RC dm_init(dataModel *dm, size_t buffSize, array *arr) {
    fail(!_isValid(dm) || (dm->buff = malloc(buffSize * sizeof *dm->buff)) == NULL, "dm buff allocation")
    if (arr == NULL) {
        fail((dm->lineBreaks = malloc(sizeof *dm->lineBreaks)) == NULL, "dm->lineBreaks bad alloc")
        array_init(dm->lineBreaks);
    } else
        dm->lineBreaks = arr;
    dm->maxLen = 0;
    return SUCCESS;
}

void dm_free(dataModel *dm) {
    if (_isValid(dm)) {
        array_free(dm->lineBreaks);
        free(dm->lineBreaks);
        free(dm->buff);
    }
}

void dm_setMaxLen(dataModel *dm, size_t len) {
    if (dm->maxLen < len)
        dm->maxLen = len;
}
