#define MOMO_FOOLISH 
#include "momo.h"



static void aoc23_d1p1(const char* filename, arena_t* arena) 
{
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_str(filename, arena, false); 
  if (!file_buffer) return;


  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    buffer_t line = stream_consume_line(s);  
    u32_t number = 0;
    
    // count from the front
    for( s32_t i = 0; i <  line.size; ++i) {
      if (u8_is_digit(line.e[i])) {
        number += ascii_to_digit(line.e[i]);
        number *= 10;
        break;
      }
    }

    // count from the back
    for(s32_t i = line.size-1; i >= 0; --i) {
      if (u8_is_digit(line.e[i])) {
        number += ascii_to_digit(line.e[i]);
        break;
      }
    }
    sum += number;
  }
  printf("%u\n", sum);
}


static void aoc23_d1p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_str(filename, arena, false); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);

  buffer_t buffer_table[] = { 
    buffer_from_lit("1"), 
    buffer_from_lit("2"), 
    buffer_from_lit("3"), 
    buffer_from_lit("4"), 
    buffer_from_lit("5"), 
    buffer_from_lit("6"), 
    buffer_from_lit("7"), 
    buffer_from_lit("8"),
    buffer_from_lit("9"), 
    buffer_from_lit("one"), 
    buffer_from_lit("two"), 
    buffer_from_lit("three"), 
    buffer_from_lit("four"), 
    buffer_from_lit("five"), 
    buffer_from_lit("six"), 
    buffer_from_lit("seven"), 
    buffer_from_lit("eight"), 
    buffer_from_lit("nine") 
  };
  u32_t num_table[] = { 1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9 };

  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    buffer_t line = stream_consume_line(s);  
    u32_t number = 0;

    // count from the front
    for( s32_t i = 0; i <  line.size; ++i) {
      b32_t found = false;
      for_arr(table_index, num_table) {
        buffer_t str = buffer_table[table_index];
        u32_t num = num_table[table_index];

        buffer_t sub_line = buffer_set(line.e + i, str.size);
        if ((i + str.size-1) < line.size && 
          buffer_match(str, sub_line)) 
        {
          found = true;
          number += num * 10;
          break;
        }
      }
      if (found)
        break;
    }

    // count from the back
    for( s32_t i = line.size-1; i >= 0; --i) {
      b32_t found = false;
      for_arr(table_index, num_table) {
        buffer_t str = buffer_table[table_index];
        u32_t num = num_table[table_index];

        buffer_t sub_line = buffer_set(line.e + i - (str.size-1), str.size);
        // @note: I'm pretty sure I should need more checks here but 
        // oh well, the answer is correct.
        if ((i-(str.size-1)) >= 0 && 
           buffer_match(str, sub_line)) 
        {
          found = true;
          number += num;
          break;
        }
      }
      if (found)
        break;
    }
    sum += number;

  }
  printf("%u\n", sum);
}

static void aoc23_d2p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_str(filename, arena, false); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t game_id = 0;
  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    buffer_t line = stream_consume_line(s);  
    ++game_id;
    if (line.size == 0) {
      break;
    }
    
    // First real character starts after ": "
    u8_t* itr = line.e;
    while(dref(itr) != ':') {
      ++itr;
    }
    itr += 2; // For the space after ':'

    b32_t is_valid = true;
    while(1)
    {
      b32_t is_done = false;

      // Then we expect a number
      u32_t num = 0;
      while(u8_is_digit(dref(itr))) {
        num *= 10;
        num += ascii_to_digit(dref(itr));
        ++itr;
      }

      ++itr; // space
      
      // Then we expect a color 'r' 'g' or 'b'
      switch(dref(itr)) {
        case 'r': {
          if (num > 12) {
            is_done = true;
            is_valid = false;
          }
          itr += 3;
        }break;
        
        case 'g': {
          if (num > 13) {
            is_done = true;
            is_valid = false;
          }
          itr += 5;
        }break;

        case 'b': {
          if (num > 14) {
            is_done = true;
            is_valid = false;
          }
          itr += 4;
        }break;
      }

      // if it is not possible, just jump to the next line 
      if (is_done) break;

      // Then we look for comma or semicolon
      switch(dref(itr)) {
        case ';':
        case ',': {
          itr += 2;
        } break;
        default: {
          // Go to next line
          is_done = true;
        }
      }

      if (is_done) break;
    }

    if (is_valid)  {
      sum += game_id;
    }

  }
  printf("%u\n", sum);

}

static void aoc23_d2p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_str(filename, arena, false); 
  if (!file_buffer) return;

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t game_id = 0;
  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    buffer_t line = stream_consume_line(s);  
    ++game_id;
    if (line.size == 0) {
      break;
    }
    
    // First real character starts after ": "
    u8_t* itr = line.e;
    while(dref(itr) != ':') {
      ++itr;
    }
    itr += 2; // For the space after ':'

    b32_t is_valid = true;

    u32_t max_r = 0;
    u32_t max_g = 0;
    u32_t max_b = 0;
    // Parse the line
    while(1)
    {
      b32_t is_done = false;
      // Then we expect a number
      u32_t num = 0;
      while(u8_is_digit(dref(itr))) {
        num *= 10;
        num += ascii_to_digit(dref(itr));
        ++itr;
      }

      ++itr; // space
      
      // Then we expect a color 'r' 'g' or 'b'
      switch(dref(itr)) {
        case 'r': {
          max_r = max_of(max_r, num);
          itr += 3;
        }break;
        
        case 'g': {
          max_g = max_of(max_g, num);
          itr += 5;
        }break;

        case 'b': {
          max_b = max_of(max_b, num);
          itr += 4;
        }break;
      }

      // if it is not possible, just jump to the next line 
      if (is_done) break;

      // Then we look for comma or semicolon
      switch(dref(itr)) {
        case ';': {
          itr += 2;
        } break;

        case ',': {
          itr += 2;
        } break;
        default: {
          // Go to next line
          is_done = true;
        }
      }

      if (is_done) break;
    }

    if (is_valid)  {
      sum += max_r * max_g * max_b;
    }
    printf("%d power: %u\n",game_id, max_r * max_g * max_b);

  }
  printf("%u\n", sum);

}


static void aoc23_d3p1(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_str(filename, arena, false); 
  if (!file_buffer) return;


  // Get with width and height
  u32_t width = 0;
  u32_t height = 0;
  buffer_t grid;
  {  
    make(stream_t, s);
    stream_init(s, file_buffer);
    while(!stream_is_eos(s)) {
      buffer_t line = stream_consume_line(s);  
      ++height;
      width = line.size;
    }
    grid = arena_push_str(arena, width * height, 0);
    if (!grid) {
      printf("Cannot allocate memory\n");
      return;
    }

    stream_reset(s);
    // fill the grid
    u32_t grid_i = 0;
    while(!stream_is_eos(s)) {
      buffer_t line = stream_consume_line(s);  
      for_cnt(i, line.size) {
        if (u8_is_digit(line.e[i]) )
          grid.e[grid_i++] = ascii_to_digit(line.e[i]);
        else if (line.e[i] == '.')
          grid.e[grid_i++] = 11;
        else 
          grid.e[grid_i++] = 10;

      }
    }
  }


  auto get_index = [](buffer_t grid, u32_t x, u32_t y, u32_t w) {
    return grid.e[x + y * w];
  };
  auto check = [](buffer_t grid, s32_t x, s32_t y, s32_t w, s32_t h) {
    if (x < 0 || y < 0 || x >= w  || x >= h)
      return false;
    if (grid.e[x+y*w] == 10) {
      return true;
    }
    return false;
  };


  // Okay here we do the actual parsing
  u32_t current_num = 0;
  b32_t is_part = false; 
  b32_t is_num = false;
  u32_t sum = 0;
  for_cnt(y, height) 
  {
    for_cnt(x, width) 
    {
      u8_t c = get_index(grid, x, y, width);
      if(c >= 0 && c < 10) {
        current_num *= 10;
        current_num += c;
        is_num = true;
      }
      else {
        if (is_num) {
          if (is_part) {
            sum += current_num;
          }
          //printf("%d is %d\n", current_num, is_part);
          current_num = 0;
          is_part = false;
          is_num = false;
        }
      }

      if (is_num) {
        is_part = is_part || 
          check(grid, x-1, y-1, width, height) ||
          check(grid, x,   y-1, width, height) ||
          check(grid, x+1, y-1, width, height) ||
          check(grid, x-1, y,   width, height) ||
          check(grid, x+1, y,   width, height) ||
          check(grid, x-1, y+1, width, height) ||
          check(grid, x,   y+1, width, height) ||
          check(grid, x+1, y+1, width, height);
      }

      }
    // once we are done with the row, sum anything we found
      if (is_num) {
        if (is_part) {
          sum += current_num;
        }
        printf("%d is %d\n", current_num, is_part);
        current_num = 0;
        is_part = false;
        is_num = false;
    }
    
  }
  printf("%u\n", sum);

}

static void aoc23_d3p2(const char* filename, arena_t* arena) {
  arena_set_revert_point(arena);

  buffer_t file_buffer = file_read_into_str(filename, arena, false); 
  if (!file_buffer) return;


  // Initialize the grid data
  u32_t width = 0;
  u32_t height = 0;
  buffer_t grid;
  {  
    make(stream_t, s);
    stream_init(s, file_buffer);
    while(!stream_is_eos(s)) {
      buffer_t line = stream_consume_line(s);  
      ++height;
      width = line.size;
    }
    grid = arena_push_str(arena, width * height, 0);
    if (!grid) {
      printf("Cannot allocate memory\n");
      return;
    }

    stream_reset(s);
    // fill the grid
    u32_t grid_i = 0;
    while(!stream_is_eos(s)) {
      buffer_t line = stream_consume_line(s);  
      for_cnt(i, line.size) {
        if (u8_is_digit(line.e[i]) )
          grid.e[grid_i++] = ascii_to_digit(line.e[i]);
        else if (line.e[i] == '.')
          grid.e[grid_i++] = 11;
        else if (line.e[i] == '*')
          grid.e[grid_i++] = 12;
        else 
          grid.e[grid_i++] = 10;
      }
    }
  }

  // Useful methods for grid
  auto get_cell = [](buffer_t grid, u32_t x, u32_t y, u32_t w) {
    return grid.e[x + y * w];
  };


  // 
  // Actual work here
  //
  u32_t sum = 0;

  for_cnt(y, height) 
  {
    for_cnt(x, width) 
    {
      auto val = get_cell(grid, x, y, width);
      if (val == 12) // '*' value found
      {
        //printf("* found at %d %d\n", x, y);

        //
        // Here, we search for numbers around the '*' 
        //
        u32_t nums[2];
        u32_t num_count = 0;

        for (s32_t offset_y = -1; offset_y <= 1; ++offset_y) 
        {
          for(s32_t offset_x = -1; offset_x <= 1; ++offset_x) 
          {
            if (offset_x == 0 && offset_y == 0) continue;

            s32_t cur_x = x + offset_x;
            s32_t cur_y = y + offset_y;

            u32_t val = get_cell(grid,cur_x,cur_y,width);
            if (val < 10) {
              s32_t start_x = cur_x;
              s32_t end_x = cur_x;

              // find start point
              while(true)
              {
                u32_t cur_val = get_cell(grid, start_x, cur_y, width);
                if (cur_val >= 10) 
                {
                  ++start_x;
                  break;
                }
                --start_x;
                if(start_x < 0) 
                {
                  ++start_x;
                  break;
                }
              }


              while(true){
                u32_t cur_val = get_cell(grid, end_x, cur_y, width);
                if (cur_val >= 10) {
                  --end_x;
                  break;
                }
                ++end_x;
                if (end_x >= width) {
                  --end_x;
                  break;
                }
              }

              // Get the actual number
              u32_t actual_num = 0;
              for(u32_t i = start_x; i < end_x+1; ++i) 
              {
                u32_t cur_val = get_cell(grid, i, cur_y, width);
                actual_num = actual_num * 10 + cur_val;
              }

              // Shift offset forward based on end_x
              s32_t delta = end_x - cur_x;
              offset_x += delta;

              nums[num_count++] = actual_num;
              printf("num: %u\n", actual_num);
              if (num_count == 2) 
                goto aoc23_d3p2_escape; 
            }
          }
        }
aoc23_d3p2_escape:
        if (num_count == 2) {
          sum += nums[0] * nums[1];
          printf("value: %d\n", nums[0] * nums[1]);
        }
      }
    }
  }

  printf("%u\n", sum);

}


int main(int argv, char** argc) {

  if (argv < 2) {
    printf("Usage: aoc23 <day> <part> <filename>\nExample: aoc23 1 1 input.txt");
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

#define aoc23_route(dd, pp) if (day == dd && part == pp) aoc23_d ## dd ## p ## pp(filename, arena);
  aoc23_route(1,1);
  aoc23_route(1,2);
  aoc23_route(2,1);
  aoc23_route(2,2);
  aoc23_route(3,1);
  aoc23_route(3,2);

}
