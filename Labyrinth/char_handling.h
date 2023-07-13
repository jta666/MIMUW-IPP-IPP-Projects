#pragma once

#include <stdbool.h>

bool char_in_interval(char c, int beg, int fin);
bool is_decimal(char c);
bool is_hexadecimal_uppercase(char c);
bool is_hexadecimal_lowercase(char c);
bool wrong_hex(char c);
int hex_to_dec(char c);
bool is_a_newline(char c);
bool is_a_zero(char c);