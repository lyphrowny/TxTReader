#include <stdio.h>
#include <malloc.h>
#include "BufferedRead.h"
#include "array/array.h"

int main() {
    char *filename = "+.txt";
    DataModel dm;
    readFile(filename, 0, &dm);

    for (unsigned int i = 0; i < dm.numLines; i++)
        printf("%s", dm.lines[i]);

    freeDM(&dm);

    array* arr = malloc(sizeof *arr);
    array_init(arr);
    for (int i = 0; i < 16; i++) {
        array_append(arr, i);
    }
    printf("%zu %zu", arr->size, arr->capacity);
    for (int i = 0; i < 16; i++) {
        printf("%d ", arr->data[i]);
    }

    array_free(arr);
    free(arr);
}