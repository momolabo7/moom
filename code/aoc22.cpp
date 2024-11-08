#include "momo.h"

#include <stdio.h>
#include <stdlib.h>

struct aoc22_grid_t
{
  buffer_t buffer;
  u32_t endline_padding; // 1 if \n, 2 if \n\r
  u32_t w, h;
};

static u8_t
aoc22_grid_get(aoc22_grid_t* grid, u32_t x, u32_t y)
{
  assert(x < grid->w);
  assert(y < grid->h);
  return grid->buffer.e[x + (grid->w + grid->endline_padding) * y];
}

static void
aoc22_grid_init(aoc22_grid_t* grid, buffer_t b)
{
  // assumes that it's a square grid.
  grid->buffer = b;

  stream_t s;
  stream_init(&s, b);

  // count width
  grid->w = 0;
  grid->endline_padding = 1;
  {
    buffer_t line = stream_consume_line(&s);  
    // @note: kinda hacky way to count \n or \r or \n\r
    //
    // There's only a few combinations. The line either ends with:
    // - \n (linux)
    // - \r (mac)
    // - \r\n (windows)
    // - 0 (eof)
    //
    // There is only one case where there's two characters (windows) so 
    // we technically just need to check 2 characters ahead and see if it's
    // \r or not
    if (line.e[line.size+1] == '\n'){
      grid->endline_padding = 2;
    }
    
    grid->w = line.size;
    stream_reset(&s);
  }

  // count height
  grid->h = 0;
  while(!stream_is_eos(&s)) 
  {
    stream_consume_line(&s);  
    ++grid->h;
  }
}

// 
// This will just consume all characters until the next \n or \r or 0. 
// The returned buffer_t will not include the \n or \r or 0. 
//
static void aoc22_d1p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t maxs[3] = {};
  u32_t sum = 0;
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);  
    if (line.size == 0) 
    {
      for_arr(i, maxs) 
      {
        if (sum > maxs[i]) 
        {
          // assume that we are sorted. 
          // pass the baggage down
          u32_t baggage = maxs[i];
          for(u32_t j = i + 1; j < array_count(maxs); ++j) 
          {
            if (baggage > maxs[j]) 
            {
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
    if (buffer_to_u32(line, &value))
    {
      sum += value;
    }
    else 
    {
      printf("Invalid line found\n");
      return;
    }
  }

  u32_t total = 0;
  for_arr(i, maxs) 
  {
    total += maxs[i];
  }

  printf("%d\n", total);
}


static void aoc22_d1p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t max = 0;
  u32_t sum = 0;
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);  
    if (line.size == 0) 
    {
      if (sum > max) 
      {
        max = sum;
      }
      sum = 0;
      continue;
    }

    u32_t value = 0;
    if (buffer_to_u32(line, &value))
    {
      sum += value;
    }
    else 
    {
      printf("Invalid line found\n");
      return;
    }

  }

  printf("%d\n", max);


}

static void aoc22_d2p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, false); 
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
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);  
    s32_t lhs = line.e[0] - 'A';
    s32_t rhs = line.e[2] - 'X';
    
    if (lhs == 0) 
    {
      if (rhs == 0) sum += 3;
      else if (rhs == 1) sum += 6;
      else if (rhs == 2) sum += 0;
    }
    else if (lhs == 1) 
    {
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

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
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
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);  
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

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);  
    buffer_t lhs = buffer_set(line.e, line.size/2);
    buffer_t rhs = buffer_set(line.e + line.size/2, line.size/2);
    
    // find common items
    for_cnt(i, lhs.size) 
    {
      for_cnt(j, rhs.size) 
      {
        if (lhs.e[i] == rhs.e[j]) 
        {
          // Map
          if(lhs.e[i] >= 'a' && lhs.e[i] <= 'z') 
          {
            u32_t value = lhs.e[i] - 'a' + 1;
            sum += value;
          }
          else if (lhs.e[i] >= 'A' && lhs.e[i] <= 'Z') 
          {
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

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;
  while(!stream_is_eos(s)) 
  {
    buffer_t line0 = stream_consume_line(s);  
    buffer_t line1 = stream_consume_line(s);  
    buffer_t line2 = stream_consume_line(s);  

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
            if(item >= 'a' && item <= 'z') 
            {
              u32_t value = item - 'a' + 1;
              sum += value;
            }
            else if (item >= 'A' && item <= 'Z') 
            {
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

static void aoc22_d4p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;

  while(!stream_is_eos(s)) 
  {
    arena_set_revert_point(arena);
    buffer_t str8 = stream_consume_line(s);  
    u32_t nums[4] = {};
    
    buffer_arr_t arr0 = buffer_split(str8, ',', arena);
    u32_t cur_index = 0;
    for_cnt(i, arr0.size) 
    {
      arena_set_revert_point(arena);
      buffer_arr_t arr1 = buffer_split(arr0.e[i], '-', arena);
      for_cnt(j, arr1.size) 
      {
        buffer_to_u32(arr1.e[j], &nums[cur_index++]);
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

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t sum = 0;

  while(!stream_is_eos(s)) 
  {
    arena_set_revert_point(arena);
    buffer_t str8 = stream_consume_line(s);  
    u32_t nums[4] = {};
    
    buffer_arr_t arr0 = buffer_split(str8, ',', arena);
    u32_t cur_index = 0;
    for_cnt(i, arr0.size) 
    {
      arena_set_revert_point(arena);
      buffer_arr_t arr1 = buffer_split(arr0.e[i], '-', arena);
      for_cnt(j, arr1.size) 
      {
        buffer_to_u32(arr1.e[j], &nums[cur_index++]);
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


struct aoc22_d5_node_t {
  char value;
  aoc22_d5_node_t* next;
  aoc22_d5_node_t* prev;
};

static void aoc22_d5p1(const char* filename, arena_t* arena) {
  //
  // @note: Format looks something like this:
  //
  //     [D]    
  // [N] [C]    
  // [Z] [M] [P]
  //  1   k   3 
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


  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  //u32_t sum = 0;

  aoc22_d5_node_t stacks[9] = {};
  for_arr(i, stacks) cll_init(stacks + i);

  b32_t is_setup = true;
  while(!stream_is_eos(s)) 
  {
    buffer_t str8 = stream_consume_line(s);  
    if (str8.e[1] == '1') {
      is_setup = false;
      stream_consume_line(s); // there is one more line of whitespace
      
#if 0
      for_arr(i, stacks) 
      {
        auto* s = stacks + i;
        printf("%d: ", i);
        auto* itr = s->next;
        while(itr != s) {
          printf("[%c] > ", itr->value);
          itr = itr->next;
        }
        printf("\n");

      }
#endif
      continue;
    }

    if (is_setup) // setup phase
    {
      for(u32_t i = 1, j = 0; i < str8.size; i+=4, ++j) 
      {
        if (str8.e[i] != ' ') 
        {
          auto* new_node = arena_push(aoc22_d5_node_t, arena);
          new_node->value = str8.e[i];
          cll_push_front(stacks + j, new_node);
        }
      }
    }
    else // movement phase 
    {
      arena_set_revert_point(arena);
      buffer_arr_t split = buffer_split(str8, ' ', arena);
      u32_t amount, from, to = 0;
      buffer_to_u32(split.e[1], &amount);
      buffer_to_u32(split.e[3], &from);
      buffer_to_u32(split.e[5], &to);
      from -= 1;
      to -= 1;
      //printf("moving %d from %d to %d\n", amount, from, to);

      for_cnt(i, amount) 
      {
        auto* from_stack_sentinel = stacks + from;
        auto* to_stack_sentinel = stacks + to;
        
        // grab the last node from the 'from' stack;
        auto* node_to_move = from_stack_sentinel->prev;
        cll_remove(node_to_move);  // unlink the node
        cll_push_back(to_stack_sentinel, node_to_move); // put to to the back of the 'to' stack
      }
#if 0
      for_arr(i, stacks) 
      {
        auto* s = stacks + i;
        printf("%d: ", i);
        auto* itr = s->next;
        while(itr != s) {
          printf("[%c] > ", itr->value);
          itr = itr->next;
        }
        printf("\n");
      }
#endif
    }

  }

  for_arr(i, stacks) 
  {
    auto* s = stacks + i;
    printf("%c", s->prev->value);
  }
  printf("\n");



  //printf("%d\n", sum);
}

static void aoc22_d5p2(const char* filename, arena_t* arena) {
  //
  // @note: Format looks something like this:
  //
  //     [D]    
  // [N] [C]    
  // [Z] [M] [P]
  //  1   k   3 
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


  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  //u32_t sum = 0;

  aoc22_d5_node_t stacks[9] = {};
  for_arr(i, stacks) cll_init(stacks + i);

  b32_t is_setup = true;
  while(!stream_is_eos(s)) 
  {
    buffer_t str8 = stream_consume_line(s);  
    if (str8.e[1] == '1') {
      is_setup = false;
      stream_consume_line(s); // there is one more line of whitespace
      
#if 0
      for_arr(i, stacks) 
      {
        auto* s = stacks + i;
        printf("%d: ", i);
        auto* itr = s->next;
        while(itr != s) {
          printf("[%c] > ", itr->value);
          itr = itr->next;
        }
        printf("\n");

      }
#endif
      continue;
    }

    if (is_setup) // setup phase
    {
      for(u32_t i = 1, j = 0; i < str8.size; i+=4, ++j) 
      {
        if (str8.e[i] != ' ') 
        {
          auto* new_node = arena_push(aoc22_d5_node_t, arena);
          new_node->value = str8.e[i];
          cll_push_front(stacks + j, new_node);
        }
      }
    }
    else // movement phase 
    {
      arena_set_revert_point(arena);
      buffer_arr_t split = buffer_split(str8, ' ', arena);
      u32_t amount, from, to = 0;
      buffer_to_u32(split.e[1], &amount);
      buffer_to_u32(split.e[3], &from);
      buffer_to_u32(split.e[5], &to);
      from -= 1;
      to -= 1;
      //printf("moving %d from %d to %d\n", amount, from, to);

      auto* from_stack_sentinel = stacks + from;
      auto* to_stack_sentinel = stacks + to;
      
      // grab the nth node from the top of the 'from' stack;
      auto* node_to_move_top = from_stack_sentinel->prev;
      auto* node_to_move_bottom = from_stack_sentinel;
      for_cnt(i, amount) {
        node_to_move_bottom = node_to_move_bottom->prev;
      }
      node_to_move_bottom->prev->next = from_stack_sentinel;
      node_to_move_top->next->prev = node_to_move_bottom->prev;
      node_to_move_bottom->prev = 0;
      node_to_move_top->next = 0;
      
      
      // Add it to the top of the 'to' stack by changing its pointers
      auto* node_to_add_on = to_stack_sentinel->prev;
      node_to_add_on->next = node_to_move_bottom;
      node_to_move_bottom->prev = node_to_add_on;
      node_to_move_top->next = to_stack_sentinel;
      to_stack_sentinel->prev = node_to_move_top;
      
      

#if 0
      for_arr(i, stacks) 
      {
        auto* s = stacks + i;
        printf("%d: ", i);
        auto* itr = s->next;
        while(itr != s) {
          printf("[%c] > ", itr->value);
          itr = itr->next;
        }
        printf("\n");
      }
#endif
    }

  }

  for_arr(i, stacks) 
  {
    auto* s = stacks + i;
    printf("%c", s->prev->value);
  }
  printf("\n");



  //printf("%d\n", sum);
}

static void 
aoc22_d6p1(const char* filename, arena_t* arena) 
{
  const u32_t window_size = 4;
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t solulu = 0;
  while(!stream_is_eos(s)) 
  {
    // 012345, size = 6
    buffer_t str8 = stream_consume_line(s);  
    for_cnt(buffer_index, str8.size - window_size - 1) 
    {
      //printf("%c", str8.e[buffer_index]);
      u32_t repeats_found = false;
      for(u32_t i = 0; i < window_size; ++i) 
      {
        for(u32_t j = i + 1; j < window_size; ++j) 
        {
          if (str8.e[buffer_index + i] == str8.e[buffer_index + j])
          {
            repeats_found = true;
            goto stop_checking;
          }
        }
      }
      
stop_checking:
      if (!repeats_found) 
      {
        solulu = buffer_index + window_size;
        goto found;
      }
    }

found:;
    printf("%u\n", solulu);

  }
}

static void 
aoc22_d6p2(const char* filename, arena_t* arena) 
{
  const u32_t window_size = 14;
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t solulu = 0;
  while(!stream_is_eos(s)) 
  {
    // 012345, size = 6
    buffer_t str8 = stream_consume_line(s);  
    for_cnt(buffer_index, str8.size - window_size - 1) 
    {
      //printf("%c", str8.e[buffer_index]);
      u32_t repeats_found = false;
      for(u32_t i = 0; i < window_size; ++i) 
      {
        for(u32_t j = i + 1; j < window_size; ++j) 
        {
          if (str8.e[buffer_index + i] == str8.e[buffer_index + j])
          {
            repeats_found = true;
            goto stop_checking;
          }
        }
      }
      
stop_checking:
      if (!repeats_found) 
      {
        solulu = buffer_index + window_size;
        goto found;
      }
    }

found:;
    printf("%u\n", solulu);
  }
}

enum aoc22_d7_node_type_t {
  AOC22_D7_NODE_TYPE_DIR,
  AOC22_D7_NODE_TYPE_FILE
};

struct aoc22_d7_file_t;
struct aoc22_d7_dir_t;
struct aoc22_d7_node_t {
  aoc22_d7_node_type_t type;
  buffer_t name;
  union {
    aoc22_d7_file_t* file;
    aoc22_d7_dir_t* dir;
  };

  aoc22_d7_node_t* prev, *next;
};


struct aoc22_d7_file_t 
{
  u32_t size;
};

struct aoc22_d7_dir_t 
{
  aoc22_d7_dir_t* parent;
  aoc22_d7_node_t sentinel;
};

static aoc22_d7_node_t*
aoc22_d7_push_dir(
    aoc22_d7_dir_t* parent, 
    buffer_t name, 
    arena_t* arena)
{
  auto* node = arena_push(aoc22_d7_node_t, arena);
  node->type = AOC22_D7_NODE_TYPE_DIR;
  node->name = name;
  node->dir = arena_push(aoc22_d7_dir_t, arena);
  cll_init(&node->dir->sentinel);
  node->dir->parent = parent;
  if (parent)
  {
    cll_push_back(&parent->sentinel, node);
  }
  return node;
}

static aoc22_d7_node_t*
aoc22_d7_push_file(
    aoc22_d7_dir_t* parent, 
    buffer_t name, 
    u32_t size,
    arena_t* arena)
{
  auto* node = arena_push(aoc22_d7_node_t, arena);
  node->type = AOC22_D7_NODE_TYPE_FILE;
  node->name = name;
  node->file = arena_push(aoc22_d7_file_t, arena);
  node->file->size = size;
  if (parent)
  {
    cll_push_back(&parent->sentinel, node);
  }
  return node;
}

static void
aoc22_print_str8(buffer_t str8) {
  for_cnt(i, str8.size) {
    printf("%c", str8.e[i]);
  }
}

static u32_t 
aoc22_d7p1_parse_dir(aoc22_d7_dir_t* dir, u32_t* total_dir_sum)
{
  u32_t ret = 0; 

  cll_foreach(itr, &dir->sentinel)
  {
    if (itr->type == AOC22_D7_NODE_TYPE_DIR) 
    {
      u32_t sum = aoc22_d7p1_parse_dir(itr->dir, total_dir_sum);
#if 0
      printf("directory: ");
      aoc22_print_str8(itr->name);
      printf(" @ %d\n", sum);
#endif
      ret += sum;
    }
    else
    {
#if 0
      printf("file: ");
      aoc22_print_str8(itr->name);
      printf("\n");
#endif
      //if (itr->file->size <= 100000)
        ret += itr->file->size;
    }

  }

  if (ret <= 100000) {
    dref(total_dir_sum) += ret;
  }
  return ret;
}

static u32_t 
aoc22_d7p2_parse_dir(aoc22_d7_dir_t* dir, u32_t* kinda_min, u32_t space_needed_to_free)
{
  u32_t ret = 0; 

  cll_foreach(itr, &dir->sentinel)
  {
    if (itr->type == AOC22_D7_NODE_TYPE_DIR) 
    {
      u32_t sum = aoc22_d7p2_parse_dir(itr->dir, kinda_min, space_needed_to_free);
      ret += sum;
    }
    else
    {
      ret += itr->file->size;
    }

  }

  if (ret >= space_needed_to_free) 
  {
    if (ret < dref(kinda_min))
    {
      dref(kinda_min) = ret;
    }
  }
  return ret;
}

static u32_t 
aoc22_d7_get_dir_size(aoc22_d7_dir_t* dir)
{
  u32_t ret = 0; 

  cll_foreach(itr, &dir->sentinel)
  {
    if (itr->type == AOC22_D7_NODE_TYPE_DIR) 
    {
      u32_t sum = aoc22_d7_get_dir_size(itr->dir);
      ret += sum;
    }
    else
    {
      ret += itr->file->size;
    }

  }
  return ret;
}

static void
aoc22_d7_print_nodes(aoc22_d7_node_t* node, u32_t level = 0) 
{
  for_cnt(i, level) printf(" ");
  aoc22_print_str8(node->name);
  printf(" (dir, size = %u)\n", aoc22_d7_get_dir_size(node->dir));

  level += 2;

  cll_foreach(itr, &node->dir->sentinel)
  {
    if (itr->type == AOC22_D7_NODE_TYPE_DIR) 
    {
      aoc22_d7_print_nodes(itr, level);
    }
    else
    {
      for_cnt(i, level) printf(" ");
      aoc22_print_str8(itr->name);
      printf(" (file, size = %u)\n", itr->file->size);
    }

  }
}

static void 
aoc22_d7p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  // Initialize root first
  aoc22_d7_node_t* root = aoc22_d7_push_dir(nullptr, buffer_from_lit("/"), arena);
  aoc22_d7_dir_t* cd = root->dir;

  // @note: We are pretty much assume ignore 'ls' command because
  // it doesn't really do anything. We just need to handle lines
  // that represent 'cd' commands, files and directories
  while(!stream_is_eos(s)) 
  {
    buffer_t str8 = stream_consume_line(s);  
    if (str8.e[0] == '$') 
    {
      if (str8.e[2] == 'c') //cd
      {
        buffer_t dir_name = buffer_set(str8.e + 5, str8.size - 5);

        // go to root
        if (buffer_match(dir_name, buffer_from_lit("/")))
        {
          cd = root->dir;
          //printf("cd /\n");
        }

        // go to parent
        else if (buffer_match(dir_name, buffer_from_lit("..")))
        {
          cd = cd->parent;
          //printf("cd ..\n");
        }
        else 
        {
          // Look for directory with the name and set it as current dir
          aoc22_d7_node_t* itr = cd->sentinel.next;
          while(itr != &cd->sentinel) 
          {
            if (buffer_match(itr->name, dir_name))
            {
              cd = itr->dir; 
              break;
            }
            itr = itr->next;
          }
          //printf("cd ");
          //aoc22_print_str8(dir_name);
          //printf("\n");
        }
      }
    }
    else 
    {
      arena_marker_t mark = arena_mark(arena);
      buffer_arr_t arr = buffer_split(str8, ' ', arena);
      // format: dir <directory_name> 
      if (buffer_match(arr.e[0], buffer_from_lit("dir")))
      {
        buffer_t dir_name = arr.e[1]; 
        //printf("directory found: ");
        //aoc22_print_str8(dir_name);
        //printf("\n");

        arena_revert(mark);

        // Create a directory
        aoc22_d7_push_dir(cd, dir_name, arena); 

      }
      else 
      {
        u32_t file_size = 0;
        buffer_t file_name = arr.e[1];
        buffer_to_u32(arr.e[0], &file_size);

        //printf("file found: ");
        //aoc22_print_str8(file_name);
        //printf(" @ %d\n", file_size);

        arena_revert(mark);

        // format: <size> <filename>
        aoc22_d7_push_file(cd, file_name, file_size, arena); 
      }
    }
  }

  u32_t sum = 0;
  aoc22_d7p1_parse_dir(root->dir, &sum);
  printf("%d\n", sum);
}


static void 
aoc22_d7p2(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  // Initialize root first
  aoc22_d7_node_t* root = aoc22_d7_push_dir(nullptr, buffer_from_lit("/"), arena);
  aoc22_d7_dir_t* cd = root->dir;

  // @note: We are pretty much assume ignore 'ls' command because
  // it doesn't really do anything. We just need to handle lines
  // that represent 'cd' commands, files and directories
  while(!stream_is_eos(s)) 
  {
    buffer_t str8 = stream_consume_line(s);  
    if (str8.e[0] == '$') 
    {
      if (str8.e[2] == 'c') //cd
      {
        buffer_t dir_name = buffer_set(str8.e + 5, str8.size - 5);

        // go to root
        if (buffer_match(dir_name, buffer_from_lit("/")))
        {
          cd = root->dir;
          //printf("cd /\n");
        }

        // go to parent
        else if (buffer_match(dir_name, buffer_from_lit("..")))
        {
          cd = cd->parent;
          //printf("cd ..\n");
        }
        else 
        {
          // Look for directory with the name and set it as current dir
          aoc22_d7_node_t* itr = cd->sentinel.next;
          while(itr != &cd->sentinel) 
          {
            if (buffer_match(itr->name, dir_name))
            {
              cd = itr->dir; 
              break;
            }
            itr = itr->next;
          }
          //printf("cd ");
          //aoc22_print_str8(dir_name);
          //printf("\n");
        }
      }
    }
    else 
    {
      arena_marker_t mark = arena_mark(arena);
      buffer_arr_t arr = buffer_split(str8, ' ', arena);
      // format: dir <directory_name> 
      if (buffer_match(arr.e[0], buffer_from_lit("dir")))
      {
        buffer_t dir_name = arr.e[1]; 
        //printf("directory found: ");
        //aoc22_print_str8(dir_name);
        //printf("\n");

        arena_revert(mark);

        // Create a directory
        aoc22_d7_push_dir(cd, dir_name, arena); 

      }
      else 
      {
        u32_t file_size = 0;
        buffer_t file_name = arr.e[1];
        buffer_to_u32(arr.e[0], &file_size);

        //printf("file found: ");
        //aoc22_print_str8(file_name);
        //printf(" @ %d\n", file_size);

        arena_revert(mark);

        // format: <size> <filename>
        aoc22_d7_push_file(cd, file_name, file_size, arena); 
      }
    }
  }

  const u32_t space_total = 70000000;
  const u32_t space_required = 30000000;
  const u32_t space_used = aoc22_d7_get_dir_size(root->dir);
  const u32_t space_free = space_total - space_used; 
  const u32_t space_needed_to_free = space_required - space_free;
  
  //aoc22_d7_print_nodes(root);
  u32_t result = space_total;
  aoc22_d7p2_parse_dir(root->dir, &result, space_needed_to_free);
  printf("%d\n", result);
}


static void 
aoc22_d8p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  aoc22_grid_t grid;
  aoc22_grid_init(&grid, file_buffer);

  u32_t sum = grid.w * 2 + grid.h * 2 - 4;

  for_range(y, 1, grid.h-1)
  {
    for_range(x, 1, grid.w-1)
    {
      u32_t blocked_sides = 0; // if this is 4, means not visible
      u8_t current_tree = aoc22_grid_get(&grid, x, y);

      // check right
      for (s32_t i = x+1; i < grid.w; ++i){
        u8_t tree_to_check = aoc22_grid_get(&grid, i, y);
   //   printf("checking: %c vs %c\n", current_tree, tree_to_check);
        if (tree_to_check >= current_tree)
        {
          ++blocked_sides;
          break;
        }
      }

      // check left
      for (s32_t i = x-1; i >= 0; --i){
        u8_t tree_to_check = aoc22_grid_get(&grid, i, y);
        if (tree_to_check >= current_tree)
        {
          ++blocked_sides;
          break;
        }
      }
      
      // check down
      for (s32_t i = y+1; i < grid.h; ++i){
        u8_t tree_to_check = aoc22_grid_get(&grid, x, i);
        if (tree_to_check >= current_tree)
        {
          ++blocked_sides;
          break;
        }
      }
      
      //check up
      for (s32_t i = y-1; i >= 0; --i){
        u8_t tree_to_check = aoc22_grid_get(&grid, x, i);
        if (tree_to_check >= current_tree)
        {
          ++blocked_sides;
          break;
        }
      }
      if (blocked_sides != 4)
        ++sum;
      //printf("%c", aoc22_grid_get(&grid, x, y));
    }
  }

  printf("%d\n", sum);
}

int main(int argv, char** argc) {
  if (argv < 2) {
    printf("Usage: aoc22 <day> <part> <filename>\nExample: aoc22 1 1 input.txt\n");
    return 1;
  }

  u32_t day = 0;
  u32_t part = 0;
  char* filename = argc[3];

  if (!buffer_to_u32(buffer_from_cstr(argc[1]), &day)) {
    printf("Bad day\n");
    return 1;
  }

  if (!buffer_to_u32(buffer_from_cstr(argc[2]), &part)) {
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
  aoc22_route(5,2);
  aoc22_route(6,1);
  aoc22_route(6,2);
  aoc22_route(7,1);
  aoc22_route(7,2);
  aoc22_route(8,1);

}
