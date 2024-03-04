
#include <stdlib.h>
#include <stdio.h>

#define FOOLISH
#include "momo.h"

int main() {
  printf("Hello Test\n");
  arena_t test = {};

  const u32_t runs = 10000000;
  // arena with committed memory
  {
    arena_alloc(&test, gigabytes(1), true);
    u64_t start_time = os_get_clock_time();
    for (int i = 0; i < runs; ++i) { 
      arena_push_size(&test, sizeof(u32_t), alignof(u32_t));  
    }
    u64_t end_time = os_get_clock_time();
    printf("arena #0: %f\n", (f32_t)(end_time - start_time)/runs);
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
    printf("arena #1: %f\n", (f32_t)(end_time - start_time)/runs);
  }

  // Olde arena
  {
    arena_t normal_arena = {};
    str_t buffer = foolish_allocate_memory(gigabytes(1));
    defer { foolish_free_memory(buffer); };
    arena_init(&normal_arena, buffer);

    u64_t start_time = os_get_clock_time();
    for (int i = 0; i < runs; ++i) { 
      arena_push_size(&normal_arena, sizeof(u32_t), alignof(u32_t));  
    }
    u64_t end_time = os_get_clock_time();

    printf("arena #2: %f\n", (f32_t)(end_time - start_time)/runs);
  }

 
}
