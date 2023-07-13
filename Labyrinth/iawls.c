#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "iawls.h"

// Zestaw funkcji tworzących i obsługujących
// struktury składające się z tablic i ich długości.

// iawl - int array with length

iawl create_iawl(uint64_t* _arr, uint64_t _len) {
    iawl res;
    res.arr = _arr;
    res.len = _len;
    return res;
}

void free_iawl(iawl x) {
    if (x.arr) free(x.arr);
}

iawl iawl_dummy() {
    iawl res;
    res.arr = NULL;
    res.len = 0;
    return res;
}

iawl create_error_iawl(int error_code) {
    iawl res;
    res.arr = NULL;
    res.len = error_code;
    return res;
}

bool is_error_iawl(iawl i) { return !i.arr && i.len; }