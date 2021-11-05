#include <stddef.h>
#include <stdbool.h>
#include <malloc.h>
#include "viewModel.h"
#include "../dataModel/dataModel.h"

static inline bool _isValid(viewModel *vm) {
    return vm != NULL;
}

RC vm_init(viewModel *vm, array *arr, dataModel *dm) {
    if (!_isValid(vm) || dm == NULL) // FIXME guess, dm needs more checks
        return FAILURE;
//    array_copy(vm->lineBreaks, dm->lineBreaks);
//    if (arr == NULL) {
//        if ((vm->lineBreaks = malloc(sizeof *vm->lineBreaks)) == NULL)
//            return FAILURE;
//        array_init(vm->lineBreaks);
//    } else
//        vm->lineBreaks = arr;
    // in both cases it is going to be reallocated, why bother?
    if ((vm->lineBreaks = malloc(sizeof *vm->lineBreaks)) == NULL)
        return FAILURE;
    array_init(vm->lineBreaks);
    vm->maxLen = vm->maxLines = 0;
    vm->hPos = vm->vPos = 0;
    vm->mode = WRAP;
    return SUCCESS;
}

size_t __upperBinarySearch(array *arr, size_t l, size_t r, const ARRAY_DATATYPE *elem) {
    size_t m;
    ARRAY_DATATYPE curr;
    array_takeAt(arr, 0, &curr);
    if (*elem <= curr)
        return 0;

    while (l <= r) {
        m = l + ((r - l) >> 1);
        array_takeAt(arr, m, &curr);
        if (curr >= *elem)
            r = m - 1;
        else
            l = m + 1;
    }
    return l;
}

static RC _getLineNum(viewModel *vm, array *lineBreaks, size_t *lineNum) {
    // absolute position in the buffer
    ARRAY_DATATYPE currPos;
    defRC
    // getting the ending position of the line
    checkRC(array_takeAt(vm->lineBreaks, vm->vPos, &currPos))

    // the number of line, in which the symbol is located according to the original lines (those in the data buffer)
    // -1 is because the endings of the lines are stored, so the first pos of the line is arr[lineNum - 1] + 1
    // TODO micro optimisation of the starting boundaries
    // finding the boundary of the line in the new model (whichever it is)
    // orig lineBreaks 60
    // now lineBreaks  10 20 30 40 50 60
    //                       *
    // #1 lineBreaks   12 24 36 48 60
    //                    ^  * // 30 is bounded by 36 and 36 is the end of the line, hence the start is from 24 + ?1
    // FIXME should be just plain array, not necessarily the dm's one
    *lineNum = __upperBinarySearch(lineBreaks, 0, lineBreaks->size, &currPos) - 1;
    return SUCCESS;
}

// TODO merge into one func? ^
static RC _getAbsolutePos(viewModel *vm, dataModel *dm, array* lineBreaks, size_t *pos) {
    size_t lineNum;
    ARRAY_DATATYPE lineEnding;
    defRC
    checkRC(_getLineNum(vm, lineBreaks, &lineNum))
    // FIXME what if lineNum is -1?
    checkRC(array_takeAt(lineBreaks, lineNum, &lineEnding))
    *pos = lineEnding + (dm->buff[lineEnding] == '\r' || dm->buff[lineEnding] == ' ');
    return SUCCESS;
}

static void _remodelViewModel(viewModel *vm, dataModel *dm, ushort width) {
    // clear the array since the new model is eager to take its place
    // TODO prealloc with at least the size of dm->lineBreaks
    array_free(vm->lineBreaks);

    ARRAY_DATATYPE prev = 0, curr;
    bool flag = false;
    for (size_t i = 0; i < dm->lineBreaks->size; ++i) {
        array_takeAt(dm->lineBreaks, i, &curr);
        // while the line is larger keep splitting
        while (curr - prev > width) {
            flag = false;
            // start looking for the place to break the line from its end
            for (ARRAY_DATATYPE j = prev + width; j > prev; --j) {
                if (dm->buff[j] == ' ') {
                    array_append(vm->lineBreaks, j);
                    prev = j + 1;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                // break so that the word of length `width` was fit on the line
                array_append(vm->lineBreaks, prev + width);
                // the letter on which the break was caused shouldn't be discarded,
                // hence no +1
                prev += width;
            }
        }
        // yay, the rest of the line is short enough (even the original \n is preserved)
        array_append(vm->lineBreaks, curr);
        prev = curr;
    }
}

// TODO merge into one func? ^
static void _restoreViewModel(viewModel *vm, dataModel *dm) {
    // TODO convert position

    vm->maxLen = dm->maxLen;
    // get the original line breaks back
    array_copy(vm->lineBreaks, dm->lineBreaks);
}

RC buildViewModel(viewModel* vm, dataModel* dm, size_t* posToPreserve) {
    switch (vm->mode) {
        case WRAP:
            _getAbsolutePos(vm, dm, vm->lineBreaks, posToPreserve);
            _remodelViewModel(vm, dm, vm->maxLen);
            break;
        case NO_WRAP:
            _getAbsolutePos(vm, dm, dm->lineBreaks, posToPreserve);
            _restoreViewModel(vm, dm);
            break;
        default:
            return FAILURE;
    }
    return SUCCESS;
}

RC resizeViewModel(viewModel* vm, dataModel* dm, ushort width, ushort height) {
    vm->maxLen = width;
    size_t posToPreserve;
    defRC
    checkRC(buildViewModel(vm, dm, &posToPreserve))

    return SUCCESS;
}


RC drawViewModel(viewModel *vm, dataModel *dm, ushort width) {
    defRC
    // not the right place for that
//    vm->mode ^= 1;

//  all the operations on the buffer should happen
//  before the drawing, so the following lines are
//  out of place
//    vm->maxLen = width;
//    buildViewModel(vm, dm);

    ARRAY_DATATYPE prev = 0, curr;
    for (size_t i = 0; i < vm->lineBreaks->size; ++i) {
        checkRC(array_takeAt(vm->lineBreaks, i, &curr))
        for (ARRAY_DATATYPE j = prev; j < curr; ++j) {
            if (dm->buff[j] != '\r')
                putchar(dm->buff[j]);
        }
        putchar('\n');
        prev = curr + (dm->buff[curr] == '\r' || dm->buff[curr] == ' ');
    }
    return SUCCESS;
}

void vm_free(viewModel *vm) {
    if (_isValid(vm)) {
        array_free(vm->lineBreaks);
        free(vm->lineBreaks);
    }
    vm->hPos = vm->vPos = 0;
    vm->maxLen = vm->maxLines = 0;
}