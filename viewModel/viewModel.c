#include <stddef.h>
#include <malloc.h>
#include "viewModel.h"
#include "../dataModel/dataModel.h"

static inline bool _isValid(viewModel *vm) {
    return vm != NULL;
}

RC vm_init(viewModel *vm) {
    fail(!_isValid(vm), "dm not valid")
    fail((vm->lineBreaks = malloc(sizeof *vm->lineBreaks)) == NULL, "bad alloc for vm->lineBreaks")
    array_init(vm->lineBreaks);

    vm->maxChars = vm->maxLines = 0;
    vm->hPos = vm->vPos = 0;
    vm->mode = WRAP;
    return SUCCESS;
}

size_t __upperBinarySearch(array *arr, size_t l, size_t r, const ARRAY_DATATYPE *elem) {
    size_t m;
    ARRAY_DATATYPE curr;
    checkRC(array_takeAt(arr, 0, &curr))
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

static RC _getLineNum(viewModel *vm, size_t *lineNum) {
    // TODO micro optimization of the starting boundaries
    // finding the boundary of the line in the new model (whichever it is)
    // pos 24
    // lineBreaks  10 20 30 40 50 60
    //                  *
    // upperBinarySearch will return the index of the upper bound, 2 (index starts from 0 (duh)) for 24
    // but the drawing func starts not from the value of index 0, but from the 0 itself,
    // therefore all the indexes are shifted to the left, due to the "virtual" zero
    *lineNum = __upperBinarySearch(vm->lineBreaks, 0, vm->lineBreaks->size, lineNum);
    return SUCCESS;
}

static inline size_t _getActualLineStart(dataModel* dm, size_t pos) {
    return pos + (dm->buff[pos] == ' ');
}

static RC _getAbsolutePos(viewModel *vm, dataModel *dm, size_t *pos) {
    ARRAY_DATATYPE lineEnding = 0;
    // no lines, nothing to do
    if (!vm->lineBreaks->size) {
        *pos = 0;
        return SUCCESS;
    }
    if (vm->vPos)
        checkRC(array_takeAt(vm->lineBreaks, vm->vPos - 1, &lineEnding))
    *pos = _getActualLineStart(dm, lineEnding);
    checkRC(array_takeAt(vm->lineBreaks, vm->vPos, &lineEnding))
    // add either the horizontal scroll pos or the length of the line
    *pos += min(vm->hPos, lineEnding - *pos);
    return SUCCESS;
}

static void _remodelViewModel(viewModel *vm, dataModel *dm) {
    // clear the array since the new model is eager to take its place
    array_free(vm->lineBreaks);
    array_prealloc(vm->lineBreaks, dm->lineBreaks->size);

    ARRAY_DATATYPE prev = 0, curr;
    bool flag = false;
    for (size_t i = 0; i < dm->lineBreaks->size; ++i) {
        array_takeAt(dm->lineBreaks, i, &curr);
        // while the line is larger keep splitting
        while (curr - prev > vm->maxChars) {
            flag = false;
            // start looking for the place to break the line from its end
            for (ARRAY_DATATYPE j = prev + vm->maxChars; j > prev; --j) {
                if (dm->buff[j] == ' ') {
                    array_append(vm->lineBreaks, j);
                    // one can use _getActualLineStart here
                    // but the case is trivial, since we wouldn't
                    // get here without the condition
                    prev = j + 1;
                    flag = true;
                    break;
                }
            }
            if (!flag) {
                // break so that the word of length `width` was fit on the line
                array_append(vm->lineBreaks, prev + vm->maxChars);
                // the letter on which the break was caused shouldn't be discarded,
                // hence no +1
                prev += vm->maxChars;
            }
        }
        // yay, the rest of the line is short enough (even the original \n is preserved)
        array_append(vm->lineBreaks, curr);
        // one can use _getActualLineStart here
        // but last character of the line is always advanced
        prev = curr + 1;
    }
}

static inline void _restoreViewModel(viewModel *vm, dataModel *dm) {
    // get the original line breaks back
    array_copy(vm->lineBreaks, dm->lineBreaks);
}

inline void vm_changeViewMode(viewModel* vm) {
    if (_isValid(vm))
        vm->mode ^= 1;
}

RC vm_buildViewModel(HWND hwnd, viewModel* vm, dataModel* dm) {

    SCROLLINFO si;
    si.cbSize = sizeof si;
    size_t posToPreserve;

    checkRC(_getAbsolutePos(vm, dm, &posToPreserve))

    switch (vm->mode) {
        case WRAP:
            _remodelViewModel(vm, dm);
            checkRC(_getLineNum(vm, &posToPreserve))
            vm->hPos = 0;

            // to automatically hide it
            si.nMax = 0;
            si.nPage = si.nMax + 1;
            break;
        case NO_WRAP:
            _restoreViewModel(vm, dm);
            checkRC(_getLineNum(vm, &posToPreserve))

            si.nMax = dm->maxLen - 1;
            si.nPage = vm->maxChars;
            break;
        default:
            return FAILURE;
    }
    vm->vPos = posToPreserve;

    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nPos = vm->hPos;
    // it calls WM_PAINT because adding the scroll bar to the
    // client area changes the size of the client area
    SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

    si.nMax = vm->lineBreaks->size - 1;
    si.nPage = vm->maxLines;
    si.nPos = vm->vPos;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

    return SUCCESS;
}

RC vm_resizeViewModel(HWND hwnd, viewModel* vm, dataModel* dm, ushort width, ushort height) {
    vm->maxLines = height;
    vm->maxChars = width;

    debug("w %ld, h %ld\n", width, height);
    checkRC(vm_buildViewModel(hwnd, vm, dm))
    InvalidateRect(hwnd, NULL, TRUE);

    return SUCCESS;
}

RC vm_drawViewModel(HDC hdc, viewModel *vm, dataModel *dm, font* f) {
    ARRAY_DATATYPE prev, curr = 0;
    debug("%d\n", vm->vPos);
    if (vm->vPos)
        checkRC(array_takeAt(vm->lineBreaks, vm->vPos - 1, &curr))
    prev = _getActualLineStart(dm, curr);
    for (size_t i = vm->vPos; i < min(vm->vPos + vm->maxLines, vm->lineBreaks->size); ++i) {
        checkRC(array_takeAt(vm->lineBreaks, i, &curr))
        TextOut(hdc, 0, f->chHeight * (i - vm->vPos), &dm->buff[prev + vm->hPos], curr - (prev + vm->hPos));
        prev = _getActualLineStart(dm, curr);
    }
    return SUCCESS;
}

void vm_free(viewModel *vm) {
    if (_isValid(vm)) {
        array_free(vm->lineBreaks);
        free(vm->lineBreaks);
    }
    vm->hPos = vm->vPos = 0;
    vm->maxChars = vm->maxLines = 0;
}
