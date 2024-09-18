
#include <stdlib.h>
#include <stdio.h>

#include "momo.h"

static void
test_arena_performance() 
{
  printf("Hello Test\n");
  arena_t test = {};

  u64_t res = os_get_clock_resolution();
  printf("%zu\n", res);

  const u32_t runs = 10000000;
  // arena with committed memory
  {
    arena_alloc(&test, gigabytes(1), true);
    u64_t start_time = os_get_clock_time();
    for (int i = 0; i < runs; ++i) { 
      arena_push_size(&test, sizeof(u32_t), alignof(u32_t));  
    }
    u64_t end_time = os_get_clock_time();
    printf("arena #0: %f\n", ((f32_t)(end_time - start_time))/res);
  }

  // arena with non-committed memory
  {
    arena_alloc(&test, gigabytes(1));
    defer { arena_free(&test); };
    u64_t start_time = os_get_clock_time();
    for (int i = 0; i < runs; ++i) { 
      arena_push_size(&test, sizeof(u32_t), alignof(u32_t));  
    }
    u64_t end_time = os_get_clock_time();
    printf("arena #1: %f\n", ((f32_t)(end_time - start_time))/res);
  }

 
}

int main() {
  arena_t test = {};
  arena_alloc(&test, gigabytes(1), true);
  printf("arena address:\t%p\n", test.memory);
  printf("arena pos:\t%p\n", test.memory + test.pos);
  
  printf("> pushing array of 100 elements\n");
  u32_t* arr = arena_push_arr(u32_t, &test, 100);
  printf("arr address:\t%p\n", arr);
  printf("arr end pos:\t%p\n", arr + 100);
  printf("arena address:\t%p\n", test.memory);
  printf("arena pos:\t%p\n", test.memory + test.pos);

  if(arena_grow_arr(u32_t, &test, arr, 100, 200))
  {
    printf("> array grown successfully to 200\n"); 
    printf("arr address:\t%p\n", arr);
    printf("arr end pos:\t%p\n", arr + 200);
    printf("arena address:\t%p\n", test.memory);
    printf("arena pos:\t%p\n", test.memory + test.pos);
  }
  else
  {
    printf("> failed to grow array\n");
  }


}
