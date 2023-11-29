#define FOOLISH 1

#include "momo.h"

#include <stdio.h>
#include <stdlib.h>

// 
// This will just consume all characters until the next \n or \r or 0. 
// The returned str_t will not include the \n or \r or 0. 
//
//
static str_t 
stream_consume_line(stream_t* s) {
  if (stream_is_eos(s)) return str_bad();

  str_t ret = str_set(s->contents.e + s->pos, 0);
  while(!stream_is_eos(s)) {
    
    u8_t current_value = dref(stream_consume_block(s, 1));
    if (current_value == 0 ) {
      break;
    }

    else if (current_value == '\r') {
      // We found a termination point. Do clean up.
      current_value = dref(stream_peek_block(s, 1));
      if (current_value == '\n') {
        // \r\n found
        stream_consume_block(s, 1);
      }
      break;
    }

    else if (current_value == '\n') {
      break;
    }

    else {
      ++ret.size;
    }

  }

  return ret;
}


static void aoc22_d1b(const char* filename) {

  str_t file_buffer = foolish_read_file_into_buffer(filename, true); 
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


static void aoc22_d1a(const char* filename) {
  str_t file_buffer = foolish_read_file_into_buffer(filename, true); 
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

int main(int argv, char** argc) {
  if (argv < 2) {
    printf("Fileename not provided");
    return 1;
  }

  aoc22_d1a(argc[1]);

}
