#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "array_stuff.h"
#include "bitvectors.h"
#include "char_handling.h"
#include "constants.h"
#include "iawls.h"
#include "input_processing.h"

iawl transform_dec_to_bitvector(iawl wall_map, iawl dims, uint64_t size);
uint64_t* transform_hex_to_bitvector(char* hexes, uint64_t len);
iawl read_five_decimals(uint64_t desired_size, iawl dims);
iawl free_and_abort(char* arr);

iawl transform_dec_to_bitvector(iawl wall_map, iawl dims, uint64_t size) {
    uint64_t a = wall_map.arr[0];
    uint64_t b = wall_map.arr[1];
    uint64_t m = wall_map.arr[2];
    uint64_t r = wall_map.arr[3];
    uint64_t s0 = wall_map.arr[4];

    if (!m) return create_error_iawl(ERROR_CONSTANT);

    uint64_t* s = malloc(sizeof(uint64_t) * (r + 1));
    if (!s) return create_error_iawl(ERROR_CONSTANT);
    s[0] = s0;
    for (uint64_t i = 1; i <= r; i++) s[i] = (a * s[i - 1] + b) % m;

    uint64_t* w = malloc(sizeof(uint64_t) * (r + 1));
    if (!w) {
        free(s);
        return create_error_iawl(ERROR_CONSTANT);
    }
    uint64_t multi_dim_num = 1;
    for (uint64_t i = 0; i < dims.len; i++) multi_dim_num *= dims.arr[i];
    w[0] = 0;
    for (uint64_t i = 1; i <= r; i++) {
        w[i] = s[i] % multi_dim_num;
    }
    free(s);

    uint64_t* bit_arr = create_1bit_bitvector(size);
    if (!bit_arr) {
        free(w);
        return create_error_iawl(ERROR_CONSTANT);
    }

    for (uint64_t i = 1; i <= r; i++) {
        uint64_t big_two_pow = pow(2, 32);
        uint64_t j = w[i];
        while (j <= size) {
            set_kth_1bit_to_x(bit_arr, j, 1);
            j += big_two_pow;
        }
    }

    free(w);
    return create_iawl(bit_arr, REDUNDANT);
}

// czyta czwartą linijkę w konfiguracji z 'R'
// i przetwarza ją na wektor bitowy
iawl read_five_decimals(uint64_t desired_size, iawl dims) {
    uint64_t* res_arr = malloc(sizeof(uint64_t) * DEC_LAST_LINE_LEN);
    if (!res_arr) return create_error_iawl(ERROR_CONSTANT);

    for (int i = 0; i < DEC_LAST_LINE_LEN; i++) {
        uint64_t x;
        scanf("%lld", &x);
        res_arr[i] = x;
        // printf ("AA: %d \n", x);
    }
    while (!feof(stdin)) getc(stdin);

    iawl raw_line = create_iawl(res_arr, DEC_LAST_LINE_LEN);
    if (is_error_iawl(raw_line)) return raw_line;

    iawl res = transform_dec_to_bitvector(raw_line, dims, desired_size);

    free_iawl(raw_line);
    return res;
}

uint64_t* transform_hex_to_bitvector(char* hexes, uint64_t len) {
    uint64_t* res = create_1bit_bitvector(len * BITS_FOR_ONE_HEX);
    uint64_t bit_ind = 0;

    for (int i = len - 1; i >= 0; i--) {
        uint64_t curr = hex_to_dec(hexes[i]);

        // konwertuje liczbę szesnastkową na cztery dwójkowe
        int binary[BITS_FOR_ONE_HEX] = {0, 0, 0, 0};
        for (int j = 0; j < BITS_FOR_ONE_HEX; j++) {
            int two_power = pow(2, BITS_FOR_ONE_HEX - 1 - j);
            if (curr >= two_power) {
                curr -= two_power;
                binary[j]++;
            }
        }

        for (int j = BITS_FOR_ONE_HEX - 1; j >= 0; j--) {
            set_kth_1bit_to_x(res, bit_ind, binary[j]);
            bit_ind++;
        }
    }

    return res;
}

iawl free_and_abort(char* arr) {
    free(arr);
    return create_error_iawl(ERROR_CONSTANT);
}

// czyta liczbę szesnastkową i przetwarza ją na wektor bitowy
iawl read_big_hexadecimal(uint64_t size) {
    uint64_t char_size = (size + BITS_FOR_ONE_HEX - 1) / BITS_FOR_ONE_HEX;
    int char_size_reminder = size % BITS_FOR_ONE_HEX;

    char* arr = malloc(sizeof(char) * char_size);
    if (!arr) return free_and_abort(arr);
    for (uint64_t i = 0; i < char_size; i++) arr[i] = 0;
    char temp;
    getc(stdin);  // pomija 'x'
    temp = getc(stdin);
    if (wrong_hex(temp))  // błąd - zły znak
        return free_and_abort(arr);
    else
        ungetc(temp, stdin);

    int leading_zeros = 0;
    while (true) {  // pomija wiodące zera
        temp = getc(stdin);
        if (is_a_newline(temp) && leading_zeros)
            // obsługuje przypadek, w którym liczba to zero
            // z zerami wiodącymi
            return create_iawl((uint64_t*)arr, char_size);
        if (!is_a_zero(temp)) {
            ungetc(temp, stdin);
            break;
        }
        leading_zeros++;
    }

    uint64_t ind = 0;
    while (true) {
        if (ind > char_size)  // błąd - liczba jest za duża
            return free_and_abort(arr);
        temp = getc(stdin);
        if (is_a_newline(temp) || feof(stdin)) break;
        if (wrong_hex(temp))  // błąd - zły znak
            return free_and_abort(arr);
        arr[ind++] = temp;
    }

    if (ind == char_size) {
        if (hex_to_dec(arr[0]) > pow(2, char_size_reminder) - 1)
            ;
        // błąd - liczba jest za duża
        // return free_and_abort(arr);
    }

    char* new_arr;
    if (ind < char_size) {  // znaków było za mało
        new_arr = malloc(sizeof(char) * char_size);
        if (!new_arr) return free_and_abort(arr);
        for (uint64_t i = 0; i < char_size; i++) new_arr[i] = 0;
        uint64_t ind2 = char_size - 1;
        while (ind > 0) {
            new_arr[ind2--] = arr[--ind];  // przepisujemy i uzupełniamy zerami
        }
        free(arr);
        arr = new_arr;
    }

    uint64_t* bit_arr = transform_hex_to_bitvector(arr, char_size);
    free(arr);
    return create_iawl(bit_arr, char_size + 1);
}

// sprawdza, z jaką wersją ostatniej linijki mamy do czynienia
// i wywyołuje odpowiednią funkcję
iawl read_last_line(uint64_t desired_size, iawl dims) {
    char check = getc(stdin);
    while (check != 'R' && check != '0' && !feof(stdin)) check = getc(stdin);

    if (check == 'R')
        return read_five_decimals(desired_size, dims);
    else if (check == '0')
        return read_big_hexadecimal(desired_size);
    else
        return iawl_dummy();
}

// wczytuje jedną z pierwszych trzech linijek wejścia
iawl get_numbers(size_t min_size) {
    if (feof(stdin))  // błąd - plik za szybko się skończył
        return create_error_iawl(ERROR_CONSTANT);

    size_t* arr = malloc(sizeof(size_t) * BASIC_ARRAY_LEN);
    if (!arr) return create_error_iawl(ERROR_CONSTANT);
    for (size_t i = 0; i < BASIC_ARRAY_LEN; i++) arr[i] = 0;
    size_t curr_len = 0;
    size_t max_len = BASIC_ARRAY_LEN;
    char temp;

    while (true) {  // pomija wiodące spacje
        temp = getc(stdin);
        if (!isspace(temp) || is_a_newline(temp)) break;
    }

    if (!is_decimal(temp) || is_a_zero(temp) || is_a_newline(temp)) {
        // błąd - w linijce jest coś innego niż liczba, albo nie ma nic
        // obsługuje również pojawienie się minusa
        free(arr);
        return create_error_iawl(ERROR_CONSTANT);
    }

    ungetc(temp, stdin);

    while (true) {
        if (curr_len >= max_len) {  // jeśli nie ma miejsca, to powiększa tablicę
            size_t new_len = more(max_len);
            size_t* temp2 = realloc(arr, sizeof(size_t) * new_len);
            if (!temp2) {
                free(arr);
                return create_error_iawl(ERROR_CONSTANT);
            }
            arr = temp2;
            max_len = new_len;
        }

        int check = scanf("%llu", &arr[curr_len++]);
        if (!check || arr[curr_len - 1] == -1) {
            // jakaś liczba miała na końcu śmieci, albo była za duża
            char x = getc(stdin);
            free(arr);
            if (isspace(x))  // była za duża
                return create_error_iawl(ERROR_CONSTANT2);
            else
                return create_error_iawl(ERROR_CONSTANT);
        }

        while (true) {  // pomija wszystko co jest białym znakiem, ale nie newline'm
            temp = getc(stdin);
            if (is_a_newline(temp)) break;
            if (!isspace(temp)) break;
        }

        if (is_a_newline(temp)) break;

        if (!is_decimal(temp) || is_a_zero(temp)) {
            // błąd - w linijce jest coś innego niż liczba, albo 0;
            free(arr);
            return create_error_iawl(ERROR_CONSTANT);
        }

        ungetc(temp, stdin);
    }

    if (curr_len < min_size) {  // błąd - w linijce było za mało danych
        free(arr);
        return create_error_iawl(ERROR_CONSTANT);
    }

    for (size_t i = curr_len; i < max_len; i++) arr[i] = 0;

    return create_iawl(arr, curr_len);
}