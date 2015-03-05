#include <stdio.h>
#include <stdlib.h>
// included only for testing log2_32 against c99 floor of log2
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "minmax_heap.h"

/*
 * duplicated here only for testing purposes
 */
#define is_min(n) ((log2_32(n) & 1) == 0)
int log2_32(uint32_t value);
int index_min_child_grandchild(heap_t* h, int i);
int index_max_child_grandchild(heap_t* h, int i);

int compare_ints_min(const void* a, const void* b) {
  const int* da = (const int*)a;
  const int* db = (const int*)b;

  return (*da > *db) - (*da < *db);
}

int compare_ints_max(const void* a, const void* b) {
  const int* da = (const int*)a;
  const int* db = (const int*)b;

  return (*da < *db) - (*da > *db);
}

void t_down_max() {
  printf("Testing pop_max after sequential insert from 500k to 0...  ");
  heap_t* h = mmh_init();

  for (int i = 500000; i >= 0; i--) {
    mmh_insert(h, i);
  }

  int start = 500000;
  while (h->count != 0) {
    assert(start-- == mmh_pop_max(h));
  }

  mmh_free(h);
  printf("[OK]\n");
}

void t_down_min() {
  printf("Testing pop_min after sequential insert from 500k to 0...  ");
  heap_t* h = mmh_init();

  for (int i = 500000; i >= 0; i--) {
    mmh_insert(h, i);
  }

  int start = 0;
  while (h->count != 0) {
    assert(start++ == mmh_pop_min(h));
  }

  mmh_free(h);
  printf("[OK]\n");
}

void t_up_max() {
  printf("Testing pop_max after sequential insert from 0 to 500k...  ");
  heap_t* h = mmh_init();

  for (int i = 0; i <= 500000; i++) {
    mmh_insert(h, i);
  }

  int start = 500000;
  while (h->count != 0) {
    assert(start-- == mmh_pop_max(h));
  }

  mmh_free(h);
  printf("[OK]\n");
}

void t_up_min() {
  printf("Testing pop_mim after sequential insert from 0 to 500k...  ");
  heap_t* h = mmh_init();

  for (int i = 0; i <= 500000; i++) {
    mmh_insert(h, i);
  }

  int start = 0;
  while (h->count != 0) {
    assert(start++ == mmh_pop_min(h));
  }

  mmh_free(h);
  printf("[OK]\n");
}

void t_rand_data_max() {
  printf("Testing pop_max after 5000 random insert...                ");
  heap_t* h = mmh_init();

  srand(time(NULL));
  int randarray[5000];
  for (int i = 0; i < 5000; i++) {
    randarray[i] = rand() % 100;
    mmh_insert(h, randarray[i]);
  }

  qsort(randarray, 5000, sizeof(int), compare_ints_max);
  int index = 0;
  while (h->count != 0) {
    assert(randarray[index++] == mmh_pop_max(h));
  }
  assert(index == 5000);

  mmh_free(h);
  printf("[OK]\n");
}

void t_rand_data_min() {
  printf("Testing pop_min after 5000 random insert...                ");
  heap_t* h = mmh_init();

  srand(time(NULL));
  int randarray[5000];
  for (int i = 0; i < 5000; i++) {
    randarray[i] = rand() % 100;
    mmh_insert(h, randarray[i]);
  }

  qsort(randarray, 5000, sizeof(int), compare_ints_min);
  int index = 0;
  while (h->count != 0) {
    assert(randarray[index++] == mmh_pop_min(h));
  }
  assert(index == 5000);

  mmh_free(h);
  printf("[OK]\n");
}

void t_min_level() {
  printf("Testing level inside the heap and custom log2 function...  ");
  assert(is_min(1) == true);
  assert(is_min(2) == false);
  assert(is_min(3) == false);
  assert(is_min(4) == true);
  assert(is_min(7) == true);
  assert(is_min(8) == false);
  assert(is_min(15) == false);
  assert(is_min(16) == true);

  assert(log2_32(4) == floor(log2(4.0)));
  assert(log2_32(3) == floor(log2(3.0)));
  assert(log2_32(310409) == floor(log2(310409.0)));
  assert(log2_32(997563) == floor(log2(997563.0)));
  srand(time(NULL));
  for (int i = 0; i < 500; i++) {
    uint32_t tmp = (uint32_t)rand();
    assert(log2_32(tmp) == floor(log2((double)tmp)));
  }
  printf("[OK]\n");
}

void t_insert_data_max() {
  printf("Testing pop_max on small custom heap...                    ");
  heap_t* h = mmh_init();

  srand(time(NULL));
  int randarray[10] = {0, 4, 2, 9, 3, 7, 6, 8, 5, 1};
  for (int i = 0; i < 10; i++) {
    mmh_insert(h, randarray[i]);
  }
  qsort(randarray, 10, sizeof(int), compare_ints_min);
  int index = 9;
  while (h->count > 0) {
    assert(randarray[index--] == mmh_pop_max(h));
  }
  assert(index == -1);

  mmh_free(h);
  printf("[OK]\n");
}

void t_insert_data_min() {
  printf("Testing pop_min on small custom heap...                    ");
  heap_t* h = mmh_init();

  srand(time(NULL));
  int randarray[10] = {0, 4, 2, 9, 3, 7, 6, 8, 5, 1};
  for (int i = 0; i < 10; i++) {
    mmh_insert(h, randarray[i]);
  }

  qsort(randarray, 10, sizeof(int), compare_ints_min);
  int index = 0;
  while (h->count > 0) {
    assert(randarray[index++] == mmh_pop_min(h));
  }
  assert(index == 10);

  mmh_free(h);
  printf("[OK]\n");
}

void t_min_index() {
  printf("Testing min index calculation...                           ");
  heap_t* h = mmh_init();
  // messing with internals for this test...
  h->count = 10;
  int* tmp = h->data;

  //            0  1  2  3  4  5  6  7  8  9  10
  int a1[11] = {0, 0, 4, 2, 9, 3, 7, 6, 8, 5, 1};
  int a2[11] = {0, 0, 1, 2, 9, 3, 7, 6, 8, 5, 1};
  int a3[11] = {0, 0, 4, 2, 1, 3, 7, 6, 8, 5, 1};
  int a4[11] = {0, 0, 4, 2, 8, 1, 7, 6, 8, 5, 1};
  int a5[11] = {0, 0, 1, 2, 9, 3, 7, 6, 8, 5, 1};
  h->data = (int*)&a1;
  assert(index_min_child_grandchild(h, 1) == 3);
  h->data = (int*)&a2;
  assert(index_min_child_grandchild(h, 1) == 2);
  h->data = (int*)&a3;
  assert(index_min_child_grandchild(h, 1) == 4);
  h->data = (int*)&a4;
  assert(index_min_child_grandchild(h, 1) == 5);
  h->data = (int*)&a5;
  assert(index_min_child_grandchild(h, 1) == 2);
  for (; h->count > 1; h->count--) {
    assert(index_min_child_grandchild(h, 1) == 2);
  }

  // restore verbatim situation before calling the free function
  h->data = tmp;
  h->count = 0;

  mmh_free(h);
  printf("[OK]\n");
}

void t_rand_data_min_max() {
  printf("Testing random pop_min/pop_max after 10k random insert...  ");
  int array_size = 10000;
  heap_t* h = mmh_init();

  srand(time(NULL));
  int randarray[array_size];
  for (int i = 0; i < array_size; i++) {
    randarray[i] = rand() % (array_size * 10);
    mmh_insert(h, randarray[i]);
  }

  int randarray_max[array_size];
  int randarray_min[array_size];
  memcpy(&randarray_max, randarray, array_size * sizeof(int));
  memcpy(&randarray_min, randarray, array_size * sizeof(int));
  qsort(randarray_min, array_size, sizeof(int), compare_ints_min);
  qsort(randarray_max, array_size, sizeof(int), compare_ints_max);
  int min_index = 0;
  int max_index = 0;

  while (h->count > 0) {
    if (rand() % 2 == 0) {
      assert(randarray_min[min_index++] == mmh_pop_min(h));
    } else {
      assert(randarray_max[max_index++] == mmh_pop_max(h));
    }
  }
  assert(min_index + max_index == array_size);

  mmh_free(h);
  printf("[OK]\n");
}

void t_pop_from_empty() {
  printf("Testing empty pop...                                       ");
  heap_t* h = mmh_init();

  assert(mmh_pop_min(h) == -1);
  assert(mmh_pop_max(h) == -1);

  mmh_free(h);
  printf("[OK]\n");
}

void t_initial_size_pop_min_max() {
  printf("Testing random pop on different initial heap sizes...      ");
  for (int size = 1; size < 100; size++) {
    int array_size = 1000;
    heap_t* h = mmh_init_with_size(size);

    srand(time(NULL));
    int randarray[array_size];
    for (int i = 0; i < array_size; i++) {
      randarray[i] = rand() % (array_size * 10);
      mmh_insert(h, randarray[i]);
    }

    int randarray_max[array_size];
    int randarray_min[array_size];
    memcpy(&randarray_max, randarray, array_size * sizeof(int));
    memcpy(&randarray_min, randarray, array_size * sizeof(int));
    qsort(randarray_min, array_size, sizeof(int), compare_ints_min);
    qsort(randarray_max, array_size, sizeof(int), compare_ints_max);
    int min_index = 0;
    int max_index = 0;

    while (h->count > 0) {
      if (rand() % 2 == 0) {
        assert(randarray_min[min_index++] == mmh_pop_min(h));
      } else {
        assert(randarray_max[max_index++] == mmh_pop_max(h));
      }
    }
    assert(min_index + max_index == array_size);

    mmh_free(h);
  }
  printf("[OK]\n");
}

void t_insert_reinsert_data() {
  printf("Testing insert-empty-insert operation...                   ");
  heap_t* h = mmh_init();

  srand(time(NULL));
  int randarray[10] = {0, 4, 2, 9, 3, 7, 6, 8, 5, 1};
  for (int i = 0; i < 10; i++) {
    mmh_insert(h, randarray[i]);
  }

  qsort(randarray, 10, sizeof(int), compare_ints_min);
  int index = 0;
  while (h->count > 0) {
    assert(randarray[index++] == mmh_pop_min(h));
  }
  assert(index == 10);
  assert(mmh_pop_min(h) == -1);
  assert(mmh_pop_max(h) == -1);

  for (int i = 0; i < 10; i++) {
    mmh_insert(h, randarray[i]);
  }
  index = 0;
  while (h->count > 0) {
    assert(randarray[index++] == mmh_pop_min(h));
  }
  assert(index == 10);
  mmh_free(h);
  printf("[OK]\n");
}

void t_peek_min_max() {
  printf("Testing random peek on heap...                             ");
  for (int run = 1; run < 1000; run++) {
    heap_t* h = mmh_init();

    srand(time(NULL));
    int randarray[10];
    for (int i = 0; i < 10; i++) {
      randarray[i] = rand() % (10);
      mmh_insert(h, randarray[i]);
    }

    int randarray_max[10];
    int randarray_min[10];
    memcpy(&randarray_max, randarray, 10 * sizeof(int));
    memcpy(&randarray_min, randarray, 10 * sizeof(int));
    qsort(randarray_min, 10, sizeof(int), compare_ints_min);
    qsort(randarray_max, 10, sizeof(int), compare_ints_max);
    int min_index = 0;
    int max_index = 0;

    while (h->count > 0) {
      if (rand() % 2 == 0) {
        if (rand() % 2 == 0) {
          assert(randarray_max[max_index] == mmh_peek_max(h));
        } else {
          assert(randarray_min[min_index] == mmh_peek_min(h));
        }
      } else {
        if (rand() % 2 == 0) {
          assert(randarray_max[max_index++] == mmh_pop_max(h));
        } else {
          assert(randarray_min[min_index++] == mmh_pop_min(h));
        }
      }
    }
    assert(min_index + max_index == 10);

    mmh_free(h);
  }
  printf("[OK]\n");
}

int main() {
  t_down_max();
  t_down_min();
  t_up_max();
  t_up_min();

  t_insert_data_max();
  t_insert_data_min();

  t_rand_data_max();
  t_rand_data_min();

  t_rand_data_min_max();

  t_min_level();
  t_min_index();

  t_pop_from_empty();
  t_initial_size_pop_min_max();
  t_insert_reinsert_data();

  t_peek_min_max();

  return 0;
}
