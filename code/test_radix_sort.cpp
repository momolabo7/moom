#include "momo.h"

// How this works:
//   Recall how IEEE floating point works. 
//   If our floating points are all positive (signed bit is 0),
//   we technically don't have to care much about the exponent
//   and mantisa because they are already in order.
//   e.g. 3.14 > 5.123 when comparing just their bits.
//
//   The issue comes when we have to consider NEGATIVE floating point
//   values. We solve the positive floating point values by setting the 
//   first bit, which will shift all positive numbers forwards. 
//
//   This implies that all negative numbers will not have their first bit 
//   set. However, just 'unsetting' the negative number's first bit is not 
//   enough before negative numbers increase backwards. 
//
//   e.g. -3.14 > -5.123 but if we just remove the negative bits, 
//         3.14 is now < 5.123, which is wrong. 
//         We must maintain that 3.14 < 5.123
//         (since they are originally negative)
//
//   Consider that we want the following (remember first bit means negative):
//     100 > 101  > 110 > 111
//     (-0)  (-1)   (-2)  (-3)
//   
//   to be converted to something like this:
//     011 > 010  > 001 > 000
//     (3)   (2)    (1)   (0)
//
//   which means -0 converts to 3, -1 converts to 2, etc...
//
//   It seems that we can presumably just flip the bits. 
//
static u32_t 
_sort_key_to_u32(f32_t sort_key)
{
  u32_t result = *(u32_t*)&sort_key;
  if (result & 0x80000000) 
  {
    // if signed bit is set, flip everything
    result = ~result;
  }
  else 
  {
    // if signed bit is not set, set it
    result |= 0x80000000;
  }
  return result;
}

static void 
sort_radix(sort_entry_t* entries, u32_t entry_count, arena_t* arena)
{
  arena_set_revert_point(arena);
  sort_entry_t* tmp = arena_push_arr(sort_entry_t, arena, entry_count);
  assert(tmp);

  sort_entry_t* src = entries;
  sort_entry_t* dest = tmp;

  for(u32_t byte_index = 0;
      byte_index < 32;
      byte_index += 8)
  {
    u32_t offsets[256] = {};

    // Counting pass: count how many of each key
    for_cnt(i, entry_count) {
      u32_t value = _sort_key_to_u32(src[i].key);
      u32_t piece = (value >> byte_index) & 0xFF;
      ++offsets[piece];
    }
    
    // Cumulative sum pass: change counts to offsets
    // Basically at the end of this pass, each item
    // in offsets[] should contain the index of dest
    // where the first item of that value to go.
    u32_t total = 0;
    for_arr(offset_index, offsets) 
    {
      u32_t count = offsets[offset_index];
      offsets[offset_index] = total;
      total += count;
    }

    for_cnt(i, entry_count) {
      u32_t value = _sort_key_to_u32(src[i].key);
      u32_t piece = (value >> byte_index) & 0xFF;
      dest[offsets[piece]++] = src[i];
    }

    swap(src, dest);

  }


}

int main() 
{

  make(arena_t, arena);
  arena_alloc(arena, gigabytes(1)); 
  defer { arena_free(arena); }; 

  f32_t nums[] = { -22, 35, 53, 10, 22, 145, -230, 103};
  sort_radix(nums, array_count(nums), arena);


  for_arr(id, nums) 
  {
    printf("%f ", nums[id]);
  }
  printf("\n");
}
