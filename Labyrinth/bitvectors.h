#pragma once

uint64_t* create_2bit_bitvector(uint64_t size);
uint64_t get_kth_2bit(uint64_t* bitvector, uint64_t k);
void set_kth_2bit_to_x(uint64_t* bitvector, uint64_t k, uint64_t x);

uint64_t* create_1bit_bitvector(uint64_t size);
uint64_t get_kth_1bit(uint64_t* bitvector, uint64_t k);
void set_kth_1bit_to_x(uint64_t* bitvector, uint64_t k, uint64_t x);