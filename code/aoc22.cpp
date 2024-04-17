#include "momo.h"

#include <stdio.h>
#include <stdlib.h>

// 
// This will just consume all characters until the next \n or \r or 0. 
// The returned str_t will not include the \n or \r or 0. 
//
//
static void aoc22_d1p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t maxs[3] = {};
  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    str_t line = stream_consume_line(s);  
    if (line.size == 0) {
      for_arr(i, maxs) {
        if (sum > maxs[i]) {
          // assume that we are sorted. 
          // pass the baggage down
          u32_t baggage = maxs[i];
          for(u32_t j = i + 1; j < array_count(maxs); ++j) {
            if (baggage > maxs[j]) {
              maxs[j] = baggage;
              break;
            }
          }

          maxs[i] = sum;
          break;
        }
      }

      sum = 0;
      continue;
    }

    u32_t value = 0;
    if (str_to_u32(line, &value)){
      sum += value;
    }
    else {
      printf("Invalid line found\n");
      return;
    }

  }

  u32_t total = 0;
  for_arr(i, maxs) {
    total += maxs[i];
  }

  printf("%d\n", total);
}


static void aoc22_d1p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t max = 0;
  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    str_t line = stream_consume_line(s);  
    if (line.size == 0) {
      if (sum > max) {
        max = sum;
      }
      sum = 0;
      continue;
    }

    u32_t value = 0;
    if (str_to_u32(line, &value)){
      sum += value;
    }
    else {
      printf("Invalid line found\n");
      return;
    }

  }

  printf("%d\n", max);


}

static void aoc22_d2p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, false); 
  if (!file_buffer) return;


  make(stream_t, s);
  stream_init(s, file_buffer);

  // rock is 1
  // paper is 2
  // scissors is 3
  // 
  // lose is 0
  // draw is 3
  // win is 6

  // Format of each line is always "X Y"
  // This means we can read each line and just look at indices 0 and 2

  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    str_t line = stream_consume_line(s);  
    s32_t lhs = line.e[0] - 'A';
    s32_t rhs = line.e[2] - 'X';

    if (lhs == 0) {
      if (rhs == 0) sum += 3;
      else if (rhs == 1) sum += 6;
      else if (rhs == 2) sum += 0;
    }
    else if (lhs == 1) {
      if (rhs == 0) sum += 0;
      else if (rhs == 1) sum += 3;
      else if (rhs == 2) sum += 6;
    }
    else // (lhs == 2)
    {
      if (rhs == 0) sum += 6;
      else if (rhs == 1) sum += 0;
      else if (rhs == 2) sum += 3;
    }

    sum += rhs + 1;
  }
  printf("%d\n", sum);
}

static void aoc22_d2p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  // 
  // rock is 0
  // paper is 1
  // scissors is 2
  //
  // lose is 0
  // draw is 1
  // win is 2
  //
  // rock scores 1
  // paper scores 2
  // scissors scores 3
  //
  // lose scores 0
  // draw scores 3
  // win scores 6
  //
  const s32_t lose = 0;
  const s32_t draw = 3;
  const s32_t win = 6;
  const s32_t rock = 1;
  const s32_t paper = 2;
  const s32_t scissors = 3;

  // Format of each line is always "X Y"
  // This means we can read each line and just look at indices 0 and 2
  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    str_t line = stream_consume_line(s);  
    s32_t lhs = line.e[0] - 'A';
    s32_t rhs = line.e[2] - 'X';

    if (lhs == 0) { // is rock          
      if (rhs == 0) sum += lose + scissors; 
      else if (rhs == 1) sum += draw + rock;
      else if (rhs == 2) sum += win + paper;
    }
    else if (lhs == 1) { // is paper
      if (rhs == 0) sum += lose + rock;
      else if (rhs == 1) sum += draw + paper;
      else if (rhs == 2) sum += win + scissors;
    }
    else // (lhs == 2) // is scissors
    {
      if (rhs == 0) sum += lose + paper;
      else if (rhs == 1) sum += draw + scissors;
      else if (rhs == 2) sum += win + rock;
    }

  }
  printf("%d\n", sum);

}

static void aoc22_d3p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;
  while(!stream_is_eos(s)) 
  {
    str_t line = stream_consume_line(s);  
    str_t lhs = str_set(line.e, line.size/2);
    str_t rhs = str_set(line.e + line.size/2, line.size/2);
    
    // find common items
    for_cnt(i, lhs.size) 
    {
      for_cnt(j, rhs.size) 
      {
        if (lhs.e[i] == rhs.e[j]) 
        {
          // Map
          if(lhs.e[i] >= 'a' && lhs.e[i] <= 'z') {
            u32_t value = lhs.e[i] - 'a' + 1;
            sum += value;
          }
          else if (lhs.e[i] >= 'A' && lhs.e[i] <= 'Z') {
            u32_t value = lhs.e[i] - 'A' + 27;
            sum += value; 
          }
          goto found;
        }
      }
    }
found:;
  }
  printf("%d\n", sum);
}

static void aoc22_d3p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;
  while(!stream_is_eos(s)) 
  {
    str_t line0 = stream_consume_line(s);  
    str_t line1 = stream_consume_line(s);  
    str_t line2 = stream_consume_line(s);  


    // Find common item between line0 and line1
    for_cnt(i, line0.size) 
    {
      for_cnt(j, line1.size) 
      {
        for_cnt(k, line2.size)
        {
          if (line0.e[i] == line1.e[j] && 
              line1.e[j] == line2.e[k]) 
          {
            char item = line0.e[i]; 
            if(item >= 'a' && item <= 'z') {
              u32_t value = item - 'a' + 1;
              sum += value;
            }
            else if (item >= 'A' && item <= 'Z') {
              u32_t value = item - 'A' + 27;
              sum += value; 
            }
            goto found;
          }
        }
      }
    }
found:;
  }
  printf("%d\n", sum);
}

struct str_node_t  {
  str_t item;
  str_node_t* next;
};

struct str_arr_t {
  str_t* e;
  u32_t size;
};

static str_arr_t
str_split(str_t str, u8_t delimiter, arena_t* arena) {
  str_arr_t ret = {};
  if (!str) return ret;


  u32_t start = 0;
  u32_t end = 0;
  for(; end < str.size; ++end) 
  {
    if (str.e[end] == delimiter) 
    {
      str_t* new_node = arena_push(str_t, arena);
      dref(new_node) = str_set(str.e + start, end - start);

      if (ret.e == nullptr) {
        ret.e = new_node;
      }
      // book keeping
      ++end;
      start = end;
      ++ret.size;
    }
  }

  str_t* new_node = arena_push(str_t, arena);
  dref(new_node) = str_set(str.e + start, end - start);
  ++ret.size;

  return ret;
}
static void aoc22_d4p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;

  while(!stream_is_eos(s)) 
  {
    arena_set_revert_point(arena);
    str_t str = stream_consume_line(s);  
    u32_t nums[4] = {};
    
    str_arr_t arr0 = str_split(str, ',', arena);
    u32_t cur_index = 0;
    for_cnt(i, arr0.size) 
    {
      arena_set_revert_point(arena);
      str_arr_t arr1 = str_split(arr0.e[i], '-', arena);
      for_cnt(j, arr1.size) 
      {
        str_to_u32(arr1.e[j], &nums[cur_index++]);
      }
    }


    // Example: 20-40,60-80
    u32_t lhs_low = nums[0];
    u32_t lhs_high = nums[1];
    u32_t rhs_low = nums[2];
    u32_t rhs_high = nums[3];

    if ((rhs_low >= lhs_low && rhs_high <= lhs_high) ||
        (lhs_low >= rhs_low && lhs_high <= rhs_high)) 
    {
      ++sum;
    }
  }
  printf("%d\n", sum);
}

static void aoc22_d4p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;

  while(!stream_is_eos(s)) 
  {
    arena_set_revert_point(arena);
    str_t str = stream_consume_line(s);  
    u32_t nums[4] = {};
    
    str_arr_t arr0 = str_split(str, ',', arena);
    u32_t cur_index = 0;
    for_cnt(i, arr0.size) 
    {
      arena_set_revert_point(arena);
      str_arr_t arr1 = str_split(arr0.e[i], '-', arena);
      for_cnt(j, arr1.size) 
      {
        str_to_u32(arr1.e[j], &nums[cur_index++]);
      }
    }


    // Example: 20-40,60-80
    u32_t lhs_low = nums[0];
    u32_t lhs_high = nums[1];
    u32_t rhs_low = nums[2];
    u32_t rhs_high = nums[3];

    // This is stupid so non-intuitive for me like why is it so hard
    // Okay the way I think about it is that there are two scenarios
    // where it does not collide
    //
    // Example 1:
    //         a0 ------ a1
    //  b0 -- b1
    // 
    // Example 2:
    //         a0 ------ a1
    //                      b0 -- b1
    //
    // So, it is NOT colliding when:
    //   b1 < a0 OR b0 > a1
    //
    // Thus it IS colliding when:
    //     !(b1 < a0 OR b0 > a1)
    //   = b1 >= a0 AND b0 <= a1
    // 
    // Damn I love algebra
    //   
    if (rhs_high >= lhs_low && rhs_low <= lhs_high)
    {
      ++sum;
    }
  }
  printf("%d\n", sum);
}


struct aoc22_d5p1_node_t {
  char value;
  aoc22_d5p1_node_t* next;
  aoc22_d5p1_node_t* prev;
}
static void aoc22_d5p1(const char* filename, arena_t* arena) {
  //
  // @note: Format looks something like this:
  //
  //     [D]    
  // [N] [C]    
  // [Z] [M] [P]
  //  1   2   3 
  //
  // move 1 from 2 to 1
  // move 3 from 1 to 3
  // move 2 from 2 to 1
  // move 1 from 1 to 2
  // 
  // @note: 
  //  The maximum stacks is 9
  //
  arena_set_revert_point(arena);


  str_t file_buffer = file_read_into_str(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  //u32_t sum = 0;

  aoc22_d5p1_node_t stacks[9] = {};
  while(!stream_is_eos(s)) 
  {
    arena_set_revert_point(arena);
    str_t str = stream_consume_line(s);  
    if (str.e[1] == '1') break;

#if 0
    for(int i = 0; i < str.size; ++i) {
      printf("%c", str.e[i]);
    }
    printf("\n");
#endif
  }
  //printf("%d\n", sum);
}

int main(int argv, char** argc) {
  if (argv < 2) {
    printf("Usage: aoc22 <day> <part> <filename>\nExample: aoc22 1 1 input.txt\n");
    return 1;
  }

  u32_t day = 0;
  u32_t part = 0;
  char* filename = argc[3];

  if (!str_to_u32(str_from_cstr(argc[1]), &day)) {
    printf("Bad day\n");
    return 1;
  }

  if (!str_to_u32(str_from_cstr(argc[2]), &part)) {
    printf("Bad part\n");
    return 1;
  }

  make(arena_t, arena);
  arena_alloc(arena, gigabytes(1)); 
  defer { arena_free(arena); }; 

#define aoc22_route(dd, pp) if (day == dd && part == pp) aoc22_d ## dd ## p ## pp(filename, arena);
  aoc22_route(1,1);
  aoc22_route(1,2);
  aoc22_route(2,1);
  aoc22_route(2,2);
  aoc22_route(3,1);
  aoc22_route(3,2);
  aoc22_route(4,1);
  aoc22_route(4,2);
  aoc22_route(5,1);

}
