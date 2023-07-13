#include <stdint.h>
#include <stdio.h>

#include "array_stuff.h"

size_t more(size_t n) { return (3 * n / 2 + 1); }

void copy_arr(uint64_t* from, uint64_t* to, uint64_t len) {
    for (uint64_t i = 0; i < len; i++) {
        to[i] = from[i];
    }
}