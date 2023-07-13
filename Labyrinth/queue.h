#pragma once

typedef struct queue {
    uint64_t* arr;
    uint64_t len;
    uint64_t beg;
    uint64_t end;
} queue;

queue* initialize_queue();
void enlarge(queue* q);
bool add(uint64_t x, queue* q);
uint64_t pop(queue* q);
bool is_empty(queue* q);
void destroy_queue(queue* q);
