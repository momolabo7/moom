#define FOOLISH 1
#include "momo.h"



static void aoc23_d1p1(const char* filename) {
  str_t file_buffer = foolish_read_file_into_buffer(filename, false); 
  if (!file_buffer) return;
  defer { foolish_free_buffer(file_buffer); };

  make(stream_t, s);
  stream_init(s, file_buffer);

  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    str_t line = stream_consume_line(s);  
    u32_t number = 0;
    
    // count from the front
    for( s32_t i = 0; i <  line.size; ++i) {
      if (is_digit(line.e[i])) {
        number += ascii_to_digit(line.e[i]);
        number *= 10;
        break;
      }
    }

    // count from the back
    for( s32_t i = line.size-1; i >= 0; --i) {
      if (is_digit(line.e[i])) {
        number += ascii_to_digit(line.e[i]);
        break;
      }
    }
    sum += number;
  }
  printf("%u\n", sum);
}


static void aoc23_d1p2(const char* filename) {
  str_t file_buffer = foolish_read_file_into_buffer(filename, false); 
  if (!file_buffer) return;
  defer { foolish_free_buffer(file_buffer); };

  make(stream_t, s);
  stream_init(s, file_buffer);

  str_t str_table[] = { 
    str_from_lit("1"), 
    str_from_lit("2"), 
    str_from_lit("3"), 
    str_from_lit("4"), 
    str_from_lit("5"), 
    str_from_lit("6"), 
    str_from_lit("7"), 
    str_from_lit("8"),
    str_from_lit("9"), 
    str_from_lit("one"), 
    str_from_lit("two"), 
    str_from_lit("three"), 
    str_from_lit("four"), 
    str_from_lit("five"), 
    str_from_lit("six"), 
    str_from_lit("seven"), 
    str_from_lit("eight"), 
    str_from_lit("nine") 
  };
  u32_t num_table[] = { 1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9 };

  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    str_t line = stream_consume_line(s);  
    u32_t number = 0;

    // count from the front
    for( s32_t i = 0; i <  line.size; ++i) {
      b32_t found = false;
      for_arr(table_index, num_table) {
        str_t str = str_table[table_index];
        u32_t num = num_table[table_index];

        str_t sub_line = str_set(line.e + i, str.size);
        if ((i + str.size-1) < line.size && 
          !str_compare_lexographically(str, sub_line)) 
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
        str_t str = str_table[table_index];
        u32_t num = num_table[table_index];

        str_t sub_line = str_set(line.e + i - (str.size-1), str.size);
        // NOTE(momo): I'm pretty sure I should need more checks here but 
        // oh well, the answer is correct.
        if ((i-(str.size-1)) >= 0 && 
           !str_compare_lexographically(str, sub_line)) 
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

static void aoc23_d2p1(const char* filename) {
  str_t file_buffer = foolish_read_file_into_buffer(filename, false); 
  if (!file_buffer) return;
  defer { foolish_free_buffer(file_buffer); };

  make(stream_t, s);
  stream_init(s, file_buffer);
  u32_t game_id = 0;
  u32_t sum = 0;
  while(!stream_is_eos(s)) {
    str_t line = stream_consume_line(s);  
    ++game_id;
    
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
      while(is_digit(dref(itr))) {
        num *= 10;
        num += ascii_to_digit(dref(itr));
        ++itr;
      }
      printf("%u ", num);

      ++itr; // space
      
      // Then we expect a color 'r' 'g' or 'b'
      switch(dref(itr)) {
        case 'r': {
          printf("red ");
          if (num == 12) {
            is_done = true;
            is_valid = false;
          }
          itr += 3;
        }break;
        
        case 'g': {
          printf("green ");
          if (num == 13) {
            isdone = true
            is_valid = false;
          }
          itr += 5;
        }break;

        case 'b': {
          printf("bleu ");
          if (num == 14) {
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
        case ';': {
          printf("; ");
          itr += 2;
        } break;
        case ',': {
          printf(", ");
          itr += 2;
        } break;
        default: {
          // Go to next line
          is_done = true;
        }
      }

      if (is_done) break;
    }
    printf("\n");
    if (!is_valid) 
      sum += game_id;


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

  if (!str_to_u32(str_from_cstr(argc[1]), &day)) {
    printf("Bad day\n");
    return 1;
  }

  if (!str_to_u32(str_from_cstr(argc[2]), &part)) {
    printf("Bad part\n");
    return 1;
  }

#define aoc23_route(dd, pp) if (day == dd && part == pp) aoc23_d ## dd ## p ## pp(filename);
  aoc23_route(1,1);
  aoc23_route(1,2);
  aoc23_route(2,1);

}
