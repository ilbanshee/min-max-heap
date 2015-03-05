#ifndef MINMAX_HEAP_H_
#define MINMAX_HEAP_H_

typedef struct heap {
  int* data;
  int count;
  int size;
} heap_t;

heap_t* mmh_init();
heap_t* mmh_init_with_size(int size);
void mmh_free(heap_t* h);

void mmh_dump(heap_t* h);
void mmh_insert(heap_t* h, int value);
int mmh_pop_min(heap_t* h);
int mmh_pop_max(heap_t* h);
int mmh_peek_min(heap_t* h);
int mmh_peek_max(heap_t* h);

#endif  // MINMAX_HEAP_H_
