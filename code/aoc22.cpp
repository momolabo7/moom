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
    for(u32_t i = 0; i < lhs.size; ++i)
    {
      for(u32_t j = 0; j < rhs.size; ++j)
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
    for(u32_t i = 0; i < line0.size; ++i)
    {
      for(u32_t j = 0; j < line1.size; ++j)
      {
        for(u32_t k = 0; k < line2.size; ++k)
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
    for(u32_t i = 0; i < arr0.size; ++i)
    {
      arena_set_revert_point(arena);
      buffer_arr_t arr1 = buffer_split(arr0.e[i], '-', arena);
      for(u32_t j = 0; j < arr1.size; ++j)
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
    for(u32_t i = 0; i < arr0.size; ++i)
    {
      arena_set_revert_point(arena);
      buffer_arr_t arr1 = buffer_split(arr0.e[i], '-', arena);
      for(u32_t j = 0; j < arr1.size; ++j)
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

      for(u32_t i = 0; i < amount; ++i)
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
      for(u32_t i = 0; i < amount; ++i)
      {
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
    for(u32_t buffer_index = 0; buffer_index < str8.size - window_size - 1; ++buffer_index)
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
    for(u32_t buffer_index = 0; buffer_index < str8.size - window_size - 1; ++buffer_index)
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
  for(u32_t i = 0; i < str8.size; ++i)
  {
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
  for(u32_t i = 0; i < level; ++i)
    printf(" ");
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
      for(u32_t i = 0; i < level; ++i)
        printf(" ");
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

  for(u32_t y = 1; y < grid.h-1; ++y)
  {
    for(u32_t x = 1; x < grid.w-1; ++x)
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

static void 
aoc22_d8p2(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  aoc22_grid_t grid;
  aoc22_grid_init(&grid, file_buffer);

  u32_t max = 0;

  for(u32_t y = 0; y < grid.h; ++y)
  {
    for(u32_t x = 0; x < grid.w; ++x)
    {
      u32_t current_score = 1; 
      u32_t score = 0;
      u8_t current_tree = aoc22_grid_get(&grid, x, y);

      // check right
      for (s32_t i = x+1; i < grid.w; ++i){
        ++score;
        u8_t tree_to_check = aoc22_grid_get(&grid, i, y);
        if (tree_to_check >= current_tree)
        {
          break;
        }
      }
      current_score *= score;

      // check left
      score = 0;
      for (s32_t i = x-1; i >= 0; --i){
        ++score;
        u8_t tree_to_check = aoc22_grid_get(&grid, i, y);
        if (tree_to_check >= current_tree)
        {
          break;
        }
      }
      current_score *= score;
      
      // check down
      score = 0;
      for (s32_t i = y+1; i < grid.h; ++i)
      {
        ++score;
        u8_t tree_to_check = aoc22_grid_get(&grid, x, i);
        if (tree_to_check >= current_tree)
        {
          break;
        }
      }
      current_score *= score;
      
      //check up
      score = 0;
      for (s32_t i = y-1; i >= 0; --i){
        ++score;
        u8_t tree_to_check = aoc22_grid_get(&grid, x, i);
        if (tree_to_check >= current_tree)
        {
          break;
        }
      }
      current_score *= score;
      max = max_of(max, current_score);
    }
  }

  printf("%d\n", max);
}

static void 
aoc22_d9p1(const char* filename, arena_t* arena) 
{

  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  // @note: x goes right and y goes down
  s32_t head_x = 0;
  s32_t head_y = 0;

  s32_t tail_x = 0;
  s32_t tail_y = 0;

  u32_t position_count = 0;
  v2s_t* positions = arena_push_arr(v2s_t, arena, position_count+1);
  positions[position_count++] = v2s_t { tail_x, tail_y };

  // Initialize root first
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);  
    u8_t command = line.e[0];
    u32_t amount = 0;
    buffer_t amount_str = buffer_set(line.e + 2, line.size - 2);
    buffer_to_u32(amount_str, &amount);

    for(u32_t i = 0; i < amount; ++i)
    {

      if (command == 'R')
      {
        head_x++;
      }
      else if (command == 'U')
      {
        head_y--;
      }
      else if (command == 'L')
      {
        head_x--;
      }
      else if (command == 'D')
      {
        head_y++;
      }


      if ((head_x - tail_x) == 2)
      {
        tail_x++;
        if (tail_y != head_y) 
          tail_y = head_y;
      }
      else if ((head_x - tail_x) == -2)
      {
        tail_x--;
        if (tail_y != head_y) 
          tail_y = head_y;
      }
      else if ((head_y - tail_y) == 2)
      {
        tail_y++;
        if (tail_x != head_x) 
          tail_x = head_x;
      }
      else if ((head_y - tail_y) == -2)
      {
        tail_y--;
        if (tail_x != head_x) 
          tail_x = head_x;
      }

      // @note: ah fuck it whatever...
      b32_t found = false;
      for(u32_t i = 0; i < position_count; ++i)
      {
        if (tail_x == positions[i].x &&
            tail_y == positions[i].y)
        {
          found = true;
          break;
        }

      }
      if (!found)
      {
        arena_grow_arr(
            v2s_t, 
            arena, 
            positions, 
            position_count, 
            position_count+1);
        positions[position_count++] = v2s_t { tail_x, tail_y };
      }

    }
  }

  printf("%d\n", position_count);
}


static void 
aoc22_d9p2(const char* filename, arena_t* arena) 
{

  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  // @note: x goes right and y goes down
  v2s_t rope_nodes[10] = {};

  u32_t position_count = 0;
  v2s_t* positions = arena_push_arr(v2s_t, arena, position_count+1);
  positions[position_count++] = v2s_t { 0, 0 };

  // Initialize root first
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);  
    u8_t command = line.e[0];
    u32_t amount = 0;
    buffer_t amount_str = buffer_set(line.e + 2, line.size - 2);
    buffer_to_u32(amount_str, &amount);

    for(u32_t i = 0; i < amount; ++i)
    {
      v2s_t* front_node = rope_nodes;
      v2s_t* back_node = 0;

      if (command == 'R')
      {
        front_node->x++;
      }
      else if (command == 'U')
      {
        front_node->y--;
      }
      else if (command == 'L')
      {
        front_node->x--;
      }
      else if (command == 'D')
      {
        front_node->y++;
      }

      for(u32_t node_index = 1; 
          node_index < array_count(rope_nodes); 
          ++node_index)
      {
        front_node = rope_nodes + node_index - 1;
        back_node = rope_nodes + node_index;

        if ((front_node->x - back_node->x) == 2)
        {
          back_node->x++;
          if (back_node->y < front_node->y) 
            back_node->y++;
          else if (back_node->y > front_node->y)
            back_node->y--;
        }
        else if ((front_node->x - back_node->x) == -2)
        {
          back_node->x--;
          if (back_node->y < front_node->y) 
            back_node->y++;
          else if (back_node->y > front_node->y)
            back_node->y--;
        }
        else if ((front_node->y - back_node->y) == 2)
        {
          back_node->y++;
          if (back_node->x < front_node->x) 
            back_node->x++;
          else if (back_node->x > front_node->x)
            back_node->x--;
        }
        else if ((front_node->y - back_node->y) == -2)
        {
          back_node->y--;
          if (back_node->x < front_node->x) 
            back_node->x++;
          else if (back_node->x > front_node->x)
            back_node->x--;
        }
      }
#if 0
      for_arr(i, rope_nodes)
      {
        printf("%d: %d %d\n", i, rope_nodes[i].x, rope_nodes[i].y); 
      }
      printf("\n");
#endif
      v2s_t* tail_node = rope_nodes + array_count(rope_nodes) - 1;
      //printf("%d %d\n", tail_node->x, tail_node->y);

      // @note: ah fuck it whatever...
      b32_t found = false;
      for(u32_t i = 0; i < position_count; ++i)
      {
        if (tail_node->x == positions[i].x &&
            tail_node->y == positions[i].y)
        {
          found = true;
          break;
        }

      }

      if (!found)
      {
        arena_grow_arr(
            v2s_t, 
            arena, 
            positions, 
            position_count, 
            position_count+1);
        positions[position_count++] = v2s_t { tail_node->x, tail_node->y };
      }

    }
  }

  printf("%d\n", position_count);
}

static void 
aoc22_d10p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t x = 1;
  s32_t x_to_add = 0;
  u32_t cycles = 1;
  u32_t delay = 0;
  u32_t sum = 0;

  // Initialize root first
  for(;!stream_is_eos(s); ++cycles) 
  {
    if (delay == 0) 
    {
      buffer_t line = stream_consume_line(s);  
      buffer_t command = buffer_set(line.e, 4);
      x += x_to_add;
      x_to_add = 0;

      if (buffer_match(command, buffer_from_lit("addx")))
      {
        buffer_t amount_str = buffer_set(line.e + 5, line.size - 5);
        buffer_to_s32(amount_str, &x_to_add);
        delay = 1;
        //printf("addx: %d\n", x_to_add);
      }
      else {
        //printf("noop\n");
      }
    }
    else 
    {
      delay--;
    }

    if (cycles >= 20)
    {
      if ((cycles - 20) % 40 == 0)
      {
        //printf("cycle %03d: %d -> %d\n", cycles, x, cycles * x);
        sum += cycles * x;
      }
    }
  }
  printf("%d\n", sum);
}


static void 
aoc22_d10p2(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t x = 1;
  s32_t x_to_add = 0;
  u32_t cycles = 0;
  u32_t delay = 0;

  // Initialize root first
  for(;!stream_is_eos(s); ++cycles) 
  {
    if (delay == 0) 
    {
      buffer_t line = stream_consume_line(s);  
      buffer_t command = buffer_set(line.e, 4);
      x += x_to_add;
      x_to_add = 0;

      if (buffer_match(command, buffer_from_lit("addx")))
      {
        buffer_t amount_str = buffer_set(line.e + 5, line.size - 5);
        buffer_to_s32(amount_str, &x_to_add);
        delay = 1;
        //printf("addx: %d\n", x_to_add);
      }
      else {
        //printf("noop\n");
      }
    }
    else 
    {
      delay--;
    }

    if (cycles > 0 && cycles % 40 == 0)
    {
      printf("\n");
    }
    u32_t current_pixel_index = cycles % 40;
    if (current_pixel_index == x ||
        current_pixel_index == x-1 ||
        current_pixel_index == x+1)
    {
      printf("#");
    }
    else {
      printf(".");
    }
     
  }
  printf("\n");
}

struct aoc22_d11_item_node_t
{
  u64_t worry_level;

  aoc22_d11_item_node_t* next;
  aoc22_d11_item_node_t* prev;
};

struct aoc22_d11_monkey_t
{
  u8_t operation_type; // '+' or '*'
  u32_t operation_value; // let 0 be a special case, where it's old*old

  u32_t test_value;
  u32_t throw_to_index_if_true;
  u32_t throw_to_index_if_false;

  u64_t inspect_count;
  aoc22_d11_item_node_t item_sentinel;
};


static void 
aoc22_d11p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t item_count = 0;
  u32_t monkey_count = 0;

  // One pass to find out how many monkeys and items there are
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);
    if (line.e[0] == 'M') 
    {
      ++monkey_count;
    }
    else if (line.e[2] == 'S')
    {
      //@note: we can safely assume that all monkeys have at least 1 item
      ++item_count;
      for(u32_t i = 18; i < line.size; ++i)
      {
        if (line.e[i] == ',')
          ++item_count;
      }
    }
  }

  auto* items = arena_push_arr_zero(aoc22_d11_item_node_t, arena, item_count);
  auto* monkeys = arena_push_arr_zero(aoc22_d11_monkey_t, arena, monkey_count);
  for(u32_t i = 0; i < monkey_count; ++i)
    cll_init(&monkeys[i].item_sentinel);

  stream_reset(s);
  u32_t monkey_index = 0;
  u32_t item_index = 0;
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);
    if (line.e[2] == 'S') // "Starting items"
    {
      u32_t start = 18;
      for(u32_t end = start+1; end < line.size; ++end)
      {
        if (line.e[end] == ',') 
        {
          aoc22_d11_item_node_t* item = items + item_index++;
          aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
          buffer_to_u64(buffer_set(line.e + start, end-start), &item->worry_level);
          cll_push_back(&monkey->item_sentinel, item);
          start = end += 2;
        }
      }
      aoc22_d11_item_node_t* item = items + item_index++;
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u64(buffer_set(line.e + start, line.size-start), &item->worry_level);
      cll_push_back(&monkey->item_sentinel, item);
    }
    else if (line.e[2] == 'O') // "Operation"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      monkey->operation_type = line.e[23];

      // @note: if this fails, operation_value will be 0, which will indicate old*old
      buffer_to_u32(buffer_set(line.e + 25, line.size - 25), &monkey->operation_value);
    }
    else if (line.e[2] == 'T') // "Test"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u32(buffer_set(line.e + 21, line.size - 21), &monkey->test_value);


    }
    else if (line.e[7] == 't') // "if true"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u32(buffer_set(line.e + 29, line.size - 29), &monkey->throw_to_index_if_true);
    }
    else if (line.e[7] == 'f') // "if false"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u32(buffer_set(line.e + 30, line.size - 30), &monkey->throw_to_index_if_false);

      ++monkey_index; // this is the last instruction
    }
  }

  // simulation
  for(u32_t round = 0; round < 20; ++round)
  {
    for(u32_t monkey_index = 0; monkey_index < monkey_count; ++monkey_index)
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      while(!cll_is_empty(&monkey->item_sentinel))
      {
        monkey->inspect_count++;
        aoc22_d11_item_node_t* item = cll_first(&monkey->item_sentinel);

        if (monkey->operation_type == '*')
        {
          // @note: operation value might be 0, in which it's old*old
          item->worry_level *= ((monkey->operation_value == 0) ? item->worry_level : monkey->operation_value);
        }
        else if (monkey->operation_type == '+')
        {
          // @note: operation value should always be > 0
          item->worry_level += monkey->operation_value;
        }
        item->worry_level /= 3;
        cll_remove(item);
        if (item->worry_level % monkey->test_value == 0) // is divisible
        {
          cll_push_back(&monkeys[monkey->throw_to_index_if_true].item_sentinel, item);
        }
        else
        {
          cll_push_back(&monkeys[monkey->throw_to_index_if_false].item_sentinel, item);
        }

      }

    }

  }


#if 0
  for(u32_t i = 0; i < monkey_count; ++i)
  {
    aoc22_d11_monkey_t* monkey = monkeys + i;
    printf("monkey %d:\n", i);
    printf("  items: ");
    cll_foreach(itr, &monkey->item_sentinel)
    {
      printf("%d ", itr->worry_level);
    }
    printf("\n");
    printf("  operation: %c %d\n", monkey->operation_type, monkey->operation_value);
    printf("  test: %d\n", monkey->test_value);
    printf("  true: %d\n", monkey->throw_to_index_if_true);
    printf("  false: %d\n", monkey->throw_to_index_if_false);
  }
#endif

  u32_t active_monkey_0 = 0;
  u32_t active_monkey_1 = 0;

  for(u32_t i = 0; i < monkey_count; ++i)
  {
    if (monkeys[i].inspect_count > monkeys[active_monkey_0].inspect_count)
    {
      active_monkey_0 = i;
    }
  }
  if (active_monkey_0 == 0) active_monkey_1 = 1;

  for(u32_t i = 0; i < monkey_count; ++i)
  {
    if (i == active_monkey_0) continue;

    if (monkeys[i].inspect_count > monkeys[active_monkey_1].inspect_count )
    {
      active_monkey_1 = i;
      break;
    }
  }
  printf("%d\n", monkeys[active_monkey_0].inspect_count * monkeys[active_monkey_1].inspect_count);

}

// @note:
//
// There's apparently a smart way and dumb way to do this.
// 
// The dumb (but scalable) way is to just use a growing bigint
//
// The smarter way (but not scalable way) is apparently to 
// realize that we need to do the 'tests' through checking if
// something is divisible. Then we should be able to find a number
// in which the nature of what we are checking starts repeating. 
// Okay the wording is terrible but let's use examples.
//
// Let's say we are checking divisibility by 2. 
// Then:
//   0 % 2 = 0
//   1 % 2 = 1
//   2 % 2 = 0
//   3 % 2 = 1
//   4 % 2 = 0
//   5 % 2 = 1
// We can say that checking checking for 0 is the same as checking
// for 2 is the same as checking for 4. Likewise for 1 -> 3 -> 5.
//
// This expands when we check for 2 numbers. Let's say now we are
// checking for divisibility by 2 and 3:
//   0  % 2 = 0   0  % 3 = 0
//   1  % 2 = 1   1  % 3 = 1
//   2  % 2 = 0   2  % 3 = 2
//   3  % 2 = 1   3  % 3 = 0
//   4  % 2 = 0   4  % 3 = 1
//   5  % 2 = 1   5  % 3 = 2
//
//   6  % 2 = 0   6  % 3 = 0
//   7  % 2 = 1   7  % 3 = 1
//   8  % 2 = 0   8  % 3 = 2
//   9  % 2 = 1   9  % 3 = 0
//   10 % 2 = 0   10 % 3 = 1
//   11 % 2 = 1   11 % 3 = 2
//
//
//   Notice that the combination of results between %2 and %3 repeats
//   after 6, which means that checking for 6 is the same for checking
//   for 0, checking for 7 is the same for checking for 1, etc.
//
//   This means that we don't have to store numbers like 11; we can just
//   store it as 5, and we get that but doing 11 % 6!
//
//   And 6 is simply 2 * 3. 
//   Thus, we should be able to just get all the numbers we are interested to 
//   divide, multiply them together, and modulo that value on our worry level of
//   our items after result of the operation. 
//
//   (technically we can use the LCM of all the numbers too!)
// 
static void 
aoc22_d11p2(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t item_count = 0;
  u32_t monkey_count = 0;
  u32_t supermodulo = 1;

  // One pass to find out how many monkeys and items there are
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);
    if (line.e[0] == 'M') 
    {
      ++monkey_count;
    }
    else if (line.e[2] == 'S')
    {
      //@note: we can safely assume that all monkeys have at least 1 item
      ++item_count;
      for(u32_t i = 18; i < line.size; ++i)
      {
        if (line.e[i] == ',')
          ++item_count;
      }
    }
  }

  auto* items = arena_push_arr_zero(aoc22_d11_item_node_t, arena, item_count);
  auto* monkeys = arena_push_arr_zero(aoc22_d11_monkey_t, arena, monkey_count);
  for(u32_t i = 0; i < monkey_count; ++i)
    cll_init(&monkeys[i].item_sentinel);

  stream_reset(s);
  u32_t monkey_index = 0;
  u32_t item_index = 0;
  while(!stream_is_eos(s)) 
  {
    buffer_t line = stream_consume_line(s);
    if (line.e[2] == 'S') // "Starting items"
    {
      u32_t start = 18;
      for(u32_t end = start+1; end < line.size; ++end)
      {
        if (line.e[end] == ',') 
        {
          aoc22_d11_item_node_t* item = items + item_index++;
          aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
          buffer_to_u64(buffer_set(line.e + start, end-start), &item->worry_level);
          cll_push_back(&monkey->item_sentinel, item);
          start = end += 2;
        }
      }
      aoc22_d11_item_node_t* item = items + item_index++;
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u64(buffer_set(line.e + start, line.size-start), &item->worry_level);
      cll_push_back(&monkey->item_sentinel, item);
    }
    else if (line.e[2] == 'O') // "Operation"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      monkey->operation_type = line.e[23];

      // @note: if this fails, operation_value will be 0, which will indicate old*old
      buffer_to_u32(buffer_set(line.e + 25, line.size - 25), &monkey->operation_value);
    }
    else if (line.e[2] == 'T') // "Test"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u32(buffer_set(line.e + 21, line.size - 21), &monkey->test_value);
      supermodulo *= monkey->test_value;
    }
    else if (line.e[7] == 't') // "if true"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u32(buffer_set(line.e + 29, line.size - 29), &monkey->throw_to_index_if_true);
    }
    else if (line.e[7] == 'f') // "if false"
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      buffer_to_u32(buffer_set(line.e + 30, line.size - 30), &monkey->throw_to_index_if_false);

      ++monkey_index; // this is the last instruction
    }
  }

  // simulation
  for(u32_t round = 0; round < 10000; ++round)
  {
    for(u32_t monkey_index = 0; monkey_index < monkey_count; ++monkey_index)
    {
      aoc22_d11_monkey_t* monkey = monkeys + monkey_index;
      while(!cll_is_empty(&monkey->item_sentinel))
      {
        monkey->inspect_count++;
        aoc22_d11_item_node_t* item = cll_first(&monkey->item_sentinel);

        if (monkey->operation_type == '*')
        {
          // @note: operation value might be 0, in which it's old*old
          u64_t value_to_multiply = ((monkey->operation_value == 0) ? item->worry_level : monkey->operation_value);
          item->worry_level *= value_to_multiply;
        }
        else if (monkey->operation_type == '+')
        {
          // @note: operation value should always be > 0
          item->worry_level += monkey->operation_value;
        }
        item->worry_level %= supermodulo;
        cll_remove(item);
        if (item->worry_level % monkey->test_value == 0) // is divisible
        {
          cll_push_back(&monkeys[monkey->throw_to_index_if_true].item_sentinel, item);
        }
        else
        {
          cll_push_back(&monkeys[monkey->throw_to_index_if_false].item_sentinel, item);
        }

#if 0
        printf("Round End Inspect Count:\n");
        for(u32_t i = 0; i < monkey_count; ++i)
        {
          aoc22_d11_monkey_t* monkey = monkeys + i;
          printf("monkey %d: ", i);
          cll_foreach(itr, &monkey->item_sentinel)
          {
            printf("%d ", itr->worry_level);
          }
          printf("\n");
        }
#endif
      }

#if 0
      for(u32_t i = 0; i < monkey_count; ++i)
      {
        printf("monkey %u: %u\n", i, monkeys[i].inspect_count);
      }
#endif
    }

  }



  u32_t active_monkey_0 = 0;
  u32_t active_monkey_1 = 0;

  for(u32_t i = 0; i < monkey_count; ++i)
  {
    if (monkeys[i].inspect_count > monkeys[active_monkey_0].inspect_count)
    {
      active_monkey_0 = i;
    }
  }
  if (active_monkey_0 == 0) active_monkey_1 = 1;

  for(u32_t i = 0; i < monkey_count; ++i)
  {
    if (i == active_monkey_0) continue;

    if (monkeys[i].inspect_count > monkeys[active_monkey_1].inspect_count )
    {
      active_monkey_1 = i;
      break;
    }
  }
  printf("%llu\n", monkeys[active_monkey_0].inspect_count * monkeys[active_monkey_1].inspect_count);


}

struct aoc22_d12p1_vertex_t
{
  u32_t x;
  u32_t y;
  u8_t symbol;
  u32_t height;
  u32_t distance;
  b32_t done;
  aoc22_d12p1_vertex_t* prev;
};

struct aoc22_d12p1_grid_t 
{
  aoc22_d12p1_vertex_t* vertices;
  u32_t vertex_count;
  u32_t width, height;
};

static void
aoc22_d12p1_grid_init(aoc22_d12p1_grid_t* grid, buffer_t buffer, arena_t* arena)
{
  stream_t s;
  stream_init(&s, buffer);

  // figure out width
  {
    buffer_t line = stream_consume_line(&s);  
    grid->width = line.size;
  }

  stream_reset(&s);
  
  // figure out height
  grid->height = 0;
  while(!stream_is_eos(&s)) 
  {
    stream_consume_line(&s);  
    ++grid->height;
  }
  stream_reset(&s);

  grid->vertex_count = grid->width * grid->height;
  grid->vertices = arena_push_arr(aoc22_d12p1_vertex_t, arena, grid->vertex_count);
  assert(grid->vertices);
  
  u32_t vertex_index = 0;
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  

    for(u32_t i = 0; i < line.size; ++i)
    {
      aoc22_d12p1_vertex_t* vertex = grid->vertices + vertex_index;
      vertex->symbol = line.e[i];
      vertex->prev = nullptr;
      vertex->done = false;
      vertex->x = vertex_index % grid->width;
      vertex->y = vertex_index / grid->width;

      if (vertex->symbol == 'S')
      {
        vertex->distance =  0;

        // 'S' is 0.
        vertex->height = 0; 
      }
      else if (vertex->symbol == 'E')
      {
        // 'E' should be 27.
        vertex->height = 'z' - 'a' + 2;
        vertex->distance = U32_MAX;
      }
      else 
      {
        vertex->distance = U32_MAX;

        // 'a' is 1
        // 'b' is 2
        // ...etc
        vertex->height = vertex->symbol - 'a' + 1;
      }
      vertex_index++;
    }
  }
}

static void
aoc22_d12p1_grid_pathfind_recalculate_neighbour_vertex(
    aoc22_d12p1_grid_t* grid, 
    aoc22_d12p1_vertex_t* current_vertex,
    u32_t neighbour_vertex_x,
    u32_t neighbour_vertex_y)
{
  aoc22_d12p1_vertex_t* neighbour = grid->vertices + (neighbour_vertex_x + neighbour_vertex_y * grid->width);
  b32_t can_climb = (neighbour->height-1 <= current_vertex->height);
  if(!neighbour->done && can_climb)
  {
    u32_t new_distance = current_vertex->distance + 1;
    if (new_distance < neighbour->distance) 
    {
      neighbour->distance = new_distance; 
      neighbour->prev = current_vertex;
    }
  }
}

static u32_t 
aoc22_d12p1_grid_pathfind(aoc22_d12p1_grid_t* grid)
{
  u32_t vertices_left = grid->vertex_count;
  while (vertices_left > 0)
  {
    // find vertex with smallest dist
    u32_t current_vertex_index = 0;
    {
      u32_t min = U32_MAX;
      for(u32_t vertex_index = 0; vertex_index < grid->vertex_count; ++vertex_index)
      {
        aoc22_d12p1_vertex_t* itr = grid->vertices + vertex_index;
        if (itr->done) continue;

        if (itr->distance < min)
        {
          min = itr->distance;
          current_vertex_index = vertex_index;
        }
      }
    }
    aoc22_d12p1_vertex_t* current_vertex = grid->vertices + current_vertex_index;

    // terminate case
    //printf("symbol[%d,%d]: %c, %d\n", current_vertex->x, current_vertex->y, current_vertex->symbol, current_vertex->distance);
    if (current_vertex->symbol == 'E')
    {
      //printf("E found\n");
      auto* itr = current_vertex->prev;
      u32_t sum = 0;
      while(itr)
      {
        // printf("symbol[%d,%d]: %c, %d\n", itr->x, itr->y, itr->symbol, itr->distance);
        itr = itr->prev;
        ++sum;
      }
      return sum;
    }
    
    // remove the smallest vertex from the list 
    current_vertex->done = true; // emulates removal
    vertices_left--;


    // left
    if (current_vertex->x > 0)
    {
      aoc22_d12p1_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x - 1, 
          current_vertex->y);
    }
    //right
    if (current_vertex->x < grid->width-1)
    {
      aoc22_d12p1_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x + 1, 
          current_vertex->y);
    }
    // up
    if (current_vertex->y > 0)
    {
      aoc22_d12p1_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x, 
          current_vertex->y - 1);
    }
    // down
    if (current_vertex->y < grid->height-1)
    {
      aoc22_d12p1_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x, 
          current_vertex->y + 1);
    }
  }
  return 0;
}


static void 
aoc22_d12p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  aoc22_d12p1_grid_t grid;
  aoc22_d12p1_grid_init(&grid, file_buffer, arena); 
  u32_t steps = aoc22_d12p1_grid_pathfind(&grid);
  printf("%d\n", steps);
}

struct aoc22_d12p2_vertex_t
{
  u32_t x;
  u32_t y;
  u8_t symbol;
  u32_t height;
  u32_t distance;
  b32_t done;
  aoc22_d12p2_vertex_t* prev;
};

struct aoc22_d12p2_grid_t 
{
  aoc22_d12p2_vertex_t* vertices;
  u32_t vertex_count;
  u32_t width, height;

  u32_t* starts;
  u32_t start_count;
};

static void
aoc22_d12p2_grid_init(aoc22_d12p2_grid_t* grid, buffer_t buffer, arena_t* arena)
{
  stream_t s;
  stream_init(&s, buffer);

  // figure out width
  {
    buffer_t line = stream_consume_line(&s);  
    grid->width = line.size;
  }

  stream_reset(&s);
  
  // figure out height
  // at the same time, figure out how many starting points
  grid->height = 0;
  grid->start_count = 0;
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  
    for(u32_t i = 0; i < line.size; ++i)
    {
      if (line.e[i] == 'S' || line.e[i] == 'a')
        ++grid->start_count;
    }
    ++grid->height;
  }
  stream_reset(&s);

  grid->vertex_count = grid->width * grid->height;
  grid->vertices = arena_push_arr(aoc22_d12p2_vertex_t, arena, grid->vertex_count);
  grid->starts = arena_push_arr(u32_t, arena, grid->start_count);
  assert(grid->starts);
  assert(grid->vertices);
  
  u32_t vertex_index = 0;
  u32_t start_index = 0;
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  

    for(u32_t i = 0; i < line.size; ++i)
    {
      aoc22_d12p2_vertex_t* vertex = grid->vertices + vertex_index;
      vertex->symbol = line.e[i];
      vertex->prev = nullptr;
      vertex->done = false;
      vertex->x = vertex_index % grid->width;
      vertex->y = vertex_index / grid->width;
      vertex->distance = U32_MAX;

      if (vertex->symbol == 'S')
      {
        vertex->symbol = 'a';
      }


      if (vertex->symbol == 'E')
      {
        // 'E' should be 27.
        vertex->height = 'z' - 'a' + 2;
      }
      else 
      {
        // 'a' is 1
        // 'b' is 2
        // ...etc
        vertex->height = vertex->symbol - 'a' + 1;

        if (vertex->symbol == 'a')
        {
          grid->starts[start_index++] = vertex_index;
        }
      }
      vertex_index++;
    }
  }

}

static void
aoc22_d12p2_grid_pathfind_recalculate_neighbour_vertex(
    aoc22_d12p2_grid_t* grid, 
    aoc22_d12p2_vertex_t* current_vertex,
    u32_t neighbour_vertex_x,
    u32_t neighbour_vertex_y)
{
  aoc22_d12p2_vertex_t* neighbour = grid->vertices + (neighbour_vertex_x + neighbour_vertex_y * grid->width);
  b32_t can_climb = (neighbour->height-1 <= current_vertex->height);
  if(!neighbour->done && can_climb)
  {
    u32_t new_distance = current_vertex->distance + 1;
    if (new_distance < neighbour->distance) 
    {
      neighbour->distance = new_distance; 
      neighbour->prev = current_vertex;
    }
  }
}


static u32_t 
aoc22_d12p2_grid_pathfind_sub(aoc22_d12p2_grid_t* grid)
{
  u32_t vertices_left = grid->vertex_count;
  while (vertices_left > 0)
  {
    // find vertex with smallest dist
    u32_t current_vertex_index = 0;
    {
      u32_t min = U32_MAX;
      for(u32_t vertex_index = 0; vertex_index < grid->vertex_count; ++vertex_index)
      {
        aoc22_d12p2_vertex_t* itr = grid->vertices + vertex_index;
        if (itr->done) continue;

        if (itr->distance < min)
        {
          min = itr->distance;
          current_vertex_index = vertex_index;
        }
      }
    }
    aoc22_d12p2_vertex_t* current_vertex = grid->vertices + current_vertex_index;

    // terminate case
    //printf("symbol[%d,%d]: %c, %d\n", current_vertex->x, current_vertex->y, current_vertex->symbol, current_vertex->distance);
    if (current_vertex->symbol == 'E')
    {
      //printf("E found\n");
      auto* itr = current_vertex->prev;
      u32_t sum = 0;
      while(itr)
      {
        // printf("symbol[%d,%d]: %c, %d\n", itr->x, itr->y, itr->symbol, itr->distance);
        itr = itr->prev;
        ++sum;
      }
      return sum;
    }
    
    // remove the smallest vertex from the list 
    current_vertex->done = true; // emulates removal
    vertices_left--;


    // left
    if (current_vertex->x > 0)
    {
      aoc22_d12p2_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x - 1, 
          current_vertex->y);
    }
    //right
    if (current_vertex->x < grid->width-1)
    {
      aoc22_d12p2_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x + 1, 
          current_vertex->y);
    }
    // up
    if (current_vertex->y > 0)
    {
      aoc22_d12p2_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x, 
          current_vertex->y - 1);
    }
    // down
    if (current_vertex->y < grid->height-1)
    {
      aoc22_d12p2_grid_pathfind_recalculate_neighbour_vertex(
          grid, 
          current_vertex, 
          current_vertex->x, 
          current_vertex->y + 1);
    }
  }
  return 0;
}

static u32_t 
aoc22_d12p2_grid_pathfind(aoc22_d12p2_grid_t* grid)
{
  u32_t min = U32_MAX;
  for(u32_t start_index = 0; start_index < grid->start_count; ++start_index)
  {
    // reset all vertices to U32_MAX
    for(u32_t vertex_index = 0; vertex_index < grid->vertex_count; ++vertex_index)
    {
      aoc22_d12p2_vertex_t* vertex = grid->vertices + vertex_index;
      vertex->distance = U32_MAX;
      vertex->done = false;
      vertex->prev = nullptr;
    }
    // initialize the starting vertex
    u32_t start_vertex_index = grid->starts[start_index];
    aoc22_d12p2_vertex_t* vertex = grid->vertices + start_vertex_index;
    vertex->distance = 0;

    min = min_of(min, aoc22_d12p2_grid_pathfind_sub(grid));
  }
  return min;
}

static void 
aoc22_d12p2(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  aoc22_d12p2_grid_t grid;
  aoc22_d12p2_grid_init(&grid, file_buffer, arena); 
  u32_t steps = aoc22_d12p2_grid_pathfind(&grid);
  printf("%d\n", steps);
}

enum aoc22_d13_node_type_t
{
  AOC22_D13_NODE_TYPE_NIEN,

  AOC22_D13_NODE_TYPE_INTEGER,
  AOC22_D13_NODE_TYPE_LIST,
};

struct aoc22_d13_node_t
{
  aoc22_d13_node_type_t type;
  union 
  {
    u32_t integer; // represents integer
    aoc22_d13_node_t* list; // represents a list
  };
  aoc22_d13_node_t* prev;
  aoc22_d13_node_t* next;

  b32_t is_divider; // for day 2
};

static void 
aoc22_d13_node_print(aoc22_d13_node_t* node)
{
  cll_foreach(itr, node)
  {
    if (itr->type == AOC22_D13_NODE_TYPE_LIST)
    {
      printf("[");
      aoc22_d13_node_print(itr->list);
      printf("]");
    }
    else 
    {
      // Integer
      printf("(%u)", itr->integer);
    }
  }
}


static u32_t
aoc22_d13_parse_list(aoc22_d13_node_t* root, buffer_t str, u32_t start_character_index, arena_t* arena)
{
  for(u32_t character_index = start_character_index; character_index < str.size; ++character_index)
  {
    u8_t c = str.e[character_index];
    if (c == '[')
    {
      aoc22_d13_node_t* node = arena_push(aoc22_d13_node_t, arena);
      node->type = AOC22_D13_NODE_TYPE_LIST;
      node->list = arena_push_zero(aoc22_d13_node_t, arena);
      cll_init(node->list);
      cll_push_back(root, node);
      character_index = aoc22_d13_parse_list(node->list, str, character_index + 1, arena);
    }
    else if (c == ']')
    {
      // return the character index we to continue from
      return character_index;
    }
    else if (c >= '0' && c <= '9')
    {
      u32_t number = 0;
      u32_t end_index = character_index;
      while (str.e[end_index] >= '0' && str.e[end_index] <= '9') ++end_index;
      buffer_t number_as_str = buffer_set(str.e + character_index, end_index - character_index);
      b32_t success = buffer_to_u32(number_as_str, &number);
      assert(success);

      aoc22_d13_node_t * node = arena_push(aoc22_d13_node_t, arena);

      node->type = AOC22_D13_NODE_TYPE_INTEGER;
      node->integer = number;
      cll_push_back(root, node);

      character_index = end_index-1;
    }
  }
  return str.size;
}

enum aoc22_d13_compare_result_type_t
{
  AOC22_D13_COMPARE_RESULT_TYPE_CONTINUE,
  AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER,
  AOC22_D13_COMPARE_RESULT_TYPE_WRONG_ORDER,
};

static aoc22_d13_compare_result_type_t
aoc22_d13_compare_lists(aoc22_d13_node_t* left, aoc22_d13_node_t* right)
{
  aoc22_d13_node_t* left_itr = left->next;
  aoc22_d13_node_t* right_itr = right->next;
  for(;;)
  {
    // termination cases
    if (left_itr == left && right_itr == right)
    {
      return AOC22_D13_COMPARE_RESULT_TYPE_CONTINUE;
    }
    if (left_itr == left)
    {
      return AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER;
    }
    if (right_itr == right)
    {
      return AOC22_D13_COMPARE_RESULT_TYPE_WRONG_ORDER;
    }

    // normal checking cases
    if (left_itr->type == AOC22_D13_NODE_TYPE_LIST && 
        right_itr->type == AOC22_D13_NODE_TYPE_LIST)
    {
      auto result = aoc22_d13_compare_lists(left_itr->list, right_itr->list);
      if (result != AOC22_D13_COMPARE_RESULT_TYPE_CONTINUE)
        return result;
    }

    else if (left_itr->type == AOC22_D13_NODE_TYPE_INTEGER &&
             right_itr->type == AOC22_D13_NODE_TYPE_INTEGER)
    {
      if (left_itr->integer > right_itr->integer)
      {
        return AOC22_D13_COMPARE_RESULT_TYPE_WRONG_ORDER;
      }
      else if (left_itr->integer < right_itr->integer)
      {
        return AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER;
      }
      else // (left_itr->integer == right_itr->integer)
      {
        // do nothing
      }
    }
    else if (left_itr->type == AOC22_D13_NODE_TYPE_INTEGER &&
             right_itr->type == AOC22_D13_NODE_TYPE_LIST)
    {
      // recursively search until we reach an integer
      aoc22_d13_node_t* finder = right_itr;
      while(finder->type == AOC22_D13_NODE_TYPE_LIST)
      {
        if (cll_is_empty(finder->list))
        {
          return AOC22_D13_COMPARE_RESULT_TYPE_WRONG_ORDER; // terminate if list is empty
        }
        finder = finder->list->next;
      }

      assert(finder->type == AOC22_D13_NODE_TYPE_INTEGER);

      // At this point, finder should be AOC22_D13_NODE_TYPE_INTEGER
      if (finder->integer < left_itr->integer)
      {
        return AOC22_D13_COMPARE_RESULT_TYPE_WRONG_ORDER;
      }
      else 
      {
        return AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER;
      }
    }
    else 
    {
      assert(left_itr->type != AOC22_D13_NODE_TYPE_NIEN);
      assert(right_itr->type != AOC22_D13_NODE_TYPE_NIEN);
      // Mixed types. Find until they reach an integer
      aoc22_d13_node_t* left_finder = left_itr;
      aoc22_d13_node_t* right_finder = right_itr;
      while(left_finder->type == AOC22_D13_NODE_TYPE_LIST)
      {
        if (cll_is_empty(left_finder->list))
        {
          return AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER; // terminate if list is empty
        }
        left_finder = left_finder->list->next;
      }

      while(right_finder->type == AOC22_D13_NODE_TYPE_LIST)
      {
        if (cll_is_empty(right_finder->list))
        {
          return AOC22_D13_COMPARE_RESULT_TYPE_WRONG_ORDER; // terminate if list is empty
        }
        right_finder = right_finder->list->next;
      }
      assert(left_finder->type == AOC22_D13_NODE_TYPE_INTEGER);
      assert(right_finder->type == AOC22_D13_NODE_TYPE_INTEGER);

      // At this point, finder should be AOC22_D13_NODE_TYPE_INTEGER
      if (left_finder->integer < right_finder->integer)
      {
        return AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER;
      }
      else 
      {
        return AOC22_D13_COMPARE_RESULT_TYPE_WRONG_ORDER;
      }

    }

    left_itr = left_itr->next;
    right_itr = right_itr->next;
  }
}


static void 
aoc22_d13p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  stream_t s;
  stream_init(&s, file_buffer);

  aoc22_d13_node_t left = {};
  aoc22_d13_node_t right = {};

  u32_t sum = 0;

  u32_t line_index = 0;
  u32_t pair_index = 1;
  auto mark = arena_mark(arena);
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  
    switch(line_index % 3)
    {
      case 0:
      {
        cll_init(&left);
        cll_init(&right);
        aoc22_d13_parse_list(&left, line, 0, arena);  
      } break;
      case 1:
      {
        aoc22_d13_parse_list(&right, line, 0, arena);  
        b32_t result = aoc22_d13_compare_lists(&left, &right);
        if (result == AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER)
        {
          sum += pair_index;
        }
        pair_index++;
#if 0
        aoc22_d13_node_print(&left);
        printf("\n");
        aoc22_d13_node_print(&right);
        printf("\n");
        printf("%d\n", result);
        printf("\n");
#endif
        arena_revert(mark);
      } break;
      case 2:
      {
        // do nothing
      } break;

    }
    ++line_index;
  }
  
  printf("%u\n", sum);

}

sort_quick_generic_predicate_sig(aoc22_d13p2_pred)
{
  aoc22_d13_node_t* left = dref((aoc22_d13_node_t**)lhs);
  aoc22_d13_node_t* right = dref((aoc22_d13_node_t**)rhs);

  aoc22_d13_compare_result_type_t result = aoc22_d13_compare_lists(left, right);
  assert(result != AOC22_D13_COMPARE_RESULT_TYPE_CONTINUE); 
  return result == AOC22_D13_COMPARE_RESULT_TYPE_RIGHT_ORDER;

}

static void
aoc22_d13p2(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  stream_t s;
  stream_init(&s, file_buffer);


  u32_t list_count = 2; 
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  
    if (line.e[0] == '[')
      ++list_count;
  }
  stream_reset(&s);

  aoc22_d13_node_t* lists = arena_push_arr_zero(aoc22_d13_node_t, arena, list_count);
  assert(lists);

  u32_t list_itr = 0;
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  
    if (line.e[0] == '[')
    {
      cll_init(lists + list_itr);
      aoc22_d13_parse_list(lists + list_itr++, line, 0, arena); 
    }
  }
  cll_init(lists + list_itr);
  lists[list_itr].is_divider = true;
  aoc22_d13_parse_list(lists + list_itr++, buffer_from_lit("[2]"), 0, arena);
  cll_init(lists + list_itr);
  lists[list_itr].is_divider = true;
  aoc22_d13_parse_list(lists + list_itr++, buffer_from_lit("[6]"), 0, arena);

  aoc22_d13_node_t** sort_entries = arena_push_arr_zero(aoc22_d13_node_t*, arena, list_count);
  assert(sort_entries);

  for(u32_t i = 0; i < list_count; ++i)
    sort_entries[i] = (lists + i);


#if 0
  for(u32_t i = 0; i < list_count; ++i)
  {
    aoc22_d13_node_print(lists + i);
    printf("\n");
  }
#endif
  sort_quick_generic(sort_entries, list_count, aoc22_d13p2_pred);

  u32_t sum = 1;
  for(u32_t i = 0; i < list_count; ++i)
  {
    //aoc22_d13_node_print(dref(sort_entries + i));
    //printf("\n");
    if (sort_entries[i]->is_divider)
    {
      sum *= (i + 1);
    }
  }
  printf("%u\n", sum);


}

struct aoc22_d14_node_t
{
  b32_t is_divider;
  u32_t x;
  u32_t y;
  aoc22_d14_node_t* prev;
  aoc22_d14_node_t* next;
};


static void
aoc22_d14p1(const char* filename, arena_t* arena) 
{
  aoc22_d14_node_t point_list;
  cll_init(&point_list);
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  stream_t s;
  stream_init(&s, file_buffer);

  // one pass to find out what's the smallest and largest x and y value
  v2u_t min = v2u_set(U32_MAX, U32_MAX);
  v2u_t max = {};
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  

    aoc22_d14_node_t* new_node = arena_push_zero(aoc22_d14_node_t, arena);
    new_node->is_divider = true;
    cll_push_back(&point_list, new_node);
    
    u32_t chaser = 0;
    u32_t value = 0;
    for(u32_t i = 0; i < line.size; ++i)
    {
      if (line.e[i] == ',')
      {
        buffer_t substr = buffer_set(line.e + chaser, i - chaser);
        buffer_to_u32(substr, &value);
        min.x = min_of(min.x, value);
        max.x = max_of(max.x, value);
        chaser = i + 1;
        new_node = arena_push_zero(aoc22_d14_node_t, arena);
        new_node->x = value;
      }
      else if (line.e[i] == ' ')
      {
        buffer_t substr = buffer_set(line.e + chaser, i - chaser);
        buffer_to_u32(substr, &value);
        i += 3; 
        min.y = min_of(min.y, value);
        max.y = max_of(max.y, value);
        chaser = i + 1;
        new_node->y = value;
        cll_push_back(&point_list, new_node);
      }
    }
    // one more pass for y
    buffer_t substr = buffer_set(line.e + chaser, line.size-chaser);
    buffer_to_u32(substr, &value);
    min.y = min_of(min.y, value);
    max.y = max_of(max.y, value);
    new_node->y = value;
    cll_push_back(&point_list, new_node);
  }

//  printf("%u %u\n", min.x, min.y);
//  printf("%u %u\n", max.x, max.y);
  u32_t grid_w = max.x - min.x + 1;
  u32_t grid_h = max.y + 1;

  // 0 is empty
  // 1 is sand
  // 2 is obstacle
  u32_t* grid = arena_push_arr_zero(u32_t, arena, grid_w * grid_h);

  b32_t first = true;
  u32_t beg_x = 0;
  u32_t beg_y = 0;
  cll_foreach(itr, &point_list)
  {
    if (itr->is_divider) 
    {
      first = true;
      continue;
    }
    
    if (first)
    {
      first = false;
      beg_x = itr->x - min.x;
      beg_y = itr->y;
    }
    else
    {
      u32_t end_x = itr->x - min.x;
      u32_t end_y = itr->y;
      // fill between start and itr
      // lines are garunteed to be straight so need to
      // check if x or y is same
      if (beg_x == end_x)
      {
        // fill y-wards
        if (end_y < beg_y) 
          for(u32_t y = end_y; y <= beg_y; ++y)
            grid[beg_x + y * grid_w] = 1;
        else
          for(u32_t y = beg_y; y <= end_y; ++y)
            grid[beg_x + y * grid_w] = 1;
      }
      else if (beg_y == end_y)
      {
        // fill x-wards
        if (end_x < beg_x) 
          for(u32_t x = end_x; x <= beg_x; ++x)
            grid[x + beg_y * grid_w] = 1;
        else
          for(u32_t x = beg_x; x <= end_x; ++x)
            grid[x + beg_y * grid_w] = 1;
      }
      beg_x = end_x;
      beg_y = end_y;
    }
  }

  //
  // Simulation
  //
  const u32_t sand_spawn_x = 500 - min.x;
  const u32_t sand_spawn_y = 0;

  u32_t count = 0;
  for(;;)
  {
    s32_t sand_x = sand_spawn_x;
    s32_t sand_y = sand_spawn_y;
    for(;;)
    {
      if (sand_x < 0 ||
          sand_y < 0 ||
          sand_x >= grid_w-1 ||
          sand_y >= grid_h-1)
      {
        goto abyss;
      }

      u32_t down =  grid[sand_x + (sand_y + 1) * grid_w];
      if (down == 0)
      {
        sand_y++;
        continue;
      }

      u32_t left = grid[(sand_x-1) + (sand_y + 1) * grid_w];
      if (left == 0)
      {
        sand_y++;
        sand_x--;
        continue;
      }

      u32_t right = grid[(sand_x+1) + (sand_y + 1) * grid_w];
      if (right == 0)
      {
        sand_y++;
        sand_x++;
        continue;
      }

      break;
    }
    
    // come to rest
    ++count;
    grid[sand_x + sand_y * grid_w] = 2;
  }
abyss:

#if 0
  // check grid
  for(u32_t y = 0; y < grid_h; ++y)
  {
    for(u32_t x = 0; x < grid_w; ++x)
    {
      printf("%u", grid[x + y * grid_w]);
    }
    printf("\n");
  }
#endif
  printf("%u\n", count);
  
}

static void
aoc22_d14p2(const char* filename, arena_t* arena) 
{
  aoc22_d14_node_t point_list;
  cll_init(&point_list);
  arena_set_revert_point(arena);
  buffer_t file_buffer = file_read_into_buffer(filename, arena, true); 
  if (!file_buffer) return;

  stream_t s;
  stream_init(&s, file_buffer);

  // one pass to find out what's the smallest and largest x and y value
  v2u_t min = v2u_set(U32_MAX, U32_MAX);
  v2u_t max = {};
  while(!stream_is_eos(&s)) 
  {
    buffer_t line = stream_consume_line(&s);  

    aoc22_d14_node_t* new_node = arena_push_zero(aoc22_d14_node_t, arena);
    new_node->is_divider = true;
    cll_push_back(&point_list, new_node);
    
    u32_t chaser = 0;
    u32_t value = 0;
    for(u32_t i = 0; i < line.size; ++i)
    {
      if (line.e[i] == ',')
      {
        buffer_t substr = buffer_set(line.e + chaser, i - chaser);
        buffer_to_u32(substr, &value);
        min.x = min_of(min.x, value);
        max.x = max_of(max.x, value);
        chaser = i + 1;
        new_node = arena_push_zero(aoc22_d14_node_t, arena);
        new_node->x = value;
      }
      else if (line.e[i] == ' ')
      {
        buffer_t substr = buffer_set(line.e + chaser, i - chaser);
        buffer_to_u32(substr, &value);
        i += 3; 
        min.y = min_of(min.y, value);
        max.y = max_of(max.y, value);
        chaser = i + 1;
        new_node->y = value;
        cll_push_back(&point_list, new_node);
      }
    }
    // one more pass for y
    buffer_t substr = buffer_set(line.e + chaser, line.size-chaser);
    buffer_to_u32(substr, &value);
    min.y = min_of(min.y, value);
    max.y = max_of(max.y, value);
    new_node->y = value;
    cll_push_back(&point_list, new_node);
  }

  max.y += 2; // add for the infinite floor
  const u32_t x_padding = max.y;
  printf("%u\n", x_padding);
  min.x -= x_padding;
  max.x += x_padding;

  // @note: I'm not sure about how much extra horizontal space I need but I think it's 
  // related to how far the floor is to the spawn point
  u32_t grid_w = max.x - min.x + 1;
  u32_t grid_h = max.y + 1;

  // 0 is empty
  // 1 is sand
  // 2 is obstacle
  u32_t* grid = arena_push_arr_zero(u32_t, arena, grid_w * grid_h);

  b32_t first = true;
  u32_t beg_x = 0;
  u32_t beg_y = 0;
  cll_foreach(itr, &point_list)
  {
    if (itr->is_divider) 
    {
      first = true;
      continue;
    }
    
    if (first)
    {
      first = false;
      beg_x = itr->x - min.x;
      beg_y = itr->y;
    }
    else
    {
      u32_t end_x = itr->x - min.x;
      u32_t end_y = itr->y;
      // fill between start and itr
      // lines are garunteed to be straight so need to
      // check if x or y is same
      if (beg_x == end_x)
      {
        // fill y-wards
        if (end_y < beg_y) 
          for(u32_t y = end_y; y <= beg_y; ++y)
            grid[beg_x + y * grid_w] = 1;
        else
          for(u32_t y = beg_y; y <= end_y; ++y)
            grid[beg_x + y * grid_w] = 1;
      }
      else if (beg_y == end_y)
      {
        // fill x-wards
        if (end_x < beg_x) 
          for(u32_t x = end_x; x <= beg_x; ++x)
            grid[x + beg_y * grid_w] = 1;
        else
          for(u32_t x = beg_x; x <= end_x; ++x)
            grid[x + beg_y * grid_w] = 1;
      }
      beg_x = end_x;
      beg_y = end_y;
    }
  }
  // fill the last line with 1
  printf("%d\n", max.y);
  for (u32_t x = 0; x < grid_w; ++x)
  {
    grid[x + max.y * grid_w] = 1;
  }

  //
  // Simulation
  //
  const u32_t sand_spawn_x = 500 - min.x;
  const u32_t sand_spawn_y = 0;

  u32_t count = 1;
  for(;;)
  {
    s32_t sand_x = sand_spawn_x;
    s32_t sand_y = sand_spawn_y;
    for(;;)
    {
      if (sand_x < 0 ||
          sand_y < 0 ||
          sand_x >= grid_w-1 ||
          sand_y >= grid_h-1)
      {
        printf("dedge\n");
        goto abyss;
      }

      u32_t down =  grid[sand_x + (sand_y + 1) * grid_w];
      if (down == 0)
      {
        sand_y++;
        continue;
      }

      u32_t left = grid[(sand_x-1) + (sand_y + 1) * grid_w];
      if (left == 0)
      {
        sand_y++;
        sand_x--;
        continue;
      }

      u32_t right = grid[(sand_x+1) + (sand_y + 1) * grid_w];
      if (right == 0)
      {
        sand_y++;
        sand_x++;
        continue;
      }

      if (sand_x == sand_spawn_x && sand_y == sand_spawn_y)
      {
        goto abyss;
      }


      break;
    }
    
    // come to rest
    ++count;
    grid[sand_x + sand_y * grid_w] = 2;
  }
abyss:
  grid[sand_spawn_x + sand_spawn_y * grid_w] = 2;

#if 0
  // check grid
  for(u32_t y = 0; y < grid_h; ++y)
  {
    for(u32_t x = 0; x < grid_w; ++x)
    {
      printf("%u", grid[x + y * grid_w]);
    }
    printf("\n");
  }
#endif
  printf("%u\n", count);
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
  aoc22_route(8,2);
  aoc22_route(9,1);
  aoc22_route(9,2);
  aoc22_route(10,1);
  aoc22_route(10,2);
  aoc22_route(11,1);
  aoc22_route(11,2);
  aoc22_route(12,1);
  aoc22_route(12,2);
  aoc22_route(13,1);
  aoc22_route(13,2);
  aoc22_route(14,1);
  aoc22_route(14,2);

}
