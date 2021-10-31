#include <stdio.h>
#include <malloc.h>
#include "BufferedRead.h"
#include "array/array.h"

int main() {
    char *filename = "+.txt";
    dataModel dm = {NULL, NULL};
    if (readFile(filename, &dm) == FAILURE)
        perror("Oops, didn't mean to");

    for (size_t i = 0; i < dm.lineBreaks->size; ++i){
        printf("%d ", dm.lineBreaks->data[i]);
    }
    dm_free(&dm);
}