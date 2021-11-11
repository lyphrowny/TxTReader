#include "docker.h"

#include "../bufferedRead/bufferedRead.h"

static inline bool _isValid(docker* doc) {
    return doc != NULL;
}

static RC _initInternals(HWND hwnd, docker* doc) {
    // should use calloc to avoid first font_free
    if ((doc->f = calloc(1, sizeof *doc->f)) == NULL)
        return BAD_ALLOC;
    font_setFont(hwnd, doc->f);
    if ((doc->dm = calloc(1, sizeof *doc->dm)) == NULL)
        return BAD_ALLOC;
    if ((doc->vm = calloc(1, sizeof *doc->vm)) == NULL)
        return BAD_ALLOC;

    doc->si.cbSize = sizeof doc->si;
    doc->isSailed = FALSE;
    return SUCCESS;
}

RC docker_init(HWND hwnd, docker** doc) {
    return (*doc = calloc(1, sizeof **doc)) == NULL ? BAD_ALLOC : _initInternals(hwnd, *doc);
}

ushort docker_getMinWidth(docker* doc) {
    return (_isValid(doc)) * font_getWidth(doc->f) * MIN_CHARS;
}

ushort docker_getMinHeight(docker* doc) {
    return (_isValid(doc)) * font_getHeight(doc->f) * MIN_LINES;
}

inline bool docker_isSailed(docker* doc) {
    return doc->isSailed;
}

RC docker_setSail(docker* doc, char* fileName) {
    checkRC(readFile(fileName, doc->dm))
    vm_init(doc->vm);
    doc->isSailed = TRUE;
    return SUCCESS;
}

static void _freeInternals(docker* doc) {
    font_free(doc->f);
    free(doc->f);
    dm_free(doc->dm);
    free(doc->dm);
    vm_free(doc->vm);
    free(doc->vm);
}

void docker_free(docker** doc) {
    _freeInternals(*doc);
    free(*doc);
}
