#include <stdbool.h>

#include "char_handling.h"

bool char_in_interval(char c, int beg, int fin) {
    bool res = false;
    for (int i = beg; i <= fin; i++)
        if (c == i) res = true;
    return res;
}

bool is_decimal(char c) { return char_in_interval(c, 48, 57); }

bool is_hexadecimal_uppercase(char c) { return char_in_interval(c, 65, 70); }

bool is_hexadecimal_lowercase(char c) { return char_in_interval(c, 97, 102); }

bool wrong_hex(char c) { return !is_hexadecimal_lowercase(c) && !is_hexadecimal_uppercase(c) && !is_decimal(c); }

int hex_to_dec(char c) {
    if (is_decimal(c)) return c - 48;
    if (is_hexadecimal_uppercase(c)) return c - 65 + 10;
    if (is_hexadecimal_lowercase(c)) return c - 97 + 10;
}

bool is_a_newline(char c) { return (c == '\n'); }

bool is_a_zero(char c) { return (c == '0'); }