#define FOOLISH 1

#include "momo.h"

#include <stdio.h>
#include <stdlib.h>

// 
// This will just consume all characters until the next \n or \r or 0. 
// The returned str_t will not include the \n or \r or 0. 
//
//
static void aoc22_d1p2(const char* filename) {

  str_t file_buffer = foolish_read_file_into_buffer(filename, true); 
  if (!file_buffer) return;
  defer { foolish_free_buffer(file_buffer); };
  
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


static void aoc22_d1p1(const char* filename) {
  str_t file_buffer = foolish_read_file_into_buffer(filename, true); 
  if (!file_buffer) return;
  defer { foolish_free_buffer(file_buffer); };
  
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

static void aoc22_d2p1(const char* filename) {
  str_t file_buffer = foolish_read_file_into_buffer(filename, false); 
  if (!file_buffer) return;
  defer { foolish_free_buffer(file_buffer); };

  
  make(stream_t, s);
  stream_init(s, file_buffer);

  // rock is 1
  // paper is 2
  // scissors is 3
  // lose is 0
  // draw is 3
  // 6 is win
  
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
 //   printf("%d <- %d vs %d -> %d\n", sum, lhs, rhs, score);

  }
  printf("%d\n", sum);
}

static void aoc22_d2p2(const char* filename) {
  str_t file_buffer = foolish_read_file_into_buffer(filename, false); 
  if (!file_buffer) return;
  defer { foolish_free_buffer(file_buffer); };

  
  make(stream_t, s);
  stream_init(s, file_buffer);

//  printf("%d\n", sum);
}


int main(int argv, char** argc) {

  if (argv < 2) {
    printf("Usage: aoc22 <day> <part> <filename>\nExample: aoc22 1 1 input.txt");
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

#define aoc22_route(dd, pp) if (day == dd && part == pp) aoc22_d ## dd ## p ## pp(filename);
  aoc22_route(1,1);
  aoc22_route(1,2);
  aoc22_route(2,1);

}
