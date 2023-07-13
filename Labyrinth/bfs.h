#pragma once
#include "iawls.h"

uint64_t count_size(iawl dims);
bool empty(uint64_t bit_num, iawl wall_map);
bool fin(iawl coords, iawl fin_coords);
uint64_t find_the_path_classic_bfs(iawl dims, iawl beg_coords, iawl fin_coords, iawl wall_map, uint64_t size,
                                   int* check);