#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "minmax_heap.h"

#define is_min(n) ((log2_32(n) & 1) == 0)
#define parent(n) (n / 2)
#define first_child(n) (n * 2)
#define second_child(n) ((n * 2) + 1)

#define min(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
  })
#define max(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
  })

const int tab32[32] = {0,  9,  1,  10, 13, 21, 2,  29, 11, 14, 16,
                       18, 22, 25, 3,  30, 8,  12, 20, 28, 15, 17,
                       24, 7,  19, 27, 23, 6,  26, 5,  4,  31};

int log2_32(uint32_t value) {
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  return tab32[(uint32_t)(value * 0x07C4ACDD) >> 27];
}

static void swap(heap_t* h, int i, int j) {
  int tmp = h->prio[i];
  h->prio[i] = h->prio[j];
  h->prio[j] = tmp;

  void* dtmp = h->data[i];
  h->data[i] = h->data[j];
  h->data[j] = dtmp;
}

static void bubbleup_min(heap_t* h, int i) {
  int pp_idx = parent(parent(i));
  if (pp_idx <= 0) return;

  if (h->prio[i] < h->prio[pp_idx]) {
    swap(h, i, pp_idx);
    bubbleup_min(h, pp_idx);
  }
}

static void bubbleup_max(heap_t* h, int i) {
  int pp_idx = parent(parent(i));
  if (pp_idx <= 0) return;

  if (h->prio[i] > h->prio[pp_idx]) {
    swap(h, i, pp_idx);
    bubbleup_max(h, pp_idx);
  }
}

static void bubbleup(heap_t* h, int i) {
  int p_idx = parent(i);
  if (p_idx <= 0) return;

  if (is_min(i)) {
    if (h->prio[i] > h->prio[p_idx]) {
      swap(h, i, p_idx);
      bubbleup_max(h, p_idx);
    } else {
      bubbleup_min(h, i);
    }
  } else {
    if (h->prio[i] < h->prio[p_idx]) {
      swap(h, i, p_idx);
      bubbleup_min(h, p_idx);
    } else {
      bubbleup_max(h, i);
    }
  }
}

int index_max_child_grandchild(heap_t* h, int i) {
  int a = first_child(i);
  int b = second_child(i);
  int d = second_child(a);
  int c = first_child(a);
  int f = second_child(b);
  int e = first_child(b);

  int min_idx = -1;
  if (a <= h->count) min_idx = a;
  if (b <= h->count && h->prio[b] > h->prio[min_idx]) min_idx = b;
  if (c <= h->count && h->prio[c] > h->prio[min_idx]) min_idx = c;
  if (d <= h->count && h->prio[d] > h->prio[min_idx]) min_idx = d;
  if (e <= h->count && h->prio[e] > h->prio[min_idx]) min_idx = e;
  if (f <= h->count && h->prio[f] > h->prio[min_idx]) min_idx = f;

  return min_idx;
}

int index_min_child_grandchild(heap_t* h, int i) {
  int a = first_child(i);
  int b = second_child(i);
  int c = first_child(a);
  int d = second_child(a);
  int e = first_child(b);
  int f = second_child(b);

  int min_idx = -1;
  if (a <= h->count) min_idx = a;
  if (b <= h->count && h->prio[b] < h->prio[min_idx]) min_idx = b;
  if (c <= h->count && h->prio[c] < h->prio[min_idx]) min_idx = c;
  if (d <= h->count && h->prio[d] < h->prio[min_idx]) min_idx = d;
  if (e <= h->count && h->prio[e] < h->prio[min_idx]) min_idx = e;
  if (f <= h->count && h->prio[f] < h->prio[min_idx]) min_idx = f;

  return min_idx;
}

static void trickledown_max(heap_t* h, int i) {
  int m = index_max_child_grandchild(h, i);
  if (m <= -1) return;
  if (m > second_child(i)) {
    // m is a grandchild
    if (h->prio[m] > h->prio[i]) {
      swap(h, i, m);
      if (h->prio[m] < h->prio[parent(m)]) {
        swap(h, m, parent(m));
      }
      trickledown_max(h, m);
    }
  } else {
    // m is a child
    if (h->prio[m] > h->prio[i]) swap(h, i, m);
  }
}

static void trickledown_min(heap_t* h, int i) {
  int m = index_min_child_grandchild(h, i);
  if (m <= -1) return;
  if (m > second_child(i)) {
    // m is a grandchild
    if (h->prio[m] < h->prio[i]) {
      swap(h, i, m);
      if (h->prio[m] > h->prio[parent(m)]) {
        swap(h, m, parent(m));
      }
      trickledown_min(h, m);
    }
  } else {
    // m is a child
    if (h->prio[m] < h->prio[i]) swap(h, i, m);
  }
}

static void trickledown(heap_t* h, int i) {
  if (is_min(i)) {
    trickledown_min(h, i);
  } else {
    trickledown_max(h, i);
  }
}

void mmh_insert(heap_t* h, int priority, void* data) {
  assert(priority >= 0);
  h->count++;
  // check for realloc
  if (h->count + 1 == h->size) {
    // printf("realloc: %d, %d\n", h->count, h->size * 2);
    h->prio = realloc(h->prio, (h->size * 2) * sizeof(int));
    h->data = realloc(h->data, (h->size * 2) * sizeof(void*));
    h->size = h->size * 2;
  }
  h->prio[h->count] = priority;
  h->data[h->count] = data;
  bubbleup(h, h->count);
}

void* mmh_pop_min_data(heap_t* h) {
  if (h->count > 1) {
    void* rv = h->data[1];
    h->prio[1] = h->prio[h->count];
    h->data[1] = h->data[h->count];
    h->count--;
    trickledown(h, 1);
    if (h->count == h->size / 3) {
      // printf("realloc: %d, %d\n", h->count, h->size / 2);
      h->prio = realloc(h->prio, ((h->size / 2) + 1) * sizeof(int));
      h->data = realloc(h->data, ((h->size / 2) + 1) * sizeof(void*));
      h->size = (h->size / 2) + 1;
    }
    return rv;
  }

  if (h->count == 1) {
    h->count--;
    return h->data[1];
  }
  return NULL;
}

void* mmh_pop_max_data(heap_t* h) {
  if (h->count > 2) {
    int idx = 2;
    if (h->prio[2] < h->prio[3]) idx = 3;
    void* rv = h->data[idx];
    h->prio[idx] = h->prio[h->count];
    h->data[idx] = h->data[h->count];
    h->count--;
    trickledown(h, idx);
    if (h->count == h->size / 3) {
      // printf("realloc: %d, %d\n", h->count, h->size / 2);
      h->prio = realloc(h->prio, ((h->size / 2) + 1) * sizeof(int));
      h->data = realloc(h->data, ((h->size / 2) + 1) * sizeof(void*));
      h->size = (h->size / 2) + 1;
    }
    return rv;
  }

  if (h->count == 2) {
    h->count--;
    return h->data[2];
  }

  if (h->count == 1) {
    h->count--;
    return h->data[1];
  }
  return NULL;
}

void* mmh_peek_min_data(heap_t* h) {}

void* mmh_peek_max_data(heap_t* h) {}

int mmh_pop_min(heap_t* h) {
  if (h->count > 1) {
    int d = h->prio[1];
    h->prio[1] = h->prio[h->count--];
    trickledown(h, 1);
    if (h->count == h->size / 3) {
      // printf("realloc: %d, %d\n", h->count, h->size / 2);
      h->prio = realloc(h->prio, ((h->size / 2) + 1) * sizeof(int));
      h->data = realloc(h->data, ((h->size / 2) + 1) * sizeof(void*));
      h->size = (h->size / 2) + 1;
    }
    return d;
  }

  if (h->count == 1) {
    h->count--;
    return h->prio[1];
  }
  return -1;
}

int mmh_pop_max(heap_t* h) {
  if (h->count > 2) {
    int idx = 2;
    if (h->prio[2] < h->prio[3]) idx = 3;
    int d = h->prio[idx];
    h->prio[idx] = h->prio[h->count--];
    trickledown(h, idx);
    if (h->count == h->size / 3) {
      // printf("realloc: %d, %d\n", h->count, h->size / 2);
      h->prio = realloc(h->prio, ((h->size / 2) + 1) * sizeof(int));
      h->data = realloc(h->data, ((h->size / 2) + 1) * sizeof(void*));
      h->size = (h->size / 2) + 1;
    }
    return d;
  }

  if (h->count == 2) {
    h->count--;
    return h->prio[2];
  }

  if (h->count == 1) {
    h->count--;
    return h->prio[1];
  }
  return -1;
}

int mmh_peek_min(heap_t* h) {
  if (h->count > 0) {
    return h->prio[1];
  }
  return -1;
}

int mmh_peek_max(heap_t* h) {
  if (h->count > 2) {
    return max(h->prio[2], h->prio[3]);
  }
  if (h->count == 2) {
    return h->prio[2];
  }
  if (h->count == 1) {
    return h->prio[1];
  }
  return -1;
}

void mmh_dump(heap_t* h) {
  printf("count is %d, elements are:\n\t [", h->count);
  for (int i = 1; i <= h->count; i++) {
    printf(" %d ", h->prio[i]);
  }
  printf("]\n");
}

heap_t* mmh_init() { return mmh_init_with_size(50); }

heap_t* mmh_init_with_size(int size) {
  // first array element is wasted since 1st heap element is on position 1
  // inside the array i.e. => [0,(1),(2), ... (n)] so minimum viable size is 2
  size = size > 2 ? size : 2;
  heap_t* h = calloc(1, sizeof(heap_t));
  h->prio = calloc(size, sizeof(int));
  h->data = calloc(size, sizeof(void*));
  h->count = 0;
  h->size = size;

  return h;
}

void mmh_free(heap_t* h) {
  free(h->prio);
  free(h->data);
  free(h);
}
