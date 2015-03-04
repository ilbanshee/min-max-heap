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
  int tmp = h->root[i];
  h->root[i] = h->root[j];
  h->root[j] = tmp;
}

void bubbleup_min(heap_t* h, int i) {
  int ppi = parent(parent(i));
  if (ppi <= 0) return;

  if (h->root[i] < h->root[ppi]) {
    swap(h, i, ppi);
    bubbleup_min(h, ppi);
  }
}

void bubbleup_max(heap_t* h, int i) {
  int ppi = parent(parent(i));
  if (ppi <= 0) return;

  if (h->root[i] > h->root[ppi]) {
    swap(h, i, ppi);
    bubbleup_max(h, ppi);
  }
}

void bubbleup(heap_t* h, int i) {
  int parentIdx = parent(i);
  if (parentIdx <= 0) return;

  if (is_min(i)) {
    if (h->root[i] > h->root[parentIdx]) {
      swap(h, i, parentIdx);
      bubbleup_max(h, parentIdx);
    } else {
      bubbleup_min(h, i);
    }
  } else {
    if (h->root[i] < h->root[parentIdx]) {
      swap(h, i, parentIdx);
      bubbleup_min(h, parentIdx);
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

  int mini = -1;
  int count = h->count;
  if (a <= count) mini = a;
  if (b <= count && h->root[b] > h->root[mini]) mini = b;
  if (c <= count && h->root[c] > h->root[mini]) mini = c;
  if (d <= count && h->root[d] > h->root[mini]) mini = d;
  if (e <= count && h->root[e] > h->root[mini]) mini = e;
  if (f <= count && h->root[f] > h->root[mini]) mini = f;

  return mini;
}

int index_min_child_grandchild(heap_t* h, int i) {
  int a = first_child(i);
  int b = second_child(i);
  int c = first_child(a);
  int d = second_child(a);
  int e = first_child(b);
  int f = second_child(b);

  int mini = -1;
  int count = h->count;

  if (a <= count) mini = a;
  if (b <= count && h->root[b] < h->root[mini]) mini = b;
  if (c <= count && h->root[c] < h->root[mini]) mini = c;
  if (d <= count && h->root[d] < h->root[mini]) mini = d;
  if (e <= count && h->root[e] < h->root[mini]) mini = e;
  if (f <= count && h->root[f] < h->root[mini]) mini = f;

  return mini;
}

void bubbledown_max(heap_t* h, int i) {
  int m = index_max_child_grandchild(h, i);
  // debug_printf("max m: %d\n", m);
  if (m <= -1) return;
  if (m > second_child(i)) {
    // m is a grandchild
    if (h->root[m] > h->root[i]) {
      swap(h, i, m);
      if (h->root[m] < h->root[parent(m)]) {
        swap(h, m, parent(m));
      }
      bubbledown_max(h, m);
    }
  } else {
    // m is a child
    if (h->root[m] > h->root[i]) swap(h, i, m);
  }
}

void bubbledown_min(heap_t* h, int i) {
  int m = index_min_child_grandchild(h, i);
  if (m <= -1) return;
  // debug_printf("min m: %d\n", m);
  if (m > second_child(i)) {
    // m is a grandchild
    if (h->root[m] < h->root[i]) {
      swap(h, i, m);
      if (h->root[m] > h->root[parent(m)]) {
        swap(h, m, parent(m));
      }
      bubbledown_min(h, m);
    }
  } else {
    // m is a child
    if (h->root[m] < h->root[i]) swap(h, i, m);
  }
}

void bubbledown(heap_t* h, int i) {
  if (is_min(i)) {
    bubbledown_min(h, i);
  } else {
    bubbledown_max(h, i);
  }
}

void insert(heap_t* h, int value) {
  h->count++;
  // check for realloc
  if (h->count + 1 == h->size) {
    // debug_print("realloc: %d, %d\n", h->count, h->size * 2);
    h->root = realloc(h->root, (h->size * 2) * sizeof(int));
    h->size = h->size * 2;
  }
  h->root[h->count] = value;
  bubbleup(h, h->count);
}

int pop_min(heap_t* h) {
  int d = h->root[1];
  h->root[1] = h->root[h->count--];
  bubbledown(h, 1);

  return d;
}

int pop_max(heap_t* h) {
  if (h->count == 1) {
    h->count--;
    return h->root[1];
  }
  if (h->count == 2) {
    h->count--;
    return h->root[2];
  }

  int idx = 2;
  if (h->root[2] < h->root[3]) idx = 3;
  int d = h->root[idx];
  h->root[idx] = h->root[h->count--];
  bubbledown(h, idx);

  return d;
}

void dump(heap_t* h) {
  printf("count is %d: [", h->count);
  for (int i = 1; i <= h->count; i++) {
    printf(" %d ", h->root[i]);
  }
  printf("]\n");
}