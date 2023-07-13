#include <stdint.h>
#include <stdlib.h>

#include "hash.h"
#include "iawls.h"

// Zestaw funkcji haszującej i odwrotnej do niej.

uint64_t rank(iawl dims, iawl coords) {
    uint64_t bit_number = coords.arr[0] - 1;
    for (uint64_t i = 1; i < dims.len; i++) {
        uint64_t multiplier = 1;
        for (uint64_t j = 0; j < i; j++) multiplier *= dims.arr[j];
        bit_number += ((coords.arr[i] - 1) * multiplier);
    }
    return bit_number;
}

iawl unrank(iawl dims, uint64_t bit_number) {
    uint64_t divider = 1;
    for (uint64_t i = 0; i < dims.len - 1; i++)  // do n(k-1)
        divider *= dims.arr[i];
    uint64_t* coords = malloc(sizeof(uint64_t) * dims.len);
    if (!coords) return iawl_dummy();
    for (uint64_t i = dims.len - 1; i < dims.len /*>= 0*/; i--) {
        coords[i] = (bit_number / divider) + 1;
        bit_number = bit_number % divider;
        if (i > 0) divider /= dims.arr[i - 1];
    }
    return create_iawl(coords, dims.len);
}
