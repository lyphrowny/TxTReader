#include <stdbool.h>
#include <stddef.h>
#include <malloc.h>
#include "array.h"

static bool _isValid(array *arr) {
    return arr != NULL;
}

static bool _hasSpace(array *arr) {
    return _isValid(arr) && arr->size < arr->capacity;
}

static RC _realloc(array *arr) {
    ulong _capacity = (arr->capacity > MIN_ALLOC ? arr->capacity : MIN_ALLOC) * REALLOC_COEFF;
    ARRAY_DATATYPE *_data;
    if ((_data = realloc(arr->data, _capacity * sizeof *arr->data)) == NULL) {
        free(arr->data);
        perror("Error reallocating memory");
        return BAD_REALLOC;
    }
    arr->data = _data;
    arr->capacity = _capacity;
    return SUCCESS;
}

RC array_init(array* arr) {
    if (!_isValid(arr))
        return FAILURE;
    arr->size = 0;
    arr->capacity = MIN_ALLOC;
    return (arr->data = malloc(arr->capacity * sizeof *arr->data)) == NULL ? BAD_ALLOC : SUCCESS;
}

RC array_append(array *arr, ARRAY_DATATYPE num) {
    if (!_hasSpace(arr) && _realloc(arr) != SUCCESS)
        return FAILURE;
    // else we have either the success or available space
    arr->data[arr->size++] = num;
    return SUCCESS;
}

void array_free(array* arr) {
    if (_isValid(arr)) {
        free(arr->data);
        arr->data = NULL;
        arr->capacity = arr->size = 0;
        arr = NULL;
    }
}