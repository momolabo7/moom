/* date = January 27th 2022 9:18 am */

#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

#define assert_callback(s) printf("[test][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout); (*(volatile int*)0 = 0);


static unsigned test_log_spaces = 0;
#define test_assets_dir(filename) "../assets/test/" ##filename
#define test_eval_d(s) test_log(#s " = %d\n", s);
#define test_eval_lld(s) test_log(#s " = %lld\n", s);
#define test_eval_f(s) test_log(#s " = %f\n", s);
#define test_create_log_section_until_scope test_log_spaces += 2; defer {test_log_spaces -= 2;}

#define test_unit(unit_name) \
test_log(">> "#unit_name " start\n"); \
{ test_create_log_section_until_scope; unit_name; } \
test_log(">> " #unit_name " end\n\n"); \

#define test_log(...) { for(unsigned test_log_spaces_index = 0; test_log_spaces_index < test_log_spaces; ++test_log_spaces_index) { printf(" "); } printf(__VA_ARGS__); };

#include "momo.h"

static inline Block
test_read_file_to_memory(arena_t* allocator, const char* filename) {
  Block result = {0};
  FILE* file = fopen(filename, "rb");
  if (!file) { 
    test_log("Cannot find file\n");
    return result;
  }
  
  fseek(file, 0, SEEK_END);
  s32_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  void* file_memory = arena_push_size(allocator, file_size, 4);
  fread(file_memory, 1, file_size, file); 
  
  result.data = file_memory;
  result.size = file_size;
  
  fclose(file);
  
  return result;
}


static inline b32_t
test_write_memory_to_file(Block block, const char* filename) {
  FILE* file = fopen(filename, "wb");
  if (!file) {
    test_log("Cannot open file for writing\n");
    return false;
  }
  fwrite(block.data, sizeof(char), block.size, file);
  
  fclose(file);
  
  return true;
  
}

#if 0
#include "test_essentials.h"
#include "test_list.h"
#include "test_sort.h"
#include "test_png.h"
#endif
#include "test_ttf.h"

#endif //TEST_H
