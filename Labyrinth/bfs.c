#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "array_stuff.h"
#include "bfs.h"
#include "bitvectors.h"
#include "constants.h"
#include "hash.h"
#include "iawls.h"
#include "input_processing.h"
#include "queue.h"

uint64_t count_size(iawl dims) {
    uint64_t res = 1;
    for (uint64_t i = 0; i < dims.len; i++)
        if (res * dims.arr[i] < res)
            return 0;
        else
            res *= dims.arr[i];
    return res;
}

bool empty(uint64_t bit_num, iawl wall_map) { return !get_kth_1bit(wall_map.arr, bit_num); }

iawl generate_neighbors(uint64_t rank, iawl dims) {
    uint64_t* arr = malloc(dims.len * 2 * sizeof(uint64_t));
    uint64_t multiplier = 1;
    uint64_t ind = 0;
    iawl curr = unrank(dims, rank);

    for (int i = 0; i < dims.len; i++) {
        if (curr.arr[i] < dims.arr[i]) arr[ind++] = rank + multiplier;
        if (curr.arr[i] > 1) arr[ind++] = rank - multiplier;
        multiplier *= dims.arr[i];
    }

    free_iawl(curr);
    return create_iawl(arr, ind);
}

bool fin(iawl coords, iawl fin_coords) {
    for (uint64_t i = 0; i < coords.len; i++)
        if (coords.arr[i] != fin_coords.arr[i]) return false;
    return true;
}

uint64_t one_dim_solve(iawl beg_coords, iawl fin_coords, iawl wall_map, int* check) {
    uint64_t a = beg_coords.arr[0];
    uint64_t b = fin_coords.arr[0];

    uint64_t beg = a <= b ? a : b;
    uint64_t fin = a <= b ? b : a;

    for (uint64_t i = beg; i < fin; i++)
        if (!empty(i, wall_map)) {
            *check = NO_WAY;
            return -1;
        }
    return fin - beg;
}

uint64_t find_the_path_classic_bfs(iawl dims, iawl beg_coords, iawl fin_coords, iawl wall_map, uint64_t size,
                                   int* check) {
    // bfs liczy odległość w taki sposób, że zapisuje dla każdego wierzchołka,
    // że jest on bardziej oddalony od początku, niż obecny,
    // a następnie zwiększa odległość, gdy na taki bardziej oddalony
    // napotyka wyjmując kolejne wierzchołki z kolejki

    if (dims.len == 1)  // specjalne rozwiązanie dla wymiaru 1
        return one_dim_solve(beg_coords, fin_coords, wall_map, check);

    if (fin(beg_coords, fin_coords)) return 0;

    uint64_t* bitvector = create_2bit_bitvector(size);
    if (!bitvector) {
        *check = ERROR_CONSTANT;
        return 0;
    }

    for (uint64_t i = 0; i < size; i++)
        if (empty(i, wall_map))
            set_kth_2bit_to_x(bitvector, i, NOT_VISITED);
        else
            set_kth_2bit_to_x(bitvector, i, WALL);

    queue* q = initialize_queue();
    uint64_t start_rank = rank(dims, beg_coords);
    add(start_rank, q);
    set_kth_2bit_to_x(bitvector, start_rank, VISITED1);

    int lvl = 0;
    int vis_state = VISITED1;

    while (!is_empty(q)) {
        uint64_t curr_bit = pop(q);
        iawl curr_coords = unrank(dims, curr_bit);

        int curr_vis_state = get_kth_2bit(bitvector, curr_bit);
        if (curr_vis_state != vis_state) {
            lvl++;
            vis_state = curr_vis_state;
        }

        if (fin(curr_coords, fin_coords)) {
            free(bitvector);
            free_iawl(curr_coords);
            destroy_queue(q);
            return lvl;
        }

        iawl successors = generate_neighbors(curr_bit, dims);
        for (int i = 0; i < successors.len; i++) {
            uint64_t curr_rank = successors.arr[i];
            if (get_kth_2bit(bitvector, curr_rank) == NOT_VISITED) {
                if (vis_state == VISITED1)  // zaznacza
                    set_kth_2bit_to_x(bitvector, curr_rank, VISITED2);
                else
                    set_kth_2bit_to_x(bitvector, curr_rank, VISITED1);

                if (!add(curr_rank, q)) {  // kolejka zrobiła się zbyt duża
                    destroy_queue(q);
                    free_iawl(curr_coords);
                    free_iawl(successors);
                    *check = ERROR_CONSTANT;
                    return 0;
                };
            }
        }
        free_iawl(curr_coords);
        free_iawl(successors);
    }

    free(bitvector);
    destroy_queue(q);
    *check = NO_WAY;
    return 0;
}