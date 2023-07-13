#pragma once
#include <stdbool.h>

typedef struct int_arr_with_len {
    uint64_t* arr;
    uint64_t len;
} iawl;

iawl create_iawl(uint64_t* _arr, uint64_t _len);
void free_iawl(iawl x);
iawl iawl_dummy();
iawl create_error_iawl(int error_code);
bool is_error_iawl(iawl i);