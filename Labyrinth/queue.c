#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "array_stuff.h"
#include "constants.h"
#include "iawls.h"
#include "queue.h"

// Zestaw funkcji tworzących i obsługujących kolejkę FIFO.

queue* initialize_queue() {
    uint64_t* arr = malloc(BASIC_QUEUE_LEN * sizeof(uint64_t));
    if (!arr) return NULL;
    uint64_t len = BASIC_QUEUE_LEN;
    uint64_t beg = 0;
    uint64_t end = 0;
    queue* res = malloc(sizeof(queue));
    if (!res) {
        free(arr);
        return NULL;
    }
    res->arr = arr;
    res->len = len;
    res->end = end;
    res->beg = beg;
    return res;
}

void enlarge(queue* q) {
    uint64_t new_len = more(q->len);
    uint64_t* new_arr = realloc(q->arr, new_len * sizeof(uint64_t));
    q->len = new_len;
    q->arr = new_arr;
}

bool add(uint64_t x, queue* q) {
    if (q->end == q->len) enlarge(q);
    if (!q)  // obsługuje próbę zaalokowania zbyt dużej ilości pamięci
        return false;
    q->arr[q->end++] = x;
    return true;
}

uint64_t pop(queue* q) { return q->arr[q->beg++]; }

bool is_empty(queue* q) { return (q->end == q->beg); }

void destroy_queue(queue* q) {
    free(q->arr);
    free(q);
}
