#include <stdio.h>
#include <malloc.h>
#include "BufferedRead.h"
#include "array/array.h"
#include "viewModel/viewModel.h"

int main() {
    char *filename = "+.txt";
    dataModel dm = {NULL, NULL};
    if (readFile(filename, &dm) == FAILURE)
        perror("Oops, didn't mean to");

    for (size_t i = 0; i < dm.lineBreaks->size; ++i) {
        printf("%d ", dm.lineBreaks->data[i]);
    }
    printf("\n");

    viewModel vm = {NULL, 0, 0, 0, 0, WRAP};
    vm_init(&vm, NULL, &dm);
    vm.mode = NO_WRAP;
    drawViewModel(&vm, &dm, 40);

    dm_free(&dm);
    vm_free(&vm);
//    array *arr = malloc(sizeof *arr);
//    defRC
//    checkRC(array_init(arr));
//    for (size_t i = 0; i < 100; ++i) {
//        if (i % 3 == 0 && i > 0) {
//            checkRC(array_append(arr, i))
//            printf("%zu ", i);
//        }
//    }
//    printf("\n");
//    ARRAY_DATATYPE elem = 0;
//    for (; elem < 103; ++elem) {
//        size_t pos = __upperBinarySearch(arr, 0, 32, &elem);
//        if (pos < 33)
//        printf("e:%u p:%ld a:%u\n", elem, pos, arr->data[pos]);
//    }
//    array_free(arr);
//    free(arr);

}