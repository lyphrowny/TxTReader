#include <stdio.h>
#include "global.h"

RC _perror(RC rc) {
    char *msg;
    switch (rc) {
        case OUT_OF_BOUNDS:
            msg = "Out of bounds";
            break;
        case BAD_REALLOC:
            msg = "bad reallocation";
            break;
        case BAD_ALLOC:
            msg = "bad allocation";
            break;
        case FAILURE:
            msg = "some sort of failure";
            break;
        default:
            msg = "unknown return code";
            break;
    }
    perror(msg);
    return rc;
}
