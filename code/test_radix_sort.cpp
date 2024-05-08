#include "momo.h"


int main() 
{

  make(arena_t, arena);
  arena_alloc(arena, gigabytes(1)); 
  defer { arena_free(arena); }; 

  u32_t size = 8;
  sort_entry_t* nums = arena_push_arr(sort_entry_t, arena, size);
  nums[0].key = 10.f;
  nums[1].key = 3.f;
  nums[2].key = 12.f;
  nums[3].key = 5.f;
  nums[4].key = 14.f;
  nums[5].key = 8.f;
  nums[6].key = 25.f;
  nums[7].key = -13.f;
  sort_radix(nums, size, arena);

  for_cnt(id, size) 
  {
    printf("%.2f ", nums[id].key);
  }
  printf("\n");
}
