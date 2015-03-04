#ifndef MINMAX_HEAP_H_
#define MINMAX_HEAP_H_

typedef struct heap {
  int* root;
  int count;
  int size;
} heap_t;

void insert(heap_t* h, int value);
int pop_min(heap_t* h);
int pop_max(heap_t* h);
void dump(heap_t* h);

#endif  // MINMAX_HEAP_H_