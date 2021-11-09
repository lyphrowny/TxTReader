#include <stdio.h>
#include <stdarg.h>

#include "global.h"

char* errorMsg[] = {"Out of bounds", "bad reallocation", "bad allocation", "some sort of failure", "", "unknown return code"};

void debug(const char* format, ...) {
#ifdef DEBUG
    va_list arglist;
    va_start (arglist, format);
    vprintf(format, arglist);
    va_end(arglist);
#endif // DEBUG
}


// init extern rc
RC rc;

void errorPrint(char* msg, const char* fileName, const char*funcName, const int lineNum) {
    fprintf(stderr, "%s on line %d in %s (%s)\n", msg, lineNum, funcName, fileName);
}

RC _perror(RC rc, const char* fileName, const char*funcName, const int lineNum) {
    if (rc > SUCCESS || rc < OUT_OF_BOUNDS)
        rc = SUCCESS + 1;
    char *msg = errorMsg[rc - OUT_OF_BOUNDS];

    if (strcmp(msg, "")) {
        errorPrint(msg, fileName, funcName, lineNum);
    }
    return rc;
}
