#include "momo.h"
#include <stdio.h>


struct bigint_t 
{
  u8_t* arr;
  u32_t cap;
  u32_t count; 
};



int main() {
  make(arena_t, a);
  arena_alloc(a, megabytes(256), false);

  make(bigint_t, b1);
  bigint_init(b1, 32, a);
  bigint_add(b1, 123);
  bigint_add(b1, 123);

  make(bigint_t, b2);
  bigint_init(b2, 32, a);
  bigint_add(b2, 123);

  printf("%d\n", bigint_compare(b1, b2));
  for_cnt(i, b1->count) {
    printf("%d", b1->arr[i]);
  }
  printf("\n");


}
