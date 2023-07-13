#include <stdint.h>
#include <stdlib.h>

#include "bitvectors.h"

// Zestaw funkcji tworzących i obsłgujących
// wektory jedno- i dwubitowe.

uint64_t* create_2bit_bitvector(uint64_t size) {
    uint64_t bit_size = (size + 31) / 32;
    uint64_t* res = malloc(bit_size * sizeof(uint64_t));
    if (!res) {
        return NULL;
    }
    for (int i = 0; i < bit_size; i++) res[i] = 0;
    return res;
}

uint64_t get_kth_2bit(uint64_t* bitvector, uint64_t k) {
    uint64_t chunk_index = (k >> 5);
    uint64_t bit_in_chunk_index = (k & 0x1F) << 1;
    return (bitvector[chunk_index] >> bit_in_chunk_index) & 3;
}

void set_kth_2bit_to_x(uint64_t* bitvector, uint64_t k, uint64_t x) {
    uint64_t chunk_index = (k >> 5);
    uint64_t bit_in_chunk_index = (k & 0x1F) << 1;
    uint64_t mask = 3;
    mask <<= bit_in_chunk_index;
    bitvector[chunk_index] &= (~mask);
    mask = x;
    mask <<= bit_in_chunk_index;
    bitvector[chunk_index] |= mask;
}

uint64_t* create_1bit_bitvector(uint64_t size) {
    uint64_t bit_size = (size + 63) / 64;
    uint64_t* res = malloc(bit_size * sizeof(uint64_t));
    if (!res) return NULL;
    for (int i = 0; i < bit_size; i++) res[i] = 0;
    return res;
}

uint64_t get_kth_1bit(uint64_t* bitvector, uint64_t k) {
    uint64_t chunk_index = (k >> 6);
    uint64_t bit_in_chunk_index = (k & 0x3F);
    return (bitvector[chunk_index] >> bit_in_chunk_index) & 1;
}

void set_kth_1bit_to_x(uint64_t* bitvector, uint64_t k, uint64_t x) {
    uint64_t chunk_index = (k >> 6);
    uint64_t bit_in_chunk_index = (k & 0x3F);
    uint64_t mask = 1;
    mask <<= bit_in_chunk_index;
    bitvector[chunk_index] &= (~mask);
    mask = x;
    mask <<= bit_in_chunk_index;
    bitvector[chunk_index] |= mask;
}