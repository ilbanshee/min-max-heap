#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

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
    _a < _b ? _a : _b;      \
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

void swap(heap_t* h, int i, int j) {
  int tmp = h->data[i];
  h->data[i] = h->data[j];
  h->data[j] = tmp;
}

void bubbleup_min(heap_t* h, int i) {
  int pp_idx = parent(parent(i));
  if (pp_idx <= 0) return;

  if (h->data[i] < h->data[pp_idx]) {
    swap(h, i, pp_idx);
    bubbleup_min(h, pp_idx);
  }
}

void bubbleup_max(heap_t* h, int i) {
  int pp_idx = parent(parent(i));
  if (pp_idx <= 0) return;

  if (h->data[i] > h->data[pp_idx]) {
    swap(h, i, pp_idx);
    bubbleup_max(h, pp_idx);
  }
}

void bubbleup(heap_t* h, int i) {
  int p_idx = parent(i);
  if (p_idx <= 0) return;

  if (is_min(i)) {
    if (h->data[i] > h->data[p_idx]) {
      swap(h, i, p_idx);
      bubbleup_max(h, p_idx);
    } else {
      bubbleup_min(h, i);
    }
  } else {
    if (h->data[i] < h->data[p_idx]) {
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
  if (b <= h->count && h->data[b] > h->data[min_idx]) min_idx = b;
  if (c <= h->count && h->data[c] > h->data[min_idx]) min_idx = c;
  if (d <= h->count && h->data[d] > h->data[min_idx]) min_idx = d;
  if (e <= h->count && h->data[e] > h->data[min_idx]) min_idx = e;
  if (f <= h->count && h->data[f] > h->data[min_idx]) min_idx = f;

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
  if (b <= h->count && h->data[b] < h->data[min_idx]) min_idx = b;
  if (c <= h->count && h->data[c] < h->data[min_idx]) min_idx = c;
  if (d <= h->count && h->data[d] < h->data[min_idx]) min_idx = d;
  if (e <= h->count && h->data[e] < h->data[min_idx]) min_idx = e;
  if (f <= h->count && h->data[f] < h->data[min_idx]) min_idx = f;

  return min_idx;
}

void trickledown_max(heap_t* h, int i) {
  int m = index_max_child_grandchild(h, i);
  if (m <= -1) return;
  if (m > second_child(i)) {
    // m is a grandchild
    if (h->data[m] > h->data[i]) {
      swap(h, i, m);
      if (h->data[m] < h->data[parent(m)]) {
        swap(h, m, parent(m));
      }
      trickledown_max(h, m);
    }
  } else {
    // m is a child
    if (h->data[m] > h->data[i]) swap(h, i, m);
  }
}

void trickledown_min(heap_t* h, int i) {
  int m = index_min_child_grandchild(h, i);
  if (m <= -1) return;
  if (m > second_child(i)) {
    // m is a grandchild
    if (h->data[m] < h->data[i]) {
      swap(h, i, m);
      if (h->data[m] > h->data[parent(m)]) {
        swap(h, m, parent(m));
      }
      trickledown_min(h, m);
    }
  } else {
    // m is a child
    if (h->data[m] < h->data[i]) swap(h, i, m);
  }
}

void trickledown(heap_t* h, int i) {
  if (is_min(i)) {
    trickledown_min(h, i);
  } else {
    trickledown_max(h, i);
  }
}

void insert(heap_t* h, int value) {
  assert(value >= 0);
  h->count++;
  // check for realloc
  if (h->count + 1 == h->size) {
    // printf("realloc: %d, %d\n", h->count, h->size * 2);
    h->data = realloc(h->data, (h->size * 2) * sizeof(int));
    h->size = h->size * 2;
  }
  h->data[h->count] = value;
  bubbleup(h, h->count);
}

int pop_min(heap_t* h) {
  if (h->count == 0) {
    return -1;
  }
  int d = h->data[1];
  h->data[1] = h->data[h->count--];
  trickledown(h, 1);

  if (h->count == h->size / 3) {
    // printf("realloc: %d, %d\n", h->count, h->size / 2);
    h->data = realloc(h->data, (h->size / 2) * sizeof(int));
    h->size = h->size / 2;
  }

  return d;
}

int pop_max(heap_t* h) {
  if (h->count == 0) {
    return -1;
  }
  if (h->count == 1) {
    h->count--;
    return h->data[1];
  }
  if (h->count == 2) {
    h->count--;
    return h->data[2];
  }

  int idx = 2;
  if (h->data[2] < h->data[3]) idx = 3;
  int d = h->data[idx];
  h->data[idx] = h->data[h->count--];
  trickledown(h, idx);

  if (h->count == h->size / 3) {
    // printf("realloc: %d, %d\n", h->count, h->size / 2);
    h->data = realloc(h->data, (h->size / 2) * sizeof(int));
    h->size = h->size / 2;
  }

  return d;
}

void dump(heap_t* h) {
  printf("count is %d, elements are:\n\t [", h->count);
  for (int i = 1; i <= h->count; i++) {
    printf(" %d ", h->data[i]);
  }
  printf("]\n");
}