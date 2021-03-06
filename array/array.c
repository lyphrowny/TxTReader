#include <stdbool.h>
#include <stddef.h>
#include <malloc.h>
#include <memory.h>
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
        return BAD_REALLOC;
    }
    arr->data = _data;
    arr->capacity = _capacity;
    return SUCCESS;
}

static RC _array_alloc(array* arr, ARRAY_SIZETYPE capacity) {
    if (!_isValid(arr))
        return FAILURE;
    arr->size = 0;
    arr->capacity = max(MIN_ALLOC, capacity);
    return (arr->data = malloc(arr->capacity * sizeof *arr->data)) == NULL ? BAD_ALLOC : SUCCESS;
}

RC array_init(array *arr) {
    checkRC(_array_alloc(arr, MIN_ALLOC))
    return SUCCESS;
}

RC array_prealloc(array* arr, ARRAY_SIZETYPE capacity) {
    checkRC(_array_alloc(arr, capacity))
    return SUCCESS;
}

RC array_append(array *arr, ARRAY_DATATYPE num) {
    fail(!_hasSpace(arr) && _realloc(arr) != SUCCESS, "either not enough space or the realloc failed")
    // else we have either the success or available space
    arr->data[arr->size++] = num;
    return SUCCESS;
}

RC array_takeAt(array *arr, ARRAY_SIZETYPE pos, ARRAY_DATATYPE *dest) {
    if (pos < arr->size) {
        *dest = arr->data[pos];
        return SUCCESS;
    }
    return OUT_OF_BOUNDS;
}

RC array_copy(array* dest, const array* src) {
    array_free(dest);
    checkRC(array_prealloc(dest, src->size))
    memcpy(dest->data, src->data, src->size * sizeof *src->data);
    dest->size = src->size;
    return SUCCESS;
}

void array_free(array *arr) {
    if (_isValid(arr)) {
        free(arr->data);
        arr->data = NULL;
        arr->capacity = arr->size = 0;
    }
}
