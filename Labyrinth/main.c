#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bfs.h"
#include "constants.h"
#include "hash.h"
#include "iawls.h"
#include "input_processing.h"

void free_all_and_abort(iawl a, iawl b, iawl c, iawl d, int line_num);

void free_all_and_abort(iawl a, iawl b, iawl c, iawl d, int line_num) {
    free_iawl(a);
    free_iawl(b);
    free_iawl(c);
    free_iawl(d);
    fprintf(stderr, "ERROR %d\n", line_num);
    exit(0);
}

int main() {
    iawl dims = iawl_dummy();
    iawl beg_coords = iawl_dummy();
    iawl fin_coords = iawl_dummy();
    iawl wall_map = iawl_dummy();

    dims = get_numbers(0);

    if (is_error_iawl(dims)) {
        int error_code = (int)dims.len == ERROR_CONSTANT2 ? LINE0 : LINE1;
        free_all_and_abort(dims, beg_coords, fin_coords, wall_map, error_code);
    }

    uint64_t size = count_size(dims);

    if (!size)  // rozmiar okazał się za duży
        free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE0);

    beg_coords = get_numbers(dims.len);

    if (is_error_iawl(beg_coords))
        free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE2);

    fin_coords = get_numbers(dims.len);

    if (is_error_iawl(fin_coords))
        free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE3);

    wall_map = read_last_line(size, dims);

    if (is_error_iawl(wall_map))
        free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE4);

    getc(stdin);
    getc(stdin);
    if (!feof(stdin)) free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE5);

    if (!(wall_map.arr)) free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE4);
    if (!empty(rank(dims, beg_coords), wall_map))
        free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE4);

    if (!empty(rank(dims, fin_coords), wall_map))
        free_all_and_abort(dims, beg_coords, fin_coords, wall_map, LINE3);

    int check = 0;  // wyłapuje błędy powstałe w wyniku działania bfs
    uint64_t res = find_the_path_classic_bfs(dims, beg_coords, fin_coords, wall_map, size, &check);

    if (check == NO_WAY)
        printf("NO WAY\n");
    else if (check == ERROR_CONSTANT)
        fprintf(stderr, "ERROR 0\n");
    else
        printf("%ld\n", res);

    free(dims.arr);
    free(beg_coords.arr);
    free(fin_coords.arr);
    free(wall_map.arr);

    return 0;
}